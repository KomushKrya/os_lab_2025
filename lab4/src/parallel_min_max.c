#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

// Глобальные переменные для хранения PID дочерних процессов
pid_t *child_pids = NULL;
int child_count = 0;
int timeout = 0;

// Обработчик сигнала SIGALRM
void alarm_handler(int sig) {
    printf("Timeout reached! Sending SIGKILL to all child processes.\n");
    for (int i = 0; i < child_count; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGKILL);
        }
    }
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;
    timeout = 0; // по умолчанию таймаут отключен

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 't'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "ft:", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        if (seed < 0) {
                            printf("Error: seed must be a non-negative number.\n");
                            return 1;
                        }
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        if (array_size <= 0) {
                            printf("Error: array_size must be a positive number.\n");
                            return 1;
                        }
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        if (pnum <= 0 || pnum > array_size) {
                            printf("Error: pnum must be a positive number and not greater than array_size.\n");
                            return 1;
                        }
                        break;
                    case 3:
                        with_files = true;
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                        break;
                }
                break;
            case 'f':
                with_files = true;
                break;
            case 't':
                timeout = atoi(optarg);
                if (timeout <= 0) {
                    printf("Error: timeout must be a positive number.\n");
                    return 1;
                }
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"seconds\"]\n", argv[0]);
        return 1;
    }

    // Выделяем память для хранения PID дочерних процессов
    child_pids = malloc(sizeof(pid_t) * pnum);
    child_count = pnum;

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    int pipes[pnum][2];
    char filename[256];

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Устанавливаем обработчик сигнала SIGALRM
    if (timeout > 0) {
        signal(SIGALRM, alarm_handler);
        alarm(timeout); // устанавливаем таймер
    }

    for (int i = 0; i < pnum; i++) {
        if (!with_files) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                return 1;
            }
        }
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            // successful fork
            active_child_processes += 1;
            child_pids[i] = child_pid; // сохраняем PID дочернего процесса
            if (child_pid == 0) {
                // child process

                // Определение диапазона для текущего процесса
                int start = i * (array_size / pnum);
                int end = (i == pnum - 1) ? array_size : (i + 1) * (array_size / pnum);
                struct MinMax local_min_max = GetMinMax(array, start, end);

                if (with_files) {
                    // Использование файлов для передачи данных
                    sprintf(filename, "result_%d.txt", i);
                    FILE *fp = fopen(filename, "w");
                    if (fp == NULL) {
                        perror("fopen");
                        exit(1);
                    }
                    fprintf(fp, "%d %d", local_min_max.min, local_min_max.max);
                    fclose(fp);
                } else {
                    // Использование pipe для передачи данных
                    close(pipes[i][0]); // Закрываем чтение
                    write(pipes[i][1], &local_min_max, sizeof(struct MinMax));
                    close(pipes[i][1]); // Закрываем запись
                }
                free(array);
                exit(0);
            }
        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    // Ожидание завершения дочерних процессов с использованием неблокирующего wait
    while (active_child_processes > 0) {
        int status;
        pid_t finished_pid = waitpid(-1, &status, WNOHANG);
        
        if (finished_pid > 0) {
            active_child_processes -= 1;
            // Обновляем массив child_pids
            for (int i = 0; i < child_count; i++) {
                if (child_pids[i] == finished_pid) {
                    child_pids[i] = 0; // помечаем как завершенный
                    break;
                }
            }
        } else if (finished_pid == 0) {
            // Есть еще работающие процессы, ждем немного
            usleep(10000); // 10ms
        } else {
            // Ошибка
            perror("waitpid");
            break;
        }
    }

    // Если таймаут был установлен, отменяем его
    if (timeout > 0) {
        alarm(0);
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        struct MinMax local_min_max;  
        if (with_files) {
            // Чтение из файлов
            sprintf(filename, "result_%d.txt", i);
            FILE *fp = fopen(filename, "r");
            if (fp == NULL) {
                perror("fopen");
                continue;
            }
            fscanf(fp, "%d %d", &local_min_max.min, &local_min_max.max);
            fclose(fp);
        } else {
            // Чтение из pipe
            close(pipes[i][1]);
            read(pipes[i][0], &local_min_max, sizeof(struct MinMax));
            close(pipes[i][0]);
        }

        if (local_min_max.min < min_max.min) min_max.min = local_min_max.min;
        if (local_min_max.max > min_max.max) min_max.max = local_min_max.max;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);
    free(child_pids);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    fflush(NULL);
    return 0;
}
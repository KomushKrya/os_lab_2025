#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("=== Демонстрация зомби-процесса ===\n\n");
    
    printf("1. Создаем дочерний процесс...\n");
    pid_t pid = fork();
    
    if (pid == 0) {
        printf("2. [Дочерний] PID=%d сразу завершается\n", getpid());
        exit(0);
    } else {
        printf("3. [Родитель] Дочерний PID=%d завершен, но wait() не вызывается\n", pid);
        printf("4. Процесс %d теперь ЗОМБИ (состояние Z)\n", pid);
        printf("5. Ждем 5 секунд...\n\n");
        
        sleep(5);
        
        printf("6. [Родитель] Вызываю wait() - убираю зомби\n");
        wait(NULL);
        printf("7. Зомби-процесс убран из системы!\n");
    }
    
    return 0;
}
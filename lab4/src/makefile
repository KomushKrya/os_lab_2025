CC = gcc
CFLAGS = -w -std=c99
TARGETS = parallel_min_max min_max zombie process_memory sum_utils libsum_utils static_sum

# Основная цель - компилирует все программы
all: $(TARGETS)

# Программа 1: Параллельный min/max с таймаутом
parallel_min_max: parallel_min_max.c find_min_max.h utils.h
	$(CC) $(CFLAGS) -o min_max parallel_min_max.c find_min_max.c utils.c

# Программа 2: Демонстрация зомби-процессов
zombie: zombie.c
	$(CC) $(CFLAGS) -o zombie zombie.c

# Программа 3: Анализ памяти процесса
process_memory: process_memory.c
	$(CC) $(CFLAGS) -o process_memory process_memory.c

sum_utils: sum_utils.c
	$(CC) $(CFLAGS) -c sum_utils.c -o sum_utils.o

libsum_utils: sum_utils.o
	ar rcs libsum_utils.a sum_utils.o

static_sum: parallel_sum.c utils.c libsum_utils.a
	$(CC) $(CFLAGS) parallel_sum.c utils.c -L. -lsum_utils -o parallel_sum

clean:
	rm -f $(TARGETS) *.o
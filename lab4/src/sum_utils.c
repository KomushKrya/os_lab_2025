#include "sum_utils.h"

int Sum(int *array, unsigned int begin, unsigned int end) {
    int sum = 0;
    for (unsigned int i = begin; i < end; i++) {
        sum += array[i];
    }
    return sum;
}
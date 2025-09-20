#include "revert_string.h"
#include "string.h"

void RevertString(char *str)
{
	if (str == NULL) return;
    
    int length = strlen(str);
    int start = 0;
    int end = length - 1;
    
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        
        start++;
        end--;
    }
}


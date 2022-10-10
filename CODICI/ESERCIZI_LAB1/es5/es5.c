#include <stdio.h>
#include <string.h>
#define false 0
#define true 1

int main(int argc, char **argv){
    int val = strnlen(argv[1], 256);
    printf("%d\n", val);
    return 0;
}
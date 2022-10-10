#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char **argv){
    char in = getchar();
    int count = 0;
    printf("%c\n",argv[1][0]);
    printf(" %s\n",argv[1]);
    while(in != EOF){
        printf("%c",in);
        if(in == argv[1][0]) count++;
        in = getchar();
    }
    printf("%d\n",count);
    return 0;
}
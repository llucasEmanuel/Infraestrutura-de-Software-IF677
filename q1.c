#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define true 1
#define false 0
typedef char bool;

int main() {

    FILE *file = fopen("a1.txt", "r");
    if (file == NULL) exit(1);
    char *seek = "qleberlicia";
    bool flag = false;

    while (!feof(file)) {
        char str[51];
        fscanf(file, " %50[^\n]", str);
        if (strcmp(str, seek) == 0) {
            flag = true;
            break;
        }
    }

    if (flag == true) {
        printf("%s esta no arquivo\n", seek);
    }
    else {
        printf("%s nao esta no arquivo\n", seek);
    }

    fclose(file);
    return 0;
}

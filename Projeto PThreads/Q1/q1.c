#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    char nome[51];
    FILE *arq;
    pthread_t th;
} Arquivo;

const char palavra[51] = "ratoeira";
int global_cont = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *contar_palavra(void *arg) {
    FILE *arq = (FILE *) arg;
    while (!feof(arq)) {
        char str[51];
        fscanf(arq, " %50[^\n]", str);
        if (strcmp(palavra, str) == 0) {
            pthread_mutex_lock(&mutex);
            global_cont++;
            pthread_mutex_unlock(&mutex);
        }
    }
}

int main() {
    
    int x; // numero de arquivos
    scanf("%d", &x);

    Arquivo *arqs = (Arquivo *) malloc(x * sizeof(Arquivo)); // array de x arquivos
    for (int i = 0; i < x; i++) {
        scanf(" %s", arqs[i].nome); // nao precisa digitar a extensao
        strcat(arqs[i].nome, ".txt");
        arqs[i].arq = fopen(arqs[i].nome, "r"); // retorna NULL se nao encontrar o arquivo
        pthread_create(&arqs[i].th, NULL, contar_palavra, (void *)arqs[i].arq);
    }

    // ** desalocar memoria no final **
    for (int i = 0; i < x; i++) {
        pthread_join(arqs[i].th, NULL); // espera a thread terminar
        fclose(arqs[i].arq);
    }
    free(arqs);
    printf("A palavra '%s' foi encontrada %d vez(es) nos arquivos\n", palavra, global_cont);
    return 0;
}

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int l;
    int r;
} Limit;

typedef struct {
    Limit lim;
    pthread_t th_ord;
} SubArray;

int array[] = {4, 9, 6, 1, 3, 10, 2, 7, 5, 8};
const int array_size = 10;
const int num_threads_ord = 2; // divide em 2 subarrays
SubArray subarrays[num_threads_ord]; // subarray de cada threae
pthread_t thread_merge;
pthread_barrier_t barreira; // sincronizar as ordenacoes parciais

void swap(int arr[], int ind) {
    int temp = arr[ind];
    arr[ind] = arr[ind+1];
    arr[ind+1] = temp;
}

void *bubble_sort(void *arg) {
    Limit lim = *((Limit *) arg);
    for (int i = lim.l; i <= lim.r - 1; i++) {
        for (int j = lim.l; j <= lim.r - 1 - i; j++) {
            swap(array, j);
        }
    }

    pthread_barrier_wait(&barreira); // espera as outras threads concluirem a ordenação
    pthread_exit(NULL);
}

int main() {

    pthread_barrier_init(&barreira, NULL, num_threads_ord);
    int sub_size = (array_size / num_threads_ord);
    for (int i = 0; i < num_threads_ord; i++) {
        subarrays[i].lim.l = i*sub_size;
        subarrays[i].lim.r = subarrays[i].lim.l + sub_size - 1;
        pthread_create(&subarrays[i].th_ord, NULL, bubble_sort, (void *) &subarrays[i].lim);
    }

    for (int i = 0; i < array_size; i++) {
        pthread_join(subarrays[i].th_ord, NULL);
    }
    
    for (int i = 0; i < array_size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    pthread_barrier_destroy(&barreira);
    return 0;
}

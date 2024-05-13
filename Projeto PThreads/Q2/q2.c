#define _XOPEN_SOURCE 600
#define NUM_THREADS_ORD 2
#define ARRAY_SIZE 10
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

int array[ARRAY_SIZE] = {4, 9, 6, 1, 3, 10, 2, 7, 5, 8};
SubArray subarrays[NUM_THREADS_ORD]; // subarray de cada threae
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
            if (array[j] > array[j+1]) 
                swap(array, j);
        }
    }


    pthread_barrier_wait(&barreira); // espera as outras threads concluirem a ordenação
    pthread_exit(NULL);
}

int main() {

    pthread_barrier_init(&barreira, NULL, NUM_THREADS_ORD);
    int sub_size = (ARRAY_SIZE / NUM_THREADS_ORD);
    for (int i = 0; i < NUM_THREADS_ORD; i++) {
        subarrays[i].lim.l = i*sub_size;
        subarrays[i].lim.r = subarrays[i].lim.l + sub_size - 1;
        pthread_create(&subarrays[i].th_ord, NULL, bubble_sort, (void *) &subarrays[i].lim);
    }

    for (int i = 0; i < NUM_THREADS_ORD; i++) {
        pthread_join(subarrays[i].th_ord, NULL);
    }
    
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    pthread_barrier_destroy(&barreira);
    return 0;
}

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

#define NUM_THREADS_ORD 4
#define ARRAY_SIZE 9

int array[ARRAY_SIZE] = {4, 9, 6, 1, 5, 3, 8, 2, 7};

typedef struct {
    int *sub; // ponteiro pro inicio do subarray
    int tam; // tamanho do subarray
} SubArray;

pthread_barrier_t barrier;

void *bubble_sort(void *arg) {
    SubArray *subarr = (SubArray *) arg;

    for (int i = 0; i < subarr->tam - 1; i++) {
        for (int j = 0; j < subarr->tam - 1 - i; j++) {
            if (subarr->sub[j] > subarr->sub[j+1]) {
                int temp = subarr->sub[j];
                subarr->sub[j] = subarr->sub[j+1];
                subarr->sub[j+1] = temp;
            }
        }
    }

    pthread_barrier_wait(&barrier);
}

// Criar a Heap Minima
typedef struct {
    int size;
    int *heap;
} Heap;

void heapify(Heap *h) { // heapify para min_heap
    int lim = (int) (h->size / 2);
    for (int i = lim; i >= 1; i--) {
        int k = i;
        int v = h->heap[k];
        while (2*k <= h->size) {
            int j = 2*k;
            if (j < h->size) {
                if (h->heap[j] >= h->heap[j+1]) {
                    j++;
                }
            }
            if (v < h->heap[j]) {
                break;
            }
            else {
                h->heap[k] = h->heap[j];
                k = j;
            }
        }
        h->heap[k] = v;
    }
}

Heap init_heap(int *arr, int size) { // int *arr é o ptr pro primeiro elemento do subarray (n criar variaveis subarray, apenas usar o array global)
    Heap h;
    h.size = size;
    h.heap = (int *) malloc((size+1) * sizeof(int));
    h.heap[0] = 0; // "header" da heap inicializado com o valor 0
    for (int i = 1; i <= size; i++) {
        h.heap[i] = arr[i-1];
    }
    heapify(&h);
    return h;
}

int get_min(Heap h) {
    if (h.size != 0) {
        return h.heap[1];
    }
    return -1;
}

int delete_min(Heap *h) {
    if (h->size > 0) {
        int min = h->heap[1];
        h->heap[1] = h->heap[h->size];
        h->size--;
        heapify(h);
        return min;
    }
    perror("Erro: delete_min em heap vazia\n");
    return -1; 
}

void print_heap(Heap h) {
    for (int i = 1; i <= h.size; i++) {
        printf("%d ", h.heap[i]);
    }
    printf("\n");
}

void *merge(void *arg) {
    Heap *heaps = (Heap *)arg;
    int count = 0;
    int index = 0;
    while (count < ARRAY_SIZE) {
        int menor = INT_MAX;
        for (int i = 0; i < NUM_THREADS_ORD; i++) {
            if (heaps[i].size > 0) {
                if(get_min(heaps[i]) < menor) {
                    menor = get_min(heaps[i]);
                    index = i;
                }
            }
        }
        array[count] = delete_min(&heaps[index]);
        count++;
    }
}

int main() {
    pthread_barrier_init(&barrier, NULL, NUM_THREADS_ORD);

    SubArray subarrays[NUM_THREADS_ORD];
    pthread_t threads[NUM_THREADS_ORD];

    int sub_size = ARRAY_SIZE / NUM_THREADS_ORD; // tamanho dos subarr uniformes
    int mod = ARRAY_SIZE % NUM_THREADS_ORD; // para achar o tamanho do ultimo é só usar o tamanho do array mod num threads -> se for diferente de zero considera ele (tam array n divisivel por num threads)
    int last_size = sub_size + mod;
    for (int i = 0; i < NUM_THREADS_ORD-1; i++) { // so considerar para a ultima thread
        subarrays[i].tam = sub_size;
        subarrays[i].sub = array + (subarrays[i].tam * i);
        int err = pthread_create(&threads[i], NULL, bubble_sort, (void *) &subarrays[i]);
        if (err != 0) {
            printf("Erro ao criar thread %d\n", i + 1);
            exit(1);
        }
    }
    // cria a ultima thread separado
    subarrays[NUM_THREADS_ORD-1].tam = last_size;
    subarrays[NUM_THREADS_ORD-1].sub = array + (sub_size * (NUM_THREADS_ORD - 1)); // talvez multiplicar pelo NUM_THREADS_ORD - 1 esteja errado
    int er = pthread_create(&threads[NUM_THREADS_ORD-1], NULL, bubble_sort, (void *) &subarrays[NUM_THREADS_ORD-1]);
    if (er != 0) {
        printf("Erro ao criar thread %d de ordenação\n", NUM_THREADS_ORD);
        exit(1);
    }

    for (int i = 0; i < NUM_THREADS_ORD; i++) {
        pthread_join(threads[i], NULL);
    }

    // passar os subarrays para as heaps
    Heap heaps[NUM_THREADS_ORD];
    for (int i = 0; i < NUM_THREADS_ORD; i++) {
        heaps[i] = init_heap(subarrays[i].sub, subarrays[i].tam);
    }

    pthread_t th_merge;
    int err = pthread_create(&th_merge, NULL, merge, heaps);
    if (err != 0) printf("Erro ao criar thread de merge\n"), exit(1);
    pthread_join(th_merge, NULL); // espera da o merge

    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    pthread_barrier_destroy(&barrier);

    for (int i = 0; i < NUM_THREADS_ORD; i++) {
        free(heaps[i].heap);
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_THREADS 10

pthread_mutex_t matrix_B_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t matrix_C_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t buffer_semaphore;  // Semaphore for buffer access
sem_t result_matrix_semaphore;  // Semaphore for result matrix access

typedef struct {
    int row_start;
    int row_end;
    int* buffer;
    int n1, m1, n2, m2;
    int* B;
    int* C;
    int* A;
} ThreadArgs;

void* producer(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;

    for (int row = tArgs->row_start; row < tArgs->row_end; ++row) {
        for (int col = 0; col < tArgs->m2; ++col) {
            int sum = 0;
            for (int i = 0; i < tArgs->m1; ++i) {
                pthread_mutex_lock(&matrix_B_mutex);
                pthread_mutex_lock(&matrix_C_mutex);

                sum += tArgs->B[row * tArgs->m1 + i] * tArgs->C[i * tArgs->m2 + col];

                pthread_mutex_unlock(&matrix_C_mutex);
                pthread_mutex_unlock(&matrix_B_mutex);
            }

            sem_wait(&buffer_semaphore);

            tArgs->buffer[row * tArgs->m2 + col] = sum;

            sem_post(&buffer_semaphore);
        }
    }

    return NULL;
}

void* consumer(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;

    sem_wait(&result_matrix_semaphore);

    for (int row = tArgs->row_start; row < tArgs->row_end; ++row) {
        for (int col = 0; col < tArgs->m2; ++col) {
            sem_wait(&buffer_semaphore);

            tArgs->A[row * tArgs->m2 + col] = tArgs->buffer[row * tArgs->m2 + col];

            sem_post(&buffer_semaphore);
        }
    }

    sem_post(&result_matrix_semaphore);

    return NULL;
}
int main() {
    int n1, m1, n2, m2;
    printf("***********************************");
    printf("\n"); 
    printf("Bissmi allah");
    printf("\n"); 
    printf("***********************************");
    printf("\n"); 
    printf("                 ***********************************");
    printf("\n");
    printf("                    projet système d'exploitation");
    printf("\n");
    printf("                 ***********************************");
    printf("\n");
    printf("groupe 03:Terkmani sara");
    printf("\n");
    printf("          Habbas tassadit");
    printf("\n");
    printf("\n");
    printf("Enter the dimensions of matrix B (n1 m1): ");
    scanf("%d %d", &n1, &m1);

    printf("Enter the dimensions of matrix C (n2 m2): ");
    scanf("%d %d", &n2, &m2);
    // Initialize semaphores
    sem_init(&buffer_semaphore, 0, 1);  // Initial value set to 1 (mutex-like)
    sem_init(&result_matrix_semaphore, 0, 1);  // Initial value set to 1 (mutex-like)

    int N = n1 * m2; // Size of the one-dimensional buffer
    pthread_t producers[MAX_THREADS];
    pthread_t consumers[MAX_THREADS];
    int buffer[MAX_THREADS * MAX_THREADS]; // One-dimensional buffer to store intermediate results
    int B[n1 * m1], C[n2 * m2], A[n1 * m2]; // Matrices B, C, and resultant A

    // Initialize matrices B and C with random values
    for (int i = 0; i < n1 * m1; ++i) {
        B[i] = rand() % 10;
    }

    for (int i = 0; i < n2 * m2; ++i) {
        C[i] = rand() % 10;
    }

    printf("**********Matrix B:*****************\n");
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < m1; ++j) {
            printf("%d ", B[i * m1 + j]);
        }
        printf("\n");
    }

    printf("**********Matrix C:*******************\n");
    for (int i = 0; i < n2; ++i) {
        for (int j = 0; j < m2; ++j) {
            printf("%d ", C[i * m2 + j]);
        }
        printf("\n");
    }

    // Create producer threads
    int threads = (n1 < MAX_THREADS) ? n1 : MAX_THREADS;
    int rows_per_thread = n1 / threads;
    for (int i = 0; i < threads; ++i) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->row_start = i * rows_per_thread;
        args->row_end = (i == (threads - 1)) ? n1 : (i + 1) * rows_per_thread;
        args->buffer = buffer;
        args->n1 = n1;
        args->m1 = m1;
        args->n2 = n2;
        args->m2 = m2;
        args->B = B;
        args->C = C;
        args->A = A;
        pthread_create(&producers[i], NULL, producer, args);
    }

    // Wait for producer threads to finish
    for (int i = 0; i < threads; ++i) {
        pthread_join(producers[i], NULL);
    }

    // Create consumer threads
    for (int i = 0; i < threads; ++i) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->row_start = i * rows_per_thread;
        args->row_end = (i == (threads - 1)) ? n1 : (i + 1) * rows_per_thread;
        args->buffer = buffer;
        args->n1 = n1;
        args->m1 = m1;
        args->n2 = n2;
        args->m2 = m2;
        args->B = B;
        args->C = C;
        args->A = A;
        pthread_create(&consumers[i], NULL, consumer, args);
    }

    // Wait for consumer threads to finish
    for (int i = 0; i < threads; ++i) {
        pthread_join(consumers[i], NULL);
    }
     // Destroy semaphores
    sem_destroy(&buffer_semaphore);
    sem_destroy(&result_matrix_semaphore);

    // Display the buffer T[N]
    printf("***********Buffer T[N]:*****************\n");
    for (int i = 0; i < N; ++i) {
        printf("%d ", buffer[i]);
    }
    printf("\n");

    // Display the resultant matrix A
    printf("*************Resultant Matrix A:*********\n");
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < m2; ++j) {
            printf("%d ", A[i * m2 + j]);
        }
        printf("\n");
    }

    return 0;
}


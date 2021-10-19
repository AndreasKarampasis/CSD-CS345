/*
 * Andreas Karampasis csd3777
 * hy345
 * askisi 2 - threads & semaphores
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sem_t lock, chef_sem, client_sem;
int customers_waiting = 0;

void *chef(void *n);

void *customer(void *ptr);

void serve_customer();

int main(int argc, char *argv[]) {
  int i, *rc, n;
  pthread_t *threads;

  if (argc != 2) {
    printf("Usage:<executable> <number-of-clients>\n");
    return 1;
  }
  n = atoi(argv[1]);
  rc = (int *)malloc(n * sizeof(int));
  if (!rc) {
    fprintf(stderr, "ERROR: Not enough memory allocating rc\n");
  }
  threads = (pthread_t *)malloc(sizeof(pthread_t));
  srand(time(NULL));
  sem_init(&lock, 0, 1);
  sem_init(&client_sem, 0, 0);
  sem_init(&chef_sem, 0, 1);
  rc[0] = pthread_create(&threads[0], NULL, chef, NULL);
  if (rc[0] != 0) {
    puts("ERROR: Could not initialise chef thread.");
  }
  for (i = 1; i < n + 1; ++i) {
    rc[i] = pthread_create(&threads[i], NULL, customer, &i);

    if (rc[i] != 0) {
      puts("ERROR: Could not initialise client thread.");
    }
    if ((i - 1) % 3 == 0) {
      sleep(3);
    }
  }
  // perimenoyme ta threads gia customers na teleiwsoyn
  for (i = 1; i < n + 1; ++i) {
    pthread_join(threads[i], NULL);
  }
  sem_destroy(&lock);
  sem_destroy(&client_sem);
  sem_destroy(&chef_sem);

  return 0;
}

void *customer(void *n) {
  sem_wait(&lock);
  puts("A new customer is waiting.");
  // Entering critical section
  ++customers_waiting;
  sem_post(&client_sem);
  sem_wait(&chef_sem);
  // Leaving critical section
  sem_post(&lock);
  pthread_exit(NULL);
}

void *chef(void *n) {
  while (1) {
    if (!customers_waiting) {
      printf("Chef is on Facebook.\n");
    }
    sem_wait(&client_sem);
    serve_customer();
    sem_post(&chef_sem);
  }
  return NULL;
}

void serve_customer() {
  printf("Chef is serving a customer.\n");
  --customers_waiting;
}

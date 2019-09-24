#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// Note the argument and return types: void*
void* fn(void* args){
  return NULL;
}

// Global variables
long global_num = 0;
static sem_t semaphore;
static sem_t initSem;
pthread_mutex_t fork1;
pthread_mutex_t fork2;
pthread_mutex_t fork3;

// Declaration
void* taskA_thread_1(void* arg) {
  for (int i = 0; i < 50*1000*10; i++) {
    (*(long*)arg)++;
    global_num++;
  }
  printf(" Thread 1 local long is: %ld\n\r", (*(long*)arg));
  return NULL;
}

void* taskA_thread_2(void* arg) {
  for (int i = 0; i < 50*1000*10; i++) {
    (*(long*)arg)++;
    global_num++;
  }
  printf(" Thread 2 local long is: %ld\n\r", (*(long*)arg));
  return NULL;
}

void* taskC_thread_1(void* arg) {
  long local = 0;
  for (int i = 0; i < 50*1000*10; i++) {
    local++;
    sem_wait(&semaphore);
    global_num++;
    sem_post(&semaphore);
  }
  printf(" Thread 1 local long is: %ld\n\r", local);
  return NULL;
}

void* taskC_thread_2(void* arg) {
  long local = 0;
  for (int i = 0; i < 50*1000*10; i++) {
    local++;
    sem_wait(&semaphore);
    global_num++;
    sem_post(&semaphore);
  }
  printf(" Thread 2 local long is: %ld\n\r", local);
  return NULL;
}

void* philosopher_1(void* arg) {
  pthread_mutex_lock(&fork1);
	pthread_mutex_lock(&fork3);
	printf("philosopher_1 is currently eating\n");
	sleep(5);
	pthread_mutex_unlock(&fork1);
	pthread_mutex_unlock(&fork3);
  return NULL;
}

void* philosopher_2(void* arg) {
  pthread_mutex_lock(&fork1);
	pthread_mutex_lock(&fork2);
	printf("philosopher_2 is currently eating\n");
	sleep(5);
	pthread_mutex_unlock(&fork1);
	pthread_mutex_unlock(&fork2);
  return NULL;
}

void* philosopher_3(void* arg) {
  pthread_mutex_lock(&fork2);
	pthread_mutex_lock(&fork3);
	printf("philosopher_3 is currently eating\n");
	sleep(5);
	pthread_mutex_unlock(&fork2);
	pthread_mutex_unlock(&fork3);
  return NULL;
}

int main(){
  long *local = malloc(sizeof(*local));
  *local = 0;

  //pthread_t thread_1;
  //pthread_t thread_2;
  
  pthread_t philosopher_1_thread;
  pthread_t philosopher_2_thread;
  pthread_t philosopher_3_thread;
  long result_1, result_2;

  sem_init(&semaphore,0,1);
  pthread_mutex_init(&fork1,NULL);
	pthread_mutex_init(&fork2,NULL);
	pthread_mutex_init(&fork3,NULL);

  //pthread_create(&thread_1, NULL, &taskA_thread_1, local);    // remember to switch tasks!
  //pthread_create(&thread_2, NULL, &taskA_thread_2, local);
  
  pthread_create(&philosopher_1_thread, NULL, &philosopher_1, local);    // remember to switch tasks!
  pthread_create(&philosopher_2_thread, NULL, &philosopher_2, local);
  pthread_create(&philosopher_3_thread, NULL, &philosopher_3, local);    // remember to switch tasks!

  //pthread_join(thread_1, NULL);
  //pthread_join(thread_2, NULL);
  pthread_join(philosopher_1_thread, NULL);
  pthread_join(philosopher_2_thread, NULL);
  pthread_join(philosopher_3_thread, NULL);
  
  pthread_mutex_destroy(&fork1);
  pthread_mutex_destroy(&fork2);
  pthread_mutex_destroy(&fork3);
  sem_destroy(&semaphore);
  //printf("The Global long number is: %ld\n\r", global_num);
}

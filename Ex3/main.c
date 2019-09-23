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
//pthread_mutex_t mtx;

static struct Singleton* g_singleton = NULL;

struct Singleton* getSingleton(){
  if(!g_singleton){
    sem_wait(&initSem);
    if(!g_singleton){
      g_singleton = malloc(sizeof(struct Singleton));
      // Performing some initialization...
    }
    sem_post(&initSem);
  }
  return g_singleton;
}

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

void* taskF_thread_1(void* arg) {
  long local = 0;
  for (int i = 0; i < 50*1000*10; i++) {
    local++;
    //pthread_mutex_lock(&mtx);
    global_num++;
    //pthread_mutex_unlock(&mtx);
  }
  printf(" Thread 1 local long is: %ld\n\r", local);
  return NULL;
}

void* taskF_thread_2(void* arg) {
  long local = 0;
  for (int i = 0; i < 50*1000*10; i++) {
    local++;
    //pthread_mutex_lock(&mtx);
    global_num++;
    //pthread_mutex_unlock(&mtx);
  }
  printf(" Thread 2 local long is: %ld\n\r", local);
  return NULL;
}

int main(){
  long *local = malloc(sizeof(*local));
  *local = 0;

  pthread_t thread_1;
  pthread_t thread_2;
  long result_1, result_2;

  sem_init(&semaphore,0,1);
  //pthread_mutex_init(&mtx,NULL);

  pthread_create(&thread_1, NULL, &taskA_thread_1, local);    // remember to switch tasks!
  pthread_create(&thread_2, NULL, &taskA_thread_2, local);
  pthread_join(thread_1, NULL);
  pthread_join(thread_2, NULL);
  //pthread_mutex_destroy(&mtx);
  sem_destroy(&semaphore);
  printf("The Global long number is: %ld\n\r", global_num);
}

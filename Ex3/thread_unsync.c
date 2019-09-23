#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Note the argument and return types: void*
void* fn(void* args){
  return NULL;
}

// Declaration
void* taskA_thread_1(void* arg);
void* taskA_thread_2(void* arg);

long global_num = 0;

int main(){
  long *local = malloc(sizeof(*local));
  *local = 0;

  pthread_t thread_1;
  pthread_t thread_2;
  long result_1, result_2;

  pthread_create(&thread_1, NULL, &taskA_thread_1, local);
  pthread_create(&thread_2, NULL, &taskA_thread_2, local);
  pthread_join(thread_1, NULL);
  pthread_join(thread_2, NULL);
  printf("The Global long number is: %ld\n\r", global_num);
}

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

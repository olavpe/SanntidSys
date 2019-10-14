#include "io.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>

/// Assigning CPU core ///


// Declaration
void delay(int milliseconds);

void* taskA_thread_1(void* arg) {
  while(1){
    if (io_read(1) == 0){
      io_write(1,0);
      usleep(5);
      io_write(1,1);
    } 
  }
}

void* taskA_thread_2(void* arg) {
  while (1) {
    if (io_read(2) == 0){
      io_write(2, 0);
      usleep(5);
      io_write(2, 1);
    } 
  }
}

void* taskA_thread_3(void* arg) {
  while (1){
    if (io_read(3) == 0){
      io_write(3,0);
      usleep(5);
      io_write(3,1);
    }
  }
}

int main(){
  io_init();

  pthread_t thread_1;
  pthread_t thread_2;
  pthread_t thread_3;
  pthread_create(&thread_1,NULL,&taskA_thread_1,NULL);
  pthread_create(&thread_2,NULL,&taskA_thread_2,NULL);
  pthread_create(&thread_3,NULL,&taskA_thread_3,NULL);
  pthread_join(thread_1,NULL);
  pthread_join(thread_2,NULL);
  pthread_join(thread_3,NULL);

  return 0;
}

void delay(int milliseconds) {
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}
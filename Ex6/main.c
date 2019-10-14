#define _GNU_SOURCE
#include "io.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

/// 'struct timespec' functions ///

struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

/// Assigning CPU core ///

int set_cpu(int cpu_number){
  cpu_set_t cpu;
  CPU_ZERO(&cpu);
  CPU_SET(cpu_number, &cpu);
  return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

// Declaration
void delay(int milliseconds);

void* taskA_thread_1(void* arg) {
  struct timespec waketime;
  clock_gettime(CLOCK_REALTIME, &waketime);
  struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};
  set_cpu(0);
  while(1){
    if (io_read(1) == 0){
      io_write(1,0);
      usleep(80);
      io_write(1,1);
      
    } 
    waketime = timespec_add(waketime, period);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
  }
}

void* taskA_thread_2(void* arg) {
  struct timespec waketime;
  clock_gettime(CLOCK_REALTIME, &waketime);
  struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};
  set_cpu(0);
  while (1) {
    if (io_read(2) == 0){
      io_write(2, 0);
      usleep(80);
      io_write(2, 1);
    } 
    waketime = timespec_add(waketime, period);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
  }
}

void* taskA_thread_3(void* arg) {
  struct timespec waketime;
  clock_gettime(CLOCK_REALTIME, &waketime);
  struct timespec period = {.tv_sec = 0, .tv_nsec = 1*1000*1000};
  set_cpu(0);
  while (1){
    if (io_read(3) == 0){
      io_write(3,0);
      usleep(80);
      io_write(3,1);
    }
    waketime = timespec_add(waketime, period);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
  }
}

void* disturbance_thread(void* arg){
  set_cpu(0);
  while(1){
    asm volatile("" ::: "memory");
  }
}


int main(){
  io_init();

  pthread_t thread_1;
  pthread_t thread_2;
  pthread_t thread_3;
  //pthread_t disturbance[10];
  pthread_create(&thread_1,NULL,&taskA_thread_1,NULL);
  pthread_create(&thread_2,NULL,&taskA_thread_2,NULL);
  pthread_create(&thread_3,NULL,&taskA_thread_3,NULL);
  /*
  for (int i = 0; i < 10; i++){
      pthread_create(&disturbance[i], NULL, &disturbance_thread,NULL);
  }
  */
  pthread_join(thread_1,NULL);
  pthread_join(thread_2,NULL);
  pthread_join(thread_3,NULL);
  /*
  for (int i = 0; i < 10; i++){
    pthread_join(disturbance[i],NULL);
  }
  */
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

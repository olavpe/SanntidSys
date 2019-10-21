#include "io.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <sched.h>
#include <unistd.h>

/// Assigning CPU core ///

int set_cpu(int cpu_number){
  cpu_set_t cpu;
  CPU_ZERO(&cpu);
  CPU_SET(cpu_number, &cpu);
  return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

RT_TASK periodic_RT_task;
RT_TASK taskA_RT_thread_1;
RT_TASK taskA_RT_thread_2;
RT_TASK taskA_RT_thread_3;


void* periodic_task(){
	while(1){
		rt_printf("periodic print \n\r");
		rt_task_wait_period(NULL);
	}
}

void* taskA_thread_1(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 10 second timeout
  unsigned long endTime = rt_timer_read() + duration;

  while(1){
    if (io_read(1) == 0){
      io_write(1,0);
      rt_timer_spin(80000);
      io_write(1,1);
    } 
    if(rt_timer_read() > endTime){
    	rt_printf("Time expired\n");
		rt_task_delete(NULL);
	}
	if(rt_task_yield()){
		rt_printf("Task failed to yield\n");
		rt_task_delete(NULL);
	}
  }
}

void* taskA_thread_2(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 10 second timeout
  unsigned long endTime = rt_timer_read() + duration;

  while (1) {
    if (io_read(2) == 0){
      io_write(2, 0);
      rt_timer_spin(80000);
      io_write(2, 1);
    } 
    if(rt_timer_read() > endTime){
    	rt_printf("Time expired\n");
		rt_task_delete(NULL);
	}
	if(rt_task_yield()){
		rt_printf("Task failed to yield\n");
		rt_task_delete(NULL);
	}
  }
}

void* taskA_thread_3(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 10 second timeout
  unsigned long endTime = rt_timer_read() + duration;

  while (1){
    if (io_read(3) == 0){
      io_write(3,0);
      rt_timer_spin(80000);
      io_write(3,1);
    }
    if(rt_timer_read() > endTime){
    	rt_printf("Time expired\n");
		rt_task_delete(NULL);
	}
	if(rt_task_yield()){
		rt_printf("Task failed to yield\n");
		rt_task_delete(NULL);
	}
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
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_print_auto_init(1);

	pthread_t disturbance[10];

  	for (int i = 0; i < 10; i++){
  	    pthread_create(&disturbance[i], NULL, &disturbance_thread,NULL);
  	}
 	
	/*
	rt_task_create(&periodic_RT_task, "periodic test task", 0, 50, T_CPU(0));
	rt_task_set_periodic(&periodic_RT_task, TM_NOW, (2.67 * 1000 * 1000 * 1000));
	rt_task_start(&periodic_RT_task, &periodic_task, NULL);
	*/
	rt_task_create(&taskA_RT_thread_1, "task1", 0, 50, T_CPU(0));
	rt_task_create(&taskA_RT_thread_2, "task2", 0, 50, T_CPU(0));
	rt_task_create(&taskA_RT_thread_3, "task3", 0, 50, T_CPU(0));

	rt_task_start(&taskA_RT_thread_1, &taskA_thread_1, NULL);
	rt_task_start(&taskA_RT_thread_2, &taskA_thread_2, NULL);
	rt_task_start(&taskA_RT_thread_3, &taskA_thread_3, NULL);
	//while(1){};
	
  	for (int i = 0; i < 10; i++){
   	 pthread_join(disturbance[i],NULL);
 	}
  
	return 0;
}
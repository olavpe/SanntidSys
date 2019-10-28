#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
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

typedef struct Resource {
  RT_MUTEX*  mutex;
  int       priority;
} Resource;

typedef struct ThreadPriorities {
  RT_TASK*   task;
  int       base;
  int       boost;
} ThreadPriorities;

RT_TASK periodic_RT_task;
RT_TASK taskA_RT_thread_1;
RT_TASK taskA_RT_thread_2;
RT_TASK taskA_RT_thread_3;
RT_TASK taskA_RT_thread_4;
RT_TASK taskD_RT_thread_1;
RT_TASK taskD_RT_thread_2;
RT_MUTEX mutex;
RT_MUTEX mutexA;
RT_MUTEX mutexB;
RT_SEM semaphore;
struct Resource resourceA = { .mutex = &mutexA, .priority = 95};
struct Resource resourceB = { .mutex = &mutexB, .priority = 96};
struct ThreadPriorities thread1_str = { .task = &taskD_RT_thread_1, .base = 70};
struct ThreadPriorities thread2_str = { .task = &taskD_RT_thread_2, .base = 80};

void busy_wait_us(unsigned long delay){
  for(; delay > 0; delay--){
    rt_timer_spin(1000);
  }
}

void icpp_lock(Resource *resc, ThreadPriorities *task_str){
  rt_task_set_priority(task_str->task, resc->priority);
  rt_mutex_acquire(resc->mutex, TM_INFINITE);
  task_str->boost = resc->priority;
}

void icpp_unlock(struct Resource *resc, struct ThreadPriorities *task_str){
  rt_task_set_priority(task_str->task,task_str->base);
  rt_mutex_release(resc->mutex);
}

void* periodic_task(){
	RTIME tstart = rt_timer_read();
	while(1){
		rt_printf("periodic print. Loop time: %.5f ms\n\r", (rt_timer_read() - tstart)/1000000.0);
		rt_task_wait_period(NULL);
	}
}

void* taskA_thread_1(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 30 second timeout
  unsigned long endTime = rt_timer_read() + duration;

//	rt_task_wait_period(NULL);
  //Barrier synchronization
  rt_sem_p(&semaphore,TM_INFINITE);
  rt_sem_v(&semaphore);
  rt_printf("Task1: Waiting and releasing semaphore\n\r");

  // Low priority task that locks the resource
  rt_printf("Task1 locking semaphore\n");
  rt_mutex_acquire(&mutex,TM_INFINITE);
  rt_printf("Task1 busy wait start\n");
  busy_wait_us(300000);
  rt_printf("Task1 busy wait stopped\n");
  rt_mutex_release(&mutex);
  rt_printf("Task1 releasing mutex\n");
}

void* taskA_thread_2(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 30 second timeout
  unsigned long endTime = rt_timer_read() + duration;

  rt_sem_p(&semaphore,TM_INFINITE);
  rt_sem_v(&semaphore);
  rt_printf("Task2: Waiting and releasing semaphore\n\r");

  // Medium priority 
  rt_task_sleep(100000000);
  rt_printf("Task2 busy wait start\n");
  busy_wait_us(500000);
  rt_printf("Task2 busy wait stopped\n");
}

void* taskA_thread_3(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 30 second timeout
  unsigned long endTime = rt_timer_read() + duration;

  rt_sem_p(&semaphore,TM_INFINITE);
  rt_sem_v(&semaphore);
  rt_printf("Task3: Waiting and releasing semaphore\n\r");

  // Medium priority 
  rt_task_sleep(200000000);
  rt_mutex_acquire(&mutex,TM_INFINITE);
  rt_printf("Task3 busy wait start\n");
  busy_wait_us(200000);
  rt_mutex_release(&mutex);
  rt_printf("Task3 busy wait stopped\n");
}

void* taskD_thread_1(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 30 second timeout
  unsigned long endTime = rt_timer_read() + duration;

//	rt_task_wait_period(NULL);
  //Barrier synchronization
  rt_printf("Task1: starting semaphore\n\r");
  rt_sem_p(&semaphore,TM_INFINITE);
  rt_printf("Task1: Waiting and releasing semaphore\n\r");

  // Low priority task that locks the resource
  icpp_lock(&resourceA, &thread1_str);
  rt_printf("Task1 locking mutexA\n");

  rt_printf("Task1 busy wait start\n");
  busy_wait_us(300000);
  rt_printf("Task1 busy wait stopped\n");

  icpp_lock(&resourceB, &thread1_str);
  rt_printf("Task1 locking mutexB\n");
  rt_printf("Task1 busy wait start\n");
  busy_wait_us(300000);
  rt_printf("Task1 busy wait stopped\n");

  icpp_unlock(&resourceB, &thread1_str);
  rt_printf("Task1 releasing mutexB\n");

  icpp_unlock(&resourceA, &thread1_str);
  rt_printf("Task1 releasing mutexA\n");
  busy_wait_us(100000);
}

void* taskD_thread_2(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 30 second timeout
  unsigned long endTime = rt_timer_read() + duration;

//	rt_task_wait_period(NULL);
  //Barrier synchronization
  rt_sem_p(&semaphore,TM_INFINITE);
  rt_printf("Task2: Waiting and releasing semaphore\n\r");

  // Low priority task that locks the resource
  rt_task_sleep(100000000);
  icpp_lock(&resourceB, &thread2_str);
  rt_printf("Task2 locking mutexB\n");

  rt_printf("Task2 busy wait start\n");
  busy_wait_us(100000);
  rt_printf("Task2 busy wait stopped\n");

  icpp_lock(&resourceA, &thread2_str);
  rt_printf("Task2 locking mutexA\n");

  rt_printf("Task2 busy wait start\n");
  busy_wait_us(200000);
  rt_printf("Task2 busy wait stopped\n");

  icpp_unlock(&resourceA, &thread2_str);
  rt_printf("Task2 releasing mutexA\n");

  icpp_unlock(&resourceB, &thread2_str);
  rt_printf("Task2 releasing mutexB\n");
  busy_wait_us(100000);
}
void* taskA_thread_4(void* arg) {
  set_cpu(T_CPU(0));
  unsigned long duration = 30000000000; // 30 second timeout
  unsigned long endTime = rt_timer_read() + duration;

  if(rt_timer_read() > endTime){
  rt_printf("Time expired\n");
  	rt_task_delete(NULL);
  }
  rt_task_sleep(100000000);
  rt_sem_broadcast(&semaphore);
//  rt_sem_delete(&semaphore);
  rt_printf("Semaphore has been synchronized\n\r");
  rt_task_sleep(500000000000);
  rt_sem_delete(&semaphore);
  rt_mutex_delete(&mutex);
  rt_mutex_delete(&mutexA);
  rt_mutex_delete(&mutexB);
}

void* disturbance_thread(void* arg){
  set_cpu(0);
  while(1){
    asm volatile("" ::: "memory");
  }
}

int main(){
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_print_auto_init(1);
	int err;
	int err_mut;
	int err_mutA;
	int err_mutB;
	err = rt_sem_create(&semaphore,"MySempahore", 1, S_FIFO);
	err_mut = rt_mutex_create(&mutex,"MyMutex");
	err_mutA = rt_mutex_create(&mutexA,"MyMutexA");
	err_mutB = rt_mutex_create(&mutexB,"MyMutexB");

	/*
	pthread_t disturbance[10];

  	for (int i = 0; i < 10; i++){
  	    pthread_create(&disturbance[i], NULL, &disturbance_thread,NULL);
  	}
 	*/
	
	//rt_task_create(&periodic_RT_task, "periodic test task", 0, 50, T_CPU(0));
	//rt_task_set_periodic(&periodic_RT_task, TM_NOW, (1 * 1000 * 1000));
	//rt_task_start(&periodic_RT_task, &periodic_task, NULL);
	
	//rt_task_create(&taskA_RT_thread_1, "task1", 0, 70, T_CPU(0));
	//rt_task_create(&taskA_RT_thread_2, "task2", 0, 75, T_CPU(0));
	//rt_task_create(&taskA_RT_thread_3, "task3", 0, 80, T_CPU(0));
	rt_task_create(&taskA_RT_thread_4, "task4", 0, 99, T_CPU(0));
	//rt_task_create(&taskD_RT_thread_1, "task3", 0, 80, T_CPU(0));
	//rt_task_create(&taskD_RT_thread_2, "task4", 0, 90, T_CPU(0));
//	rt_task_create(&taskD_RT_thread_1, "task3", 0, thread1_str.base, T_CPU(0));
//	rt_task_create(&taskD_RT_thread_2, "task3", 0, thread2_str.base, T_CPU(0));
	rt_task_create(&thread1_str.task, "task1", 0, thread1_str.base, T_CPU(0));
	rt_task_create(&thread2_str.task, "task2", 0, thread2_str.base, T_CPU(0));
	
//	rt_task_set_periodic(&taskA_RT_thread_1, TM_NOW, (1 * 1000 * 1000));
//	rt_task_set_periodic(&taskA_RT_thread_2, TM_NOW, (1 * 1000 * 1000));
//	rt_task_set_periodic(&taskA_RT_thread_3, TM_NOW, (1 * 1000 * 1000));
	
	//rt_task_start(&taskA_RT_thread_1, &taskA_thread_1, NULL);
	//rt_task_start(&taskA_RT_thread_2, &taskA_thread_2, NULL);
	//rt_task_start(&taskA_RT_thread_3, &taskA_thread_3, NULL);
	rt_task_start(&taskA_RT_thread_4, &taskA_thread_4, NULL);
	rt_task_start(&thread1_str.task, &taskD_thread_1, NULL);
	rt_task_start(&thread2_str.task, &taskD_thread_2, NULL);
	while(1){};
	/*
  	for (int i = 0; i < 10; i++){
   	 pthread_join(disturbance[i],NULL);
 	}
 	*/
  
	return 0;
}

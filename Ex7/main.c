#include "io.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>


RT_TASK periodic_RT_task;

void* periodic_task(){
	while(1){
		rt_printf("periodic print \n\r");
		rt_task_wait_period(NULL);
	}
}


int main(){
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_print_auto_init(1);


	rt_task_create(&periodic_RT_task, "periodic test task", 0, 50, T_CPU(0));
	rt_task_set_periodic(&periodic_RT_task, TM_NOW, (2.67 * 1000 * 1000 * 1000));
	rt_task_start(&periodic_RT_task, &periodic_task, NULL);
	while(1){};

	return 0;
}
#include "timeFunctions.h"

// helper functions for timestructs
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
};

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
};

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
};

int timespec_cmp(struct timespec lhs, struct timespec rhs){
    if (lhs.tv_sec < rhs.tv_sec)
        return -1;
    if (lhs.tv_sec > rhs.tv_sec)
        return 1;
    return lhs.tv_nsec - rhs.tv_nsec;
}



// Task A - Premade ready wait
void busy_wait(struct timespec t){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);

    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

// Task A - Custom busy wait
void busy_wait_custom(long hundreths) {
    struct tms st_cpu, now;
    clock_t st_time = times(&st_cpu);
    clock_t curr_time = times(&now);
    while (curr_time - st_time < hundreths){
        for(int i = 0; i < 10000; i++){}
        curr_time = times(&now);
    }
}

// Task B - Function used to determine latency
void latencyRes(){
    struct timespec time;
    struct tms time2;

    for (int i = 0; i < 10*1000*1000; i++) {
        // unsigned int i = __rdtsc();
        // clock_gettime( CLOCK_REALTIME, &time);
        clock_t st_time = times(&time2);
    }
}

// Task B - Function used to determine resolution
void resolution(){
    int ns_max = 50;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
    // struct timespec t1,t2;
    struct tms tc1,tc2;

    for(int i = 0; i < 10*1000*1000; i++){

        unsigned int t1 = __rdtsc();
        unsigned int t2 = __rdtsc();
        int ns = (t2 - t1) * 1/(2.66);

        // clock_gettime( CLOCK_REALTIME, &t1);
        // clock_gettime( CLOCK_REALTIME, &t2);
        // int ns = (t2.tv_nsec - t1.tv_nsec);

        // clock_t t1 = times(&tc1);
        // clock_t t2 = times(&tc2);
        // int ns = (t2 - t1) * 1000000;
        // times(&tc1);
        // times(&tc2);
        // int ns = (tc2.tms_stime - tc1.tms_stime) * 1/(2.66);
        // printf(ns);

        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }

    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }
}

// Task C - Measuring the times it takes to context switch to the kernal and back.
void contextSwitch(){
    int ns_max = 1000;
    int histogram[ns_max];
    memset(histogram, 0, sizeof(int)*ns_max);
    struct timespec t1,t2;

    for(int i = 0; i < 10*1000*1000; i++){

        clock_gettime( CLOCK_REALTIME, &t1);
        sched_yield();
        clock_gettime( CLOCK_REALTIME, &t2);
        int ns = (t2.tv_nsec - t1.tv_nsec);

        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }

    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }
}
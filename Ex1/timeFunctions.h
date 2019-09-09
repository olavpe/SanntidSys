#pragma once

// #include <unistd.h>
#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>

void busy_wait(struct timespec t);
void busy_wait_custom();
int timespec_cmp(struct timespec lhs, struct timespec rhs);
void latencyRes();
void resolution();
void contextSwitch();
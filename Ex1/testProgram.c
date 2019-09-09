#include <unistd.h>
#include "timeFunctions.h"

int main() {
    
    // Task A
    struct timespec t = {1};
    
    // Using standard sleep function
    // sleep(1); 

    // Using given busy wait with clock_gettime(..)
    // busy_wait(t);
    
    // Using busy wait with custom times() function
    // busy_wait_custom(100);

    // Task B
    // latencyRes();
    resolution();

    // Task C
    // contextSwitch();

}
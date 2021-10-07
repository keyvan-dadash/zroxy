


#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <errno.h>
#include <time.h>


#include "utils/timer/timers.h"
#include "logging/logs.h"

int32_t zxy_create_timer_with_expiration(int32_t seconds, int32_t nanoseconds)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd == -1) {
        LOG_FATAL(-1, "Cannot create timerfd: %s\n", strerror(errno));
    } 
    
    struct itimerspec ts;

    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = seconds;
    ts.it_value.tv_nsec = nanoseconds;

    if (timerfd_settime(tfd, 0, &ts, NULL) < 0) {
        LOG_FATAL(-1, "Cannot set time on timerfd error: %s\n", strerror(errno));
    }

    return tfd;
}

int32_t zxy_create_timer_with_interval(int32_t seconds, int32_t nanoseconds)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    
    struct itimerspec ts;

    ts.it_interval.tv_sec = seconds;
    ts.it_interval.tv_nsec = nanoseconds;
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;


    if (timerfd_settime(tfd, 0, &ts, NULL) < 0) {
        LOG_FATAL(-1, "Cannot set time on timerfd error: %s", strerror(errno));
    }

    return tfd;
}

void zxy_set_timer_time_with(int32_t timerfd, int32_t seconds, int32_t nanoseconds)
{
    struct itimerspec ts;

    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = seconds;
    ts.it_value.tv_nsec = nanoseconds;

    if (timerfd_settime(timerfd, 0, &ts, NULL) < 0) {
        LOG_FATAL(-1, "Cannot set time on timerfd error: %s", strerror(errno));
    }
}



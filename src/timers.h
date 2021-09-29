

#ifndef TIMERS_H
#define TIMERS_H

#include <sys/types.h>

#define TIMER_RELAXED -3
#define TIMER_IS_UP -2
#define RELAX_TIMER -1



int32_t create_timer_with_expiration(int32_t seconds, int32_t nanoseconds);

int32_t create_timer_with_interval(int32_t seconds, int32_t nanoseconds);

void set_timer_time_with(int32_t timerfd, int32_t seconds, int32_t nanoseconds);


#endif /* TIMERS_H */
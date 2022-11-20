/**
 * @file timers.h
 */

#ifndef TIMERS_H
#define TIMERS_H

#include <sys/types.h>

#define TIMER_RELAXED -3
#define TIMER_IS_UP -2
#define RELAX_TIMER -1

/**
 * Create timer FD with expiration.
 * \param seconds Number of seconds.
 * \param nanoseconds Number of nanoseconds.
 * \return The FD of created timer.
 */
int32_t zxy_create_timer_with_expiration(int32_t seconds, int32_t nanoseconds);

/**
 * Create timer FD with interval.
 * \param seconds Number of seconds.
 * \param nanoseconds Number of nanoseconds.
 * \return The FD of created timer.
 */ 
int32_t zxy_create_timer_with_interval(int32_t seconds, int32_t nanoseconds);

/**
 * Set timer on the timer FD.
 * \param timerfd FD of the timer.
 * \param seconds Number of seconds.
 * \param nanoseconds Number of nanoseconds.
 */
void zxy_set_timer_time_with(int32_t timerfd, int32_t seconds, int32_t nanoseconds);

#endif /* TIMERS_H */


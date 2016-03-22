#ifndef TIMER_H
#define TIMER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


#include <types_jeu.h>

void timer(pthread_t *pt, int temps, int *flag, pthread_cond_t *cond, pthread_mutex_t *mut);
void *timer_thread(void *arg);

#endif

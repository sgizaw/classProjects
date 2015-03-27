/* File: thread.c
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#include "thread.h"
#include "network.h"


void print_q()
{
	printf("test\n");
}

void init_pool() {
	/* initialize thread pool */
	
	/* create the thread pointers 
	 *	q_thread, sch_thread, execs 
	 */
	pool.execs = NULL; 
	struct s_thread *q_thread = &pool.q_thread;	
	struct s_thread *sch_thread = &pool.sch_thread;			
	q_thread->id = 0;
	q_thread->parent = 1;
	q_thread->next = NULL;
	sch_thread->id = 1;
	sch_thread->parent = 1;
	sch_thread->next = NULL;
	int i;
	pthread_cond_init(&exec_fin, NULL);	

	for(i =0; i < no_threads; i++){
		struct s_thread *new_thread, *helper;
		new_thread = malloc(sizeof(struct s_thread));
		new_thread->id = i+2; /* 0, 1 - for queuing and sched. threads */
		new_thread->state = READY;
		new_thread->command = WAIT;
		new_thread->parent = 0;
		new_thread->next = NULL;

		pthread_cond_init(&new_thread->serv_cond, NULL);
		pthread_mutex_init(&new_thread->serv_mutex, NULL);
		pthread_create(&new_thread->pthd, NULL, (void *) &service_request, (void *) new_thread);
		
		if(pool.execs == NULL)
			pool.execs = new_thread;
		else {
			helper = pool.execs; 
			while(helper->next)  
				helper = helper->next;
			helper->next = new_thread;
		}
		
		/* create new thread to execute service_request*/
		
	}
	
//	SCHEDULE = 1;
}

struct s_thread *get_thread(){  /* return a non BUSY thread */
//	sem_wait(&rdy_mutex);
	struct s_thread *ret = pool.execs;
	while(ret) {
		if(ret->state == READY)
			return ret;
		ret = ret->next;
	}
//	sem_post(&rdy_mutex);	
	return NULL;
}


	


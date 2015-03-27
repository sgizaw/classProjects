/* File: main.c
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#include "main.h"
#include "init.h"
#include "network.h"
#include "thread.h"
#include "log.h"

#include <semaphore.h>
#include <pthread.h>

int main(int argc, char *argv[]) 
{
	
	if(chk_set_flags(argc-1, argv) < 0) {
		printf("Unknown option specified. Run './myhttpd -h' for help\n");
		return 0;
	}
	/*
	if(!DEBUG) {
		pid_t pid;
		if((pid=fork())<0) {
			printf("Unable to creat Daemon process");
			return -1;
		}
		else if(pid !=0) exit(0); 
		setsid();
		umask(0);
	}*/
	
	if(setup_listening() < 0) {
		printf("Unable to setup listening port\n");
		return 0;
	}

	init();
	
	/* create and assign queuing thread to loop() to accept connections
	 * sch_thread to check_schedule()
	 */
	pthread_create(&pool.q_thread.pthd, NULL, (void *) &loop, NULL); 
	pthread_create(&pool.sch_thread.pthd, NULL, (void *) &check_schedule, NULL);	
	
	pthread_join(pool.q_thread.pthd, NULL);
	pthread_join(pool.sch_thread.pthd, NULL);	
	return 0;
}
/* ################################### */

/* Listen for incoming http requests, 
 * Handled by the queuing thread - q_thread
 */
void loop()
{
	while(1){
		if((accept_conn()) !=1) /* if problem continue*/
			continue;
		handle_request();
		SCHEDULE = 1;
	}
}

/* Check if enough time has elapsed, and schedule,
 * Handled by scheduling thread - sch_thread
 */
void check_schedule()
{
	sleep(q_time);
	while(!SCHEDULE) ;
	struct request *req;
	struct s_thread *handler;
	char temp[30]; 
	long time(), mytime;
	size_t size;
	while(1) {
		if(def_sch_policy) { /* for FCFS the queue is already ordered */
//			sem_wait(&q_mutex);
			req = ready_q;
//			sem_post(&q_mutex);
			while(req) {
				if(req->state == WAITING && req->file_name != NULL) {
					handler = get_thread(); /* get the first available thread */
					while(handler == NULL) {
						pthread_cond_wait(&thr_ready, &thr_mutex);
						handler = get_thread();
					}
					
//					sem_wait(&q_mutex);
					handler->my_req = req;  /* Assign a request to it */
					handler->command = EXECUTE; /* Inform the thread */
					handler->state = BUSY;
					req->state = SCHEDULED;	/* change the req state and update its exec time */
					mytime = time(NULL);
					size = strftime(temp, 30, "%d/%b/%Y:%T", gmtime(&(mytime)));
					req->exe_time = malloc(size);
					strncpy(req->exe_time, temp, size);
//					sem_post(&q_mutex);
					printf("broadcasting\n");
					if(DEBUG) { /* if Debug, hold after broadcast until this is serviced 
									 * ...another condition variable signaled by the exec thread... */
						if(pthread_cond_signal(&handler->serv_cond) == 0) { /* broadcast service for the exec threads */
							if(pthread_cond_wait(&exec_fin, &exec_mutex) != 0) {
								printf("Scheduling Q: Error Calling pthread_cond_wait. for cond. exec_fin\n");			
							}
						}					
					}
					else {
						pthread_cond_signal(&handler->serv_cond); /* broadcast service condition */
					}
				}	
				req = req->next;
			}

		} /* FCFS end */
		
		else { /* SJF */
			while(1){
//				sem_wait(&q_mutex);
				req = sjf_helper();
				if(req != NULL && req->state == WAITING && req->file_name != NULL) {
					handler = get_thread(); /* get the first available thread */
					while(handler == NULL) { /* if no one is available, wait */
						pthread_cond_wait(&thr_ready, &thr_mutex);
						handler = get_thread();
					}
					
//					sem_wait(&q_mutex);
					handler->my_req = req;  /* Assign a request to it */
					handler->command = EXECUTE; /* Inform the thread */
					handler->state = BUSY;
					req->state = SCHEDULED;	/* change the req state and update its exec time */
					mytime = time(NULL);
					size = strftime(temp, 30, "%d/%b/%Y:%T", gmtime(&(mytime)));
					req->exe_time = malloc(size);
					strncpy(req->exe_time, temp, size);
//					sem_post(&q_mutex);
					
					if(DEBUG) { /* if Debug, hold after broadcast until this is serviced 
									 * ...another condition variable signaled by the exec thread... */
						if(pthread_cond_signal(&handler->serv_cond) == 0) { /* broadcast service for the exec threads */
							if(pthread_cond_wait(&exec_fin, &exec_mutex) != 0) {
								printf("Scheduling Q: Error Calling pthread_cond_wait. for cond. exec_fin\n");			
							}
						}					
					}
					else {
						pthread_cond_signal(&handler->serv_cond); /* broadcast service condition */
					}
				
				}

			}
		}
	}
}

struct request *sjf_helper()
{	/* find the request with the least size
	 *	remove it from the queue and return its pointer
	 */
	struct request *result;
	pthread_mutex_lock(&q_mutex);
	if(ready_q == NULL) {
		pthread_mutex_unlock(&q_mutex);
		return NULL;
	}
		
	result = ready_q;
	struct request *r_helper = ready_q->next;
	while(r_helper) {
		if(r_helper->cont_size < result->cont_size)
			result = r_helper;
		r_helper = r_helper->next;		
	}
	pthread_mutex_unlock(&q_mutex);
	return result;
}


void update_q(int req_id) 
{
//	sem_wait(&q_mutex);
	pthread_mutex_lock(&q_mutex);
	struct request *r_helper = ready_q;
	
	if(ready_q->id == req_id && ready_q->state != WAITING) {
//		close(ready_q->cl_fd);
		ready_q = ready_q->next;
//		free(r_helper);
//		sem_post(&q_mutex);
		pthread_mutex_unlock(&q_mutex);
		return;
	}
	r_helper = ready_q->next;
	struct request *prev = ready_q;
	while(r_helper) {
		if(r_helper->id == req_id && r_helper->state != WAITING) {
			prev->next = r_helper->next;
//			free(r_helper);
//			sem_post(&q_mutex);
		//	close(r_helper->cl_fd);
			pthread_mutex_unlock(&q_mutex);
			return;
		}
		r_helper = r_helper->next;
		prev = prev->next;
	}
//	sem_post(&q_mutex);
	pthread_mutex_unlock(&q_mutex);
}


void print_ready_q()
{
	struct request *r_helper = ready_q;
	int i = 0;
	printf("Ready Queue:\n");
	while(r_helper) {
		printf("req: %d, file_name: %s, state: %s\n", i++, r_helper->file_name, (r_helper->state == SCHEDULED)? "Scheduled":"Waiting");
		r_helper = r_helper->next;
	}


}





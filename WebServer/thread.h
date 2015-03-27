/* File: thread.h
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#ifndef _THREAD_H_
#define _THREAD_H_

#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"


typedef enum {
	WAIT,
	EXECUTE
}t_command;

typedef enum {
	READY,
	BUSY
}t_state;

/*
typedef enum {
	PASS,
	NO_PASS
}wait_con;
*/

//pthread_cond_t serv_cond;
//pthread_mutex_t serv_mutex;

pthread_cond_t exec_fin;
pthread_mutex_t exec_mutex;

pthread_cond_t thr_ready;
pthread_mutex_t thr_mutex; 
sem_t rdy_mutex;
//sem_t _mutex;

pthread_mutex_t q_mutex;
pthread_mutex_t otpt_mutex;

struct s_thread {
	int id;
	pthread_t pthd; //id;
	pthread_cond_t serv_cond;
	pthread_mutex_t serv_mutex;
	int parent; /* for the first thread in the list - Scheduling thread */ 
 	t_state state;
	t_command command;
	struct request *my_req;
	struct s_thread *next;
};

struct thread_pool {
	struct s_thread q_thread; /* queuing thread */
	struct s_thread sch_thread; /* scheduler */ 

	struct s_thread *execs; /* execution threads */
};

struct thread_pool pool;


void print_q();
void init_pool();
struct s_thread *get_thread();




#endif

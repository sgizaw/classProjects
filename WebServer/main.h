/* File: main.h
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/



#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <semaphore.h>



extern int DEBUG;//  = 0;
extern int LOG;// = 0;
extern int port;// = 8080;
extern int q_time;// = 60.0;
extern int no_threads;// = 4;
extern int def_sch_policy;// = 1; /* 1 - FCFS, 0 - SJF */
extern int SCHEDULE;


//int help = 0;
extern char log_file[30];// = "";
extern FILE *log_fd; /* either stdout or file fd */
extern char req_file[100];
extern char root_dir[30];// = ".";
extern int listen_fd, client_fd;
extern char client_req[256];
extern struct sockaddr_in client_addr;
extern time_t init_time;


/* Ready queue, used by multiple threads
 * Needs synchronization. 
 */

//struct res_header {
	
//};
extern struct res_header *def_header;

struct response {
	char *date;
	char server_id[30];
	char *last_modified;
	char cont_type[30];
	int cont_size;
	char *data;
};

typedef enum {
	SCHEDULED,
	WAITING
}req_state;

typedef enum {
	GET,
	HEAD,
}req_type;


struct request {
	int id;
	char cl_addr[14];
	int cl_fd;
//	FILE *cl_fp;
	char *q_time;
	char *exe_time;
	char first_line[256];
	int status_code;
	char file_name[100];
	int cont_size;
	req_state state;
	req_type type;
	struct response res;
	struct request *next;	
};

extern struct request *ready_q;// = NULL;
//extern sem_t q_mutex;
//extern pthread_mutex_t q_mutex;


extern void loop();
extern void check_schedule();
extern void print_ready_q();
extern void update_q(int id);
extern struct request *sjf_helper();

#endif

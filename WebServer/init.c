/* File: init.c
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#include "init.h"
#include "main.h"
#include "thread.h"
#include "log.h"

int DEBUG  = 0;
int LOG = 0;
int port = 8080;
int q_time = 60.0;
int no_threads = 4;
int def_sch_policy = 1; /* 1 - FCFS, 0 - SJF */
int SCHEDULE = 0;

char log_file[30] = "";
FILE *log_fd; /* either stdout or file fd */
char req_file[100] = "";

char root_dir[30] = ".";
int listen_fd, client_fd;
char client_req[256];
struct sockaddr_in client_addr;
time_t init_time;
struct request *ready_q = NULL;
//sem_t q_mutex;

int chk_set_flags(int no_flags, char *flags[]) 
{
	if(no_flags == 0) return 0; // default settings 
	int i = 1;

	while(flags[i] != NULL) {
		char flag[2];
		memcpy(flag, flags[i], sizeof(flag));

		if(flag[0] != '-') return -1;
		switch(flag[1]) {
			case 'd':
				DEBUG = 1;
//				printf("%s\n", flag);						
				break;
			
			case 'h':
				printf("Usage:\n ./myhttpd [options]\noptions:\
				\n-d	Debugging mode\
				\n-h	print help\
				\n-l <file>	Log requests to the file\
				\n-p <port>	Specify listening port\
				\n-r <dir>	Specify root directory to serve files\
				\n-t <time>	set Queuing time to <time>. Default 60 secs\
				\n-n <threadnum>	set no. of threads in the pool to handle requests\
				\n-s <sched>	set scheduling policy (FCFS|SJF). Default FCFS\n");
				exit(0);
				break;
			
			case 'l':
				if(flags[++i] == NULL) return -1;
				LOG = 1;
				memcpy(log_file, flags[i], sizeof(flags[i]));
//				printf("%s\n", flag);
//				printf("log file: %s\n", log_file);						
				break;
				
			case 'p':
				if(flags[++i] == NULL) return -1;
				port = atoi(flags[i]);
//				printf("port: %d\n", port);						
				break;
				
			case 'r':
				if(flags[++i] == NULL) return -1;
				memcpy(root_dir, flags[i], sizeof(flags[i]));				
//				printf("root dir: %s\n", root_dir);						
				break;			
			
			case 't':
				if(flags[++i] == NULL) return -1;
				q_time = atoi(flags[i]);
//				printf("port: %f\n", q_time);					
				break;
				
			case 'n':
				if(flags[++i] == NULL) return -1;
				no_threads = atoi(flags[i]);
//				printf("no of threads : %d\n", no_threads);						
				break;
				
			case 's':
				if(flags[++i] == NULL) return -1;
				def_sch_policy = (strcmp(flags[i], "SJF")==0)? 0:1;
//				printf("%d\n", def_sch_policy);						
				break;	
				
			default:
				return -1;	
		}	
		i++;
	}

	return 1;
}


void init()
{
	{ /* log handle */
		time(&init_time);
//		sem_init(&q_mutex, 0, 1);
		pthread_mutex_init(&q_mutex, NULL);
		pthread_mutex_init(&otpt_mutex, NULL);
		pthread_mutex_init(&thr_mutex, NULL);
//		sem_init(&otpt_mutex, 0, 1);
		sem_init(&rdy_mutex, 0, 1);
		pthread_cond_init(&thr_ready, NULL);
		if(DEBUG) { /* only one conn at a time, log to stdout */
			log_fd = stdout;
		}
	}	
	init_pool();
	
}

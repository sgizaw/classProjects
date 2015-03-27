/* File: log.c
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#include "log.h"
#include "thread.h"
#include "init.h"


int log_req(struct request *req)
{
	log_fd = stdout;
	if(1) {
		pthread_mutex_lock(&otpt_mutex);
		fprintf(log_fd, "%s - [%s] [%s] \"%s\" %d %d\n", req->cl_addr, req->q_time, req->exe_time, req->first_line, req->status_code, req->cont_size);	
		pthread_mutex_unlock(&otpt_mutex);
	}
	
	else if(LOG) {
		pthread_mutex_lock(&otpt_mutex);
		if((log_fd = fopen(log_file, "a")) < 0) {
			fprintf(stderr, "Error opening log file %s\n", log_file);
			exit(0);
		}
		fprintf(log_fd, "%s - [%s] [%s] \"%s\" %d %d\n", req->cl_addr, req->q_time, req->exe_time, req->first_line, req->status_code, req->cont_size);
		fclose(log_fd);
		pthread_mutex_unlock(&otpt_mutex);
	}	
	
	return 0;
}

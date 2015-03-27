/* File: log.h
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
struct log_message {  /*struct to hold the log message*/
	char cl_addr[32];
	struct tm *queue_time;
	struct tm *exe_Thrtime;
	char first_line[256];
	int sta_code;
	int res_size;
};

int log_req(struct request *req);

#endif

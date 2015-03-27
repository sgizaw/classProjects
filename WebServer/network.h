/* File: network.h
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>

#include "thread.h"


int setup_listening();
int accept_conn();
int handle_request();
int prep_response(struct request *req);
void service_request(struct s_thread *self);

#endif 

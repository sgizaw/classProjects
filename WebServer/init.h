/* File: init.h
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#ifndef _INIT_H_
#define _INIT_H_


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>

int chk_set_flags(int no_flags, char *flags[]);
void init();

#endif

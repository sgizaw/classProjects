/* File: filehandle.h
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#ifndef _FILEHANDLE_H_
#define _FILEHANDLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

int check_file(char name[]);

int file_size(char name[]);

int get_contents(char name[], char *res_holder, int size);


#endif

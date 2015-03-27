/* File: filehandle.c
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/

#include "filehandle.h"
#include "main.h"
#include "init.h"

int check_file(char name[]){
	printf("requested file: %s\n", name);
	return 0;
}

int file_size(char name[]) {
	int size = 0;
	char root[100];
	char *req;
	char *home;
	if(name[1] != '~') {
		memcpy(root, root_dir, sizeof root);
		req = strcat(root, name);
	}
	else { /* home directory */
		home = getenv("HOME");
		int i = sizeof(home) ;
		while(i>0 && home[i] != '/')
			i--;
		strncpy(root, home, i+1);
		strtok(name, "~");
	    strcat(root,strtok(NULL, "/"));		
		strcat(root,"/myhttpd/");
		req = strcat(root, strtok(NULL, ""));

	}
	
	memcpy(req_file, req, sizeof req_file);
	FILE * fp = fopen(req_file, "r");
	
//	printf("from file Handle, requested file: %s  \n  ", req_file);
	if(fp == NULL) {
//		printf("unable to open dir\n");
		DIR *dir;		
		dir =	opendir(req_file);
		if(dir == NULL){
//			printf("unable to open dir\n");
			return -1;				
		}
		else {
			struct dirent *dp;
			while(dir){
//				printf("reading file list\n");		
				if ((dp = readdir(dir)) != NULL){

					if(strcmp(dp->d_name, "index.html") == 0){
						closedir(dir);
						if(req[strlen(req)-1] != '/')
							strcat(req, "/");
						strcat(req, "index.html");
						memcpy(req_file, req, sizeof req_file);
						if((fp = fopen(req, "r")) !=NULL){
							fseek(fp, 0L, SEEK_END);
							size = ftell(fp);
							fclose(fp);
							return size;
						}
					}
							
				}
				else
					closedir(dir);
			}
			return -1;
		}
	}
	else {
		fseek(fp, 0L, SEEK_END);
		size = ftell(fp);
		fclose(fp);
	}
	//printf("requested file: %s\n", name);
	return size;
}

int get_contents(char name[], char *res_holder, int size) {
	FILE *f = fopen(name, "r");
	if(f==NULL)
		return -1;	
	if(fread(res_holder, size, 1, f) < 0)
		return -1;
	fclose(f);
	res_holder[size] = 0;
	return 0;
} 




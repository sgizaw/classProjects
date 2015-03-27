/* File: network.c
* Author: Sirak Gizaw
* Project: Webserver 
* Operating Systems Class - Fall 2013
*/


#include "network.h"
#include "main.h"
#include "log.h"
#include "filehandle.h"


int handle_request()
{  /* only add request to the ready queue, don't service
	 * Only called by queuing thread, q_thread
	 */

	int len;
	char *req_type;
	int f_size;
	int id = 1;
	char temp[100], *temp1; 
	long time(), mytime;
	size_t size;
	struct request *new_req, *r_helper;
	len = read(client_fd, client_req, sizeof(client_req)); 
	if(len < 0) 
		return -1;
			
	{	/* set up new request and add to the ready queue */
		new_req = malloc(sizeof(struct request));
		mytime = time(NULL);
		size = strftime(temp, 30, "%d/%b/%Y:%T", gmtime(&(mytime)));
		new_req->q_time = malloc(size);
		strncpy(new_req->q_time, temp, size);
		new_req->exe_time = NULL;
		strcpy(temp, client_req);
//		memcpy(temp1, client_req, sizeof(client_req));
		strcpy(new_req->first_line, strtok(temp, "\n"));
		strcpy(new_req->cl_addr, inet_ntoa(client_addr.sin_addr));
		printf("client_rew: %s\n", client_req);
		printf("first line: %s\n", new_req->first_line);
		new_req->state = WAITING;	
		new_req->next = NULL;
		new_req->cl_fd = client_fd;
	}	/* end of new request handle */

	{	/* check request type and file */
		req_type = strtok(client_req, " ");
		if(memcmp(req_type, "GET", 3) == 0) {
			new_req->type = GET;
			f_size = file_size(strtok(NULL, " "));
			if(f_size < 0) { 
				new_req->cont_size = -1;				
			}
			else { 
				memcpy(new_req->file_name, req_file, sizeof new_req->file_name);
				new_req->cont_size = f_size;
			}
		}
		else if (memcmp(req_type, "HEAD", 4) == 0) { 
			new_req->type = HEAD;
			f_size = file_size(strtok(NULL, " "));
			if(f_size < 0) { 
				new_req->cont_size = -1;				
			}
			else { 
				memcpy(new_req->file_name, req_file, sizeof new_req->file_name);
				new_req->cont_size = f_size;
			}
		} 	
	}	/* end of type and file handle */


	{ /* add to ready queue */
//		pthread_mutex_lock(&q_mutex);
		if(ready_q == NULL){	
			new_req->id = id;
			ready_q = new_req;
		}
		else {
			r_helper = ready_q;
			id++;
			while(r_helper->next) { /* go to the end */
				r_helper = r_helper->next;
				id++;
			}
			new_req->id = id;
			r_helper->next = new_req;	/* add to queue */
		}
//		pthread_mutex_unlock(&q_mutex);
	} /* add end */

	return len;
}

void service_request(struct s_thread *self)
{ /* Service Requests, code shared by exec threads */
	int errn;
	char *to_send = malloc(sizeof(struct response) + 100);
	struct request *this_req;
	struct response *res;
	int sent;

	while(1) {
		
		/* wait for a signal from scheduling thread */
		if((errn = pthread_cond_wait(&self->serv_cond, &self->serv_mutex)) != 0) {
//			printf("%d : Error Calling pthread_cond_wait. Errno: %d\n", self->id, errn);			
			continue;
		}
				
//		sleep(1);
		switch(self->command) {
			case WAIT:
				break;
			case EXECUTE: /* send requested file */
				this_req = self->my_req;
				res = &self->my_req->res;
				if(prep_response(this_req) < 0) break; 
				sprintf(to_send, "HTTP/1.0 %s\nDate		:%s\nServer		:%s\nLast-Modified	:%s\nContent-Type	:%s\nContent-Length	:%d\n\n\r%s", (this_req->status_code==200)? "200 OK":"404 Not Found", 
				res->date, "Apache"/*res->server_id*/ , res->last_modified, res->cont_type, res->cont_size, (res->cont_size>0)? res->data:"");
				to_send[strlen(to_send)] = '\n';
				sent = send(this_req->cl_fd, to_send, strlen(to_send), 0);
				
				close(this_req->cl_fd);
				if(DEBUG)
					pthread_cond_signal(&exec_fin);
				log_req(this_req);
				update_q(this_req->id);
//				sem_wait(&rdy_mutex);
				self->state = READY;
//				sem_post(&rdy_mutex);
				pthread_cond_signal(&thr_ready);
				printf("%d: finished processing \n", self->id);
				break;
			default:	
				continue;
		} 
	}
}

int prep_response(struct request *req)
{
	if(req->cont_size < 0) {
		req->status_code = 404;
		return -1;
	}
	char file[100];
	strcpy(file, req->file_name);
	long time(), mytime;
	struct response *new_res = &req->res; 
	new_res->cont_size = req->cont_size;
	size_t size;
	char temp[30];
	struct stat attr;
	stat(file, &attr);
	size = strftime(temp, 30, "%d/%b/%Y:%T", gmtime(&(attr.st_mtime)));
	new_res->last_modified = malloc(size);
	strcpy(new_res->last_modified, temp);
	mytime = time(NULL);
	size = strftime(temp, 30, "%d/%b/%Y:%T", gmtime(&(mytime)));
	new_res->date = malloc(size);
	strncpy(new_res->date, temp, size);
	
	strtok(file, ".");
	strcpy(temp, strtok(NULL, ""));
	if(strncmp(temp, "html", 4) == 0)
		strcpy(new_res->cont_type, "text/html");
	else if (strncmp(temp, "gif", 3) == 0)
		strcpy(new_res->cont_type, "image/gif");									
	else 
		strcpy(new_res->cont_type, temp);	

	if(req->type == GET) {
		new_res->data = malloc(req->cont_size + 1);
		if(get_contents(req->file_name, new_res->data, req->cont_size) < 0)
			req->status_code = 404;
		else
			req->status_code = 200;
	}

	return 0;
}


int accept_conn()
{
	socklen_t c_len = sizeof(client_addr);
	client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &c_len);
	
	return (client_fd > 0)? 1:0;
}

int setup_listening()
{
	struct sockaddr_in server_addr;
	//int slen, sock_id, sock_fd;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = 0;
	
	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "error creating server listening socket\n");
		return -1;
	}
	if((bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) < 0) {
		fprintf(stderr, "error binding to address");
		return -1;
	}

	if(listen(listen_fd, 10) != 0 ) {
		fprintf(stderr, "error listening on port");
		return -1;
	}

	return 0;
}

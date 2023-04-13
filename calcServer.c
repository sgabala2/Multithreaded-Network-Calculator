/*
* Implementation of calculator server
* CSF Assignment 6
* Joy Neuberger and Shaina Gabala
* jneuber6@jhu.edu, sgabala2@jhu.edu
*/
#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"
#include <string.h>
#include <pthread.h>

//represents a client connection
struct ConnInfo {
  int clientfd;
  struct Calc *myCalc;
};

/* buffer size for reading lines of input from user */
#define LINEBUF_SIZE 1024
int chat_with_client(struct Calc *calc, int infd, int outfd);
void *worker(void *);

int shutdown_flag = 0;

int main(int argc, char **argv) {
	if (argc != 2) {
    printf("Invalid command line arguments.");
    exit(1);
  }

  //create single calculator for all client-server interactions
  struct Calc *calc = calc_create(); 
  //open server socket
  int listen_fd = Open_listenfd(argv[1]);

  while(!shutdown_flag) {

    int client_fd = Accept(listen_fd, NULL, NULL); //listen for client connections
		if(client_fd < 0) {
			printf("error");
		} else {
      struct ConnInfo *myConn = malloc(sizeof(struct ConnInfo));
      myConn->myCalc = calc;
      myConn->clientfd = client_fd;
			pthread_t newthread;
			pthread_create(&newthread, NULL, worker, myConn);
		}
  }
  //close socket
	close(listen_fd);
  calc_destroy(calc);
	return 0;
}



//- detaches the thread and chats with client
void *worker(void *args) {
	
  struct ConnInfo *conn = args;
  pthread_detach(pthread_self());
	chat_with_client(conn->myCalc, conn->clientfd, conn->clientfd);
	 
	close(conn->clientfd);
  free(conn);
  return NULL;
}

/* Communicate with client
 * Parameters: calc - pointer to calculator instance used to calculate client's input
 *             infd, outfd - bidirectional file descriptor for client's socket
 */
int chat_with_client(struct Calc *calc, int infd, int outfd) {
	rio_t in;
	char linebuf[LINEBUF_SIZE];

	/* wrap standard input (which is file descriptor 0) */
	rio_readinitb(&in, infd);

	//read, evaluate, and print lines of input
	while ("") {
		ssize_t n = rio_readlineb(&in, linebuf, LINEBUF_SIZE);
		//if (n <= 0) {
			/* error or end of input */
		//	done = 1;
		//} else 
    if (n <= 0 || strcmp(linebuf, "quit\n") == 0 || strcmp(linebuf, "quit\r\n") == 0) {
			/* quit command */
			return 1;
		} else if (strcmp(linebuf, "shutdown\n") == 0 || strcmp(linebuf, "shutdown\r\n") == 0) {
      shutdown_flag = 1;
			return 2;
		} else {
			/* process input line */
			int result;
			if (calc_eval(calc, linebuf, &result) == 0) {
				/* expression couldn't be evaluated */
				rio_writen(outfd, "Error\n", 6);
			} else {
				/* output result */
				int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
				if (len < LINEBUF_SIZE) {
					rio_writen(outfd, linebuf, len);
				}
			}
		}
	}
  return 1;
}

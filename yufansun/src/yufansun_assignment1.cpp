/**
 * @yufansun_assignment1
 * @author  yufansun <yufansun@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>

#include "../include/global.h"
#include "../include/logger.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

using namespace std;

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define MSG_SIZE 256
#define BUFFER_SIZE 256

int connect_to_udp(char *server_ip, int server_port);
int connect_to_host(char *server_ip, int server_port);

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
        
	if(argc != 3) {
		printf("Usage:%s [port]\n", argv[0]);
		exit(-1);
	}
	if (strcmp(argv[1],"s") == 0){
	  int port, server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len;
	  struct sockaddr_in server_addr, client_addr, self_addr;
	  fd_set master_list, watch_list;

	  /* Socket */
	  server_socket = socket(AF_INET, SOCK_STREAM, 0);
	  if(server_socket < 0)
	    perror("Cannot create socket");

	  /* Fill up sockaddr_in struct */
	  port = atoi(argv[2]);
	  bzero(&server_addr, sizeof(server_addr));

	  server_addr.sin_family = AF_INET;
	  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	  server_addr.sin_port = htons(port);

	  /* Bind */
	  if(::bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
    	    perror("Bind failed");

	  /* Listen */
	  if(listen(server_socket, BACKLOG) < 0)
	    perror("Unable to listen on port");

	  /* ---------------------------------------------------------------------------- */

	  /* Zero select FD sets */
	  FD_ZERO(&master_list);
	  FD_ZERO(&watch_list);
    
	  /* Register the listening socket */
	  FD_SET(server_socket, &master_list);
	  /* Register STDIN */
	  FD_SET(STDIN, &master_list);

	  head_socket = server_socket;

	  while(TRUE){
            memcpy(&watch_list, &master_list, sizeof(master_list));

            //printf("\n[PA1-Server@CSE489/589]$ ");
	    //fflush(stdout);

            /* select() system call. This will BLOCK */
            selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
            if(selret < 0)
	      perror("select failed.");

            /* Check if we have sockets/STDIN to process */
            if(selret > 0){
	      /* Loop through socket descriptors to check which ones are ready */
	      for(sock_index=0; sock_index<=head_socket; sock_index+=1){

		if(FD_ISSET(sock_index, &watch_list)){

		  /* Check if new command on STDIN */
		  if (sock_index == STDIN){
		    char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

		    memset(cmd, '\0', CMD_SIZE);
		    if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
		      exit(-1);

		    printf("\nI got: %s\n", cmd);
						
		    //Process PA1 commands here ...
		    if (strcmp(cmd, "AUTHOR\n") == 0){ 
		      cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "yufansun");
		    }
		    if (strcmp(cmd, "IP\n") == 0){
		      char *ip = "8.8.8.8";
		      int UDP_socket = connect_to_udp(ip, 53);
		      socklen_t size = sizeof(self_addr);
		      getsockname(UDP_socket, (struct sockaddr*)&self_addr, &size);
		      uint32_t ip_address = self_addr.sin_addr.s_addr;
		      char *ip_pointer, ip_addr[16];
		      unsigned char number;
		      int r_number, num_chars;
		      char *command_str = "IP";
		      uint32_t *ip_p = &ip_address;
		      ip_pointer = ip_addr; 
		      for (int i = 0; i < 4; i++){
			number = *(unsigned char *)ip_p;
			r_number = (int)number;
			num_chars = sprintf(ip_pointer, "%d", r_number);
			ip_p += sizeof(char);
			ip_pointer += num_chars * sizeof(char);
			if (i == 3){
			  break;
			}
			*ip_pointer = '.';
			ip_pointer++;
		      }
		      if (ip_addr == 0){
			cse4589_print_and_log("[%s:ERROR]\n", command_str);
			cse4589_print_and_log("[%s:END]\n", command_str);
		      }
		      else{
			cse4589_print_and_log("[%s:SUCCESS]\n", command_str);

			cse4589_print_and_log("IP:%s\n", ip_addr);

			cse4589_print_and_log("[%s:END]\n", command_str);
		      }
		    }
		    if (strcmp(cmd, "PORT\n") == 0){
		      cse4589_print_and_log("PORT:%d\n", port);
		    }
		    free(cmd);
		  }
		  /* Check if new client is requesting connection */
		  else if(sock_index == server_socket){
		    socklen_t caddr_len = sizeof(client_addr);
		    fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
		    if(fdaccept < 0)
		      perror("Accept failed.");

		    printf("\nRemote Host connected!\n");
			    

		    /* Add to watched socket list */
		    FD_SET(fdaccept, &master_list);
		    if(fdaccept > head_socket) head_socket = fdaccept;
		  }
		  /* Read from existing clients */
		  else{
		    /* Initialize buffer to receieve response */
		    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
		    memset(buffer, '\0', BUFFER_SIZE);

		    if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
		      close(sock_index);
		      printf("Remote Host terminated connection!\n");

		      /* Remove from watched list */
		      FD_CLR(sock_index, &master_list);
		    }
		    else {
		      //Process incoming data from existing clients here ...

		      printf("\nClient sent me: %s\n", buffer);
		      printf("ECHOing it back to the remote host ... ");
		      if(send(fdaccept, buffer, strlen(buffer), 0) == strlen(buffer))
			printf("Done!\n");
		      fflush(stdout);
		    }

		    free(buffer);
		  }
		}
	      }
            }
	  }

	}
	if (strcmp(argv[1],"c") == 0){ // client
	  int server;
	  server = connect_to_host(argv[1], atoi(argv[2]));

	  while(TRUE){
	    printf("\n[PA1-Client@CSE489/589]$ ");
	    fflush(stdout);

	    char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
    	    memset(msg, '\0', MSG_SIZE);
	    if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
	      exit(-1);

	    printf("I got: %s(size:%d chars)", msg, strlen(msg));

	    printf("\nSENDing it to the remote server ... ");
	    if(send(server, msg, strlen(msg), 0) == strlen(msg))
	      printf("Done!\n");
	    fflush(stdout);

	    /* Initialize buffer to receieve response */
	    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
	    memset(buffer, '\0', BUFFER_SIZE);

	    if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){
	      printf("Server responded: %s", buffer);
	      fflush(stdout);
	    }
	  }
	  //client end here
	}

    return 0;
	
	return 0;
}
int connect_to_udp(char *server_ip, int server_port)
{
    int fdsocket, len;
    struct sockaddr_in remote_server_addr;

    fdsocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(fdsocket < 0)
       perror("Failed to create socket");

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);

    if(connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr)) < 0)
        perror("Connect failed");

    return fdsocket;
}

int connect_to_host(char *server_ip, int server_port)
{
    int fdsocket, len;
    struct sockaddr_in remote_server_addr;

    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if(fdsocket < 0)
       perror("Failed to create socket");

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);

    if(connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr)) < 0)
        perror("Connect failed");

    return fdsocket;
}

/*citation */
/*https://ubmnc.wordpress.com/2010/09/22/on-getting-the-ip-name-of-a-machine-for-chatty/*/

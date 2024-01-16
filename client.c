/*
 * ToY.CS5600.Practicum II
 *
 * Yan To / CS5600 / Northeastern University
 * Fall 2023 / Dec 4, 2023
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client.h"
#define BUFFER_SIZE 8192

int main(int argc, char *argv[]) {
  // set arg1 as command variable
  char* command = argv[1];
  
  int socket_desc;
  struct sockaddr_in server_addr;

  // Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) {
      perror("Error creating socket");
      return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(50320);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect to the server
  if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Error connecting to server");
    return -1;
  }

  // Send the command
  send_command(socket_desc, command, argv[2], (argc > 3) ? argv[3] : argv[2]);

  // Wait for a response from the server before executing tasks
  char response[BUFFER_SIZE];
  memset(response, '\0', sizeof(response));
  recv(socket_desc, response, sizeof(response), 0);
  printf("Server response: %s\n",response);

  // Check the response and proceed accordingly
  if (strcmp(response, "READY") == 0) {
    // If the command is WRITE, send the file
    if (strcmp(command, "WRITE") == 0) {
      send_file(socket_desc, argv[2], (argc > 3) ? argv[3] : argv[2]);
    }

    // If the command is GET, retrieve the file
    if (strcmp(command, "GET") == 0) {
      send(socket_desc, "READY", strlen("READY"), 0);
      save_file(socket_desc, argv[2], (argc > 3) ? argv[3] : argv[2]);
    }

    // If the command is RM, get response from server if RM is success or not
    if (strcmp(command, "RM") == 0){
      send(socket_desc, "READY", strlen("READY"), 0);
      char removeResponse[BUFFER_SIZE];
      memset(removeResponse, '\0', sizeof(removeResponse));
      recv(socket_desc, removeResponse, sizeof(removeResponse), 0);
      printf("Remove Response: %s\n", removeResponse);
    }
  }
  
  else {
    fprintf(stderr, "Server not ready or unexpected command arguments: %s\n", response);
  }

  // Close the connection
  close(socket_desc);
  return 0;
}
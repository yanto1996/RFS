#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client.h"
#define BUFFER_SIZE 8192

// This function sends the command line arguments to the server
// Parameters include the command WRITE, GET, RM 
// arg1 = local path for WRITE, remote path for GET, file path for RM
// arg2 = remote path for WRITE, local path for GET
void send_command(int socket_desc, const char *command, const char *arg1, const char *arg2) {
  // Store the command line into a message buffer. Buffer size is set to max using limit.h library
  char message[BUFFER_SIZE];

  // use strcmp function to check if command is WRITE, GET, or RM
  if (strcmp(command, "RM") == 0) {
      // For RM command, only send command and arg1
      sprintf(message, "%s|%s", command, arg1);
  } 
  else {
      // For other commands, send command, arg1, and arg2
      sprintf(message, "%s|%s|%s", command, arg1, arg2);
  }

  // print the command line arguments being sent to the server
  printf("Message being sent: %s\n", message);
  // sends it off to the server to receive
  send(socket_desc, message, strlen(message), 0);
}

// This function sends a file in the local path to the server
// If the file is not present in the local path print error and return
// else it reads the file and sends the content to the server
void send_file(int socket_desc, const char *local_file_path, const char *remote_file_path) {
  // open the file in binary reading mode
  FILE *file = fopen(local_file_path, "rb");
  // if the file is NULL print error and return
  if (file == NULL) {
      perror("Error opening file");
      return;
  }

  // Create a loop that terminates once all contents of the file is read
  while (1) {
    // create character buffer and initialize all elements to 0
    unsigned char buffer[BUFFER_SIZE] = {0};
    // create an integer variable that stores the # of bytes read from the file
    int bytesRead = fread(buffer, 1, sizeof(buffer), file);

    // check if there are bytes to send
    if (bytesRead > 0) {
      // send the content to the server
      // if the number of bytes != integer bytesRead indicates there was an error
      if (send(socket_desc, buffer, bytesRead, 0) != bytesRead) {
        // print error and break
        perror("Error sending file");
        break;
      }

      // Print the data being sent
      printf("Sent %d bytes from local file: %.*s\n", bytesRead, bytesRead, buffer);
    }

    // check if we have reached the end of the file. Returns success message and breaks
    if (bytesRead < BUFFER_SIZE) {
      if (feof(file)) {
        printf("File sent successfully.\n");
      }
      break;
    }
  }

  printf("\n");
  fclose(file);
}

// This function attempts to save a file being sent from the remote path to the client's local path
// If the content being sent from the server side does not exist (i.e no content), it will print error message and return
// Else it will save the contents from the server's remote path to clients local path
void save_file(int socket_desc, const char *remote_file_path, char *local_file_path) {

  // create a character buffer and intialize all elements to 0
  unsigned char check_buffer[BUFFER_SIZE] = {0};
  // Attempt to receive the data from the server but uses MSG_PEEK flag to not remove it from the socket. This checks if there is data to parse into the local path
  int checkBytesRead = recv(socket_desc, check_buffer, sizeof(check_buffer), MSG_PEEK);

  // If no bytes were received, do not create the file and return
  if (checkBytesRead == 0) {
    printf("No bytes received. File not saved.\n");
    return;
  }

  // open/create file in local path in binary write mode
  FILE *file = fopen(local_file_path, "wb");
  // if file is NULL print error message and return
  if (file == NULL) {
    perror("Error creating file");
    return;
  }

  // create a loop that breaks once all the bytes have been read
  while (1) {
    // create a character buffer an set all elements to 0
    unsigned char buffer[BUFFER_SIZE] = {0};
    // calculate the # of bytes the server is sending over
    int bytesRead = recv(socket_desc, buffer, sizeof(buffer), 0);

    // if there are still bytes left keep parsing into the file
    if (bytesRead > 0) {
      // Print the data being saved
      printf("Read %d bytes from remote file: %.*s\n", bytesRead, bytesRead, buffer);
      fwrite(buffer, 1, bytesRead, file);
    }

    // once there are no more bytes print success message and break the loop
    if (bytesRead == 0) {
      printf("File received and saved: %s\n", local_file_path);
      break;
    }
  }

  printf("\n");
  fclose(file);
}
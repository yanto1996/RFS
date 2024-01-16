#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"
#define BUFFER_SIZE 8912

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t command_mutex = PTHREAD_MUTEX_INITIALIZER;

// this function saves a file sent by the client to the remote file path
// if the content being sent from the client side is null or empty the server will not save
// else it will save the content under the remote path directory
// if the client is saving it under the same directory, version control is implemented by adding _x 
// after each identical file for each x is the version
// AI used to help verify the logic of the do-while loop
void save_file(int client_sock, const char *remote_file_path) {
  // version control implementation: create a new file path array
  FILE *file;
  char new_file_path[256];

  // create a character buffer and intialize all elements to 0
  unsigned char check_buffer[BUFFER_SIZE] = {0};
  // Attempt to receive the data from the server but uses MSG_PEEK flag to not remove it from the socket. This checks if there is data to parse into the local path
  int checkBytesRead = recv(client_sock, check_buffer, sizeof(check_buffer), MSG_PEEK);

  // If no bytes were received, do not create the file and return
  if (checkBytesRead == 0) {
    printf("No bytes received. File not saved.\n");
    return;
  }

  // Check if the file already exists
  if (access(remote_file_path, F_OK) != -1) {
    // if the file already exists append the new version at the end of the file name
    int version = 1;
    // strrchr checks for the last occurance of "."
    char *extension = strrchr(remote_file_path, '.'); 

    do {
      // constructs the new filename with version number
      // check if extension is NULL
      if (extension != NULL) {
        // append the _x beefore the extension for version controlling
        snprintf(new_file_path, sizeof(new_file_path), "%.*s_%d%s", (int)(extension - remote_file_path), remote_file_path, version, extension);
      }

      else {
        // keep the version as is
        snprintf(new_file_path, sizeof(new_file_path), "%s_%d", remote_file_path, version);
      }

      version++;
      // checks if the file already exists. if so do while loop continues
    } while (access(new_file_path, F_OK) != -1);
        file = fopen(new_file_path, "wb");
  }

  else {
    // File doesn't exist, create the file
    file = fopen(remote_file_path, "wb");
  }

  // check if file is NULL and prints error if NULL and returns
  if (file == NULL) {
    perror("Error creating file");
    return;
  }

  // create a loop that breaks once all the bytes have been read
  while (1) {
    // create a character buffer an set all elements to 0
    unsigned char buffer[BUFFER_SIZE] = {0};
    // calculate the # of bytes the client is sending over
    int bytesRead = recv(client_sock, buffer, sizeof(buffer), 0);

    // if there are still bytes left keep parsing into the file
    if (bytesRead > 0) {
      // Print the data being sent
      printf("Read %d bytes from local file: %.*s\n", bytesRead, bytesRead, buffer);
      fwrite(buffer, 1, bytesRead, file);
    }

    // once there are no more bytes print success message and break the loop
    if (bytesRead == 0) {
      printf("File received and saved: %s\n", new_file_path);
      break;
    }
  }

  fclose(file);
}

// This function sends a file in the remote path to the local path
// If the file is not present in the remote path print error and return
// else it reads the file and sends the content to the client
void send_file(int client_sock, const char *remote_file_path) {
  // Open the remote file for reading
  FILE *file = fopen(remote_file_path, "rb");

  if (file == NULL) {
    perror("Error opening remote file");
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
      // sent the content to the client
      // if the number of bytes != integer bytesRead indicates there was an error
      if (send(client_sock, buffer, bytesRead, 0) != bytesRead) {
        // print error and break
        perror("Error sending file");
        break;
      }

      // Print the data being sent
      printf("Sent %d bytes from remote file: %.*s\n", bytesRead, bytesRead, buffer);
    }

    // check if we have reached the end of the file. return success message and breaks
    if (bytesRead < BUFFER_SIZE) {
      if (feof(file)) {
        printf("File sent successfully.\n");
      }

      break;
    }
  }

  fclose(file);
}

// This function removes the file from designated remote path and all its versions
// If the file is there, remove it and append any possible extensions and try to remove those are well
// else return 0
int remove_file(const char *remote_file_path) {
  // remove the original file and see if it is successful
  // if upon success, we will try and remove all its versions as well
  if (remove(remote_file_path) == 0) {
    // create version variable to help with appending to the new version file name
    // create removedVersion variable to track how many versions we have removed
    int version = 1;
    int removedVersions = 1;

    // create a loop that breaks once we cannot find another version of the original file
    while(1) {
      // create a buffer that stores the new version file path
      char versioned_file_path[256];

      // strrchr checks for the last occurance of "."
      char *extension = strrchr(remote_file_path, '.');

      // construct new filepath with version number
      // check if extension is null
      if (extension != NULL) {
        // calculate the position of the extension in the original path
        size_t extension_pos = extension - remote_file_path;

        // copy the path up to the extension
        strncpy(versioned_file_path, remote_file_path, extension_pos);

        // insert the version number
        snprintf(versioned_file_path + extension_pos, sizeof(versioned_file_path) - extension_pos, "_%d%s", version, extension);
      } 

      else {
        // no extension in the original path
        snprintf(versioned_file_path, sizeof(versioned_file_path), "%s_%d", remote_file_path, version);
      }


      printf("Attempting to remove: %s\n", versioned_file_path);

      // check if the versioned file path exists
      if (access(versioned_file_path, F_OK) == -1) {
        // versioned file not found, exit the loop
        printf("Versioned file not found: %s\n", versioned_file_path);
        break;
      }

      // try to remove the versioned file
      if (remove(versioned_file_path) != 0) {
        // version not found or error removing versioned file
        perror("Error removing versioned file");
        printf("Failed to remove: %s\n", versioned_file_path);
        break;
      }

      printf("Successfully removed: %s\n", versioned_file_path);

      removedVersions++;
      version++;
    }

    // print the number of versions removed
    printf("Removed %d versions of the file: %s\n", removedVersions, remote_file_path);
    return 1;
  } 

  else {
    // file not found or error removing file
    return 0;
  }
}

// This function controls the threads to allow the server to handle multiple clients at once
// the thread will read and process the command line arguments passed in by the client
// mutexes are placed when the server reads the command from the client and when it processes the command
void *handle_client(void *args) {
  // allows the thread function to access the ClientThreadArgs data structure to read command line
  // and process the work depending on the command call
  ClientThreadArgs *client_args = (ClientThreadArgs *)args;
  int client_sock = client_args->client_sock;

  // create a buffer to read the command line
  char command_buffer[BUFFER_SIZE];
  // fill the buffer with null characters
  memset(command_buffer, '\0', sizeof(command_buffer));
  // lock the thread
  pthread_mutex_lock(&command_mutex);

  // Receive and process the command
  recv(client_sock, command_buffer, sizeof(command_buffer), 0);
  printf("Received command: %s\n", command_buffer);

  // unlock the thread
  pthread_mutex_unlock(&command_mutex);

  // Parse the command using strtok to tokenize the command
  char *token = strtok(command_buffer, "|");

  // if the token is not NULL execute each command based on what was passed in
  if (token != NULL) {
    // lock the mutex before entering the functions that do work for the server 
    pthread_mutex_lock(&file_mutex);
    // if the token is WRITE perform the WRITE function
    if (strcmp(token, "WRITE") == 0) {   
      char *local_file_path = strtok(NULL, "|");
      char *remote_file_path = strtok(NULL, "|");

      // print the command line arguments
      printf("Command: %s\n", token);
      printf("Local File Path: %s\n", local_file_path);
      printf("Remote File Path: %s\n", remote_file_path);

      // if there is no path provided send the client a message "NULL PATH"
      if (strcmp(local_file_path, "(null)") == 0 && strcmp(remote_file_path, "(null)") == 0){
        send(client_sock, "NULL PATH", strlen("NULL PATH"), 0);
      }

      // if the arguments are passed in correctly send a ready response back to the client 
      // call the save_file function to write the file from local path to remote path
      else{
        // Send a "READY" response to the client
        send(client_sock, "READY", strlen("READY"), 0);

        // Save the file // WRITE function call
        save_file(client_sock, remote_file_path);
      }
    }

    // if the command is GET perform the GET function 
    else if (strcmp(token, "GET") == 0) {
      char *remote_file_path = strtok(NULL, "|");
      char *local_file_path = strtok(NULL, "|");

      // print the command line arguments
      printf("Command: %s\n", token);
      printf("Remote File Path: %s\n", remote_file_path);
      printf("Local File Path: %s\n", local_file_path);

      // Send a "READY" response to the client
      send(client_sock, "READY", strlen("READY"), 0);

      // wait for response from client before sending the file to avoid overlap
      char response[BUFFER_SIZE];
      memset(response, '\0', sizeof(response));
      recv(client_sock, response, sizeof(response), 0);
      printf("Client response: %s\n", response);

      // if the client responds with READY, send the file off
      if (strcmp(response, "READY") == 0) {
        // Send the File // GET file for the client
        send_file(client_sock, remote_file_path);
      }
    }

    // if the command is RM perform the RM function
    else if (strcmp(token, "RM") == 0) {
      char *remote_file_path = strtok(NULL, "|");

      // print the command line arguments
      printf("Command: %s\n", token);
      printf("Remote File Path: %s\n", remote_file_path);

      // if the path provided does not exist send the client "NULL PATH"
      if (strcmp(remote_file_path, "(null)") == 0){
        send(client_sock, "NULL PATH", strlen("NULL PATH"), 0);
      }

      else{
        // Send a "READY" response to the client
        send(client_sock, "READY", strlen("READY"), 0);

        // wait for the client to respond "READY"
        char response[BUFFER_SIZE];
        memset(response, '\0', sizeof(response));
        recv(client_sock, response, sizeof(response), 0);
        printf("Client response: %s\n", response);

         // once client responds check if the file exists and send it over to client
          if (strcmp(response, "READY") == 0) {
          // Check if the file exists
            if (access(remote_file_path, F_OK) != 0) {
              // File doesn't exist, send a message to the client
              send(client_sock, "NOT_FOUND", strlen("NOT_FOUND"), 0);
            }
            else{
              if (remove_file(remote_file_path)) {
                send(client_sock, "REMOVED", strlen("REMOVED"), 0);
              }        
            }
          }
        }
      }

    else {
      printf("Error: Invalid command %s\n", token);
    }
    // unlock mutex
    pthread_mutex_unlock(&file_mutex);
  }

  // Free the memory allocated for thread arguments
  free(client_args);

  printf("Thread %lu finished handling client\n", pthread_self());
  printf("\n");

  // Close the connection
  close(client_sock);

  // Exit the thread
  pthread_exit(NULL);
}
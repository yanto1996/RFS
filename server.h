#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#define BUFFER_SIZE 8912

// create thread structure
typedef struct Thread{
  int client_sock;
} 
ClientThreadArgs;

void save_file(int client_sock, const char *remote_file_path);

void send_file(int client_sock, const char *remote_file_path);

int remove_file(const char *remote_file_path);

void *handle_client(void *args);
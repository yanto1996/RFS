#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUFFER_SIZE 8192

void send_command(int socket_desc, const char *command, const char *arg1, const char *arg2);

void send_file(int socket_desc, const char *local_file_path, const char *remote_file_path);

void save_file(int socket_desc, const char *remote_file_path, char *local_file_path);
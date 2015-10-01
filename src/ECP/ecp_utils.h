#ifndef __ECP_UTILS_H__
#define __ECP_UTILS_H__

#define GN 9
#define ECP_PORT 58000
#define BUFFER_SIZE 128
#define TOPIC_SIZE 25
#define NR_TOPICS 5
#define AWT_STRING 128
#define TOPICID_INDEX 4
#define LINE_READ_SIZE 128

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <getopt.h>

void TQR_command(char* awtString);
void TER_command(char* awtesString, int topicID);
void IQR_command(char* buffer, char* awiString);
void printRequest(char* request, struct sockaddr_in* clientaddr);

#endif // __ECP_UTILS_H__
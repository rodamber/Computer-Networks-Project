#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define GN 9
#define ECP_PORT 58000
#define BUFFER_SIZE 128
#define TOPIC_SIZE 25
#define NR_TOPICS 5
#define AWT_STRING 128

int main(int argc, char *argv[]){
	int option = 0;
	int ecpPort = ECP_PORT + GN;
	
	int fd;
	struct hostent *hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	int addrlen;
	
	char buffer[BUFFER_SIZE];

	char topic[TOPIC_SIZE];
	char awtString[AWT_STRING] = "AWT ";
	
	FILE *fp;
	int i;
	char nrTopics[2];

	while ((option = getopt(argc, argv, ":p")) != -1){
		switch (option){
			case 'p': 
				ecpPort = atoi (optarg);
				break;
			default:
				break;
		}
		
	}
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd==-1){
		fprintf(stderr, "Erro ao criar o socket UDP"); /*colocamos o errno?*/
		exit(EXIT_FAILURE);
	}
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((u_short)ecpPort);
	
	bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	
	addrlen = sizeof(clientaddr);
	
	recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientaddr, &addrlen);
	
	if (strncmp(buffer, "TQR", 3) == 0){
		fp = fopen("topics.txt", "r");
		if(fp == NULL){
			fprinf(stderr, "Erro ao abrir o ficheiro topics.txt");
			exit(EXIT_FAILURE);
		} 
		else{
			sprintf(nrTopics, "%d", NR_TOPICS);
			strcat(awtString, nrTopics);
			strcat(awtString, " ");
			
			for(i = 0; i < NR_TOPICS-1; i++){
				fscanf(fp, "%s%*[^\n]", topic);
				strcat(awtString, topic);
				strcat(awtString, " ");
			}
			fscanf(fp, "%s%*[^\n]", topic);
			strcat(awtString, topic);
			strcat(awtString, "\n"); 

			sendto(fd, awtString, strlen(awtString)+1, 0, (struct sockaddr*)&clientaddr, addrlen);
			
			fclose(fp);
		}
		//if the TQR request is not correctly formulated the reply is string "ERR"
	}
	else if (strncmp(buffer, "TER", 3) == 0){
		//...
	}
	
	
	
	
	
	
	
	
	//sendto(fd, msg, strlen(msg)+1, 0, (struct sockaddr*)&clientaddr, addrlen);
	
	close(fd);
	
	return 0;
}
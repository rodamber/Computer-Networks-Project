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
#define TOPICID_INDEX 4
#define LINE_READ_SIZE 128

int main(int argc, char *argv[]){
	int option = 0;
	int ecpPort = ECP_PORT + GN;
	
	int fd;
	struct hostent *hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	int addrlen;
	
	char buffer[BUFFER_SIZE];
	
	FILE *fp;
	
	int i;
	char topic[TOPIC_SIZE];
	char awtString[AWT_STRING];
	char nrTopics[2];
	
	int topicID;
	int linesRead = 1;
	char awtesString[AWT_STRING];
	char lineRead[LINE_READ_SIZE];
	char *pch;
	
	char awiString[AWT_STRING];
	char* substr;
	
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
		fprintf(stderr, "Erro ao criar o socket UDP"); /*do we use errno?*/
		exit(EXIT_FAILURE);
	}
	
	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((u_short)ecpPort);
	
	bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	
	addrlen = sizeof(clientaddr);
	
	while(1){
		recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientaddr, &addrlen);
		
		if (strncmp(buffer, "TQR", 3) == 0){
			fp = fopen("topics.txt", "r");
			if(fp == NULL){
				fprintf(stderr, "Erro ao abrir o ficheiro topics.txt\n");
				exit(EXIT_FAILURE);
			} 
			strcpy(awtString, "AWT ");
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
			
			memset(awtString, 0, strlen(awtString));
			
			fclose(fp);
		}
		else if (strncmp(buffer, "TER", 3) == 0){
			topicID = buffer[TOPICID_INDEX];
			
			fp = fopen("topics.txt", "r");
			if(fp == NULL){
				fprintf(stderr, "Erro ao abrir o ficheiro topics.txt\n");
				exit(EXIT_FAILURE);
			} 
			while (fgets(lineRead, sizeof(lineRead), fp) != NULL){
				if (linesRead == topicID){
					break;
				}
				else{
					linesRead++;
				}
			}
			linesRead = 1;
			
			strcpy(awtesString, "AWTES ");
			pch = strtok (lineRead," "); //topic name (ignored)
			pch = strtok (NULL, " "); //topic id address
			strcat(awtesString, pch);
			strcat(awtesString, " ");
			pch = strtok (NULL, " "); //topic port number (with \n)
			strcat(awtesString, pch);

			sendto(fd, awtesString, strlen(awtesString)+1, 0, (struct sockaddr*)&clientaddr, addrlen);
			
			memset(awtesString, 0, strlen(awtesString));
			
			fclose(fp);
		}
		else if(strncmp(buffer, "IQR", 3) == 0){
			fp = fopen("stats.txt", "a");
			if(fp == NULL){
				fprintf(stderr, "Erro ao abrir o ficheiro stats.txt\n");
				exit(EXIT_FAILURE);
			}
			buffer[strlen(buffer)] = '\0';
			strncpy(substr, buffer+4, strlen(buffer)-3);
			fprintf(fp, "%s", substr);
			
			strcpy(awiString, "AWI ");
			pch = strtok (buffer," "); //IQR (ignored)
			pch = strtok (NULL, " "); //SID (ignored)
			pch = strtok (NULL, " "); //QID
			strcat(awiString, pch);
			strcat(awiString, "\n");
			
			sendto(fd, awiString, strlen(awiString)+1, 0, (struct sockaddr*)&clientaddr, addrlen);
		
			fclose(fp);
		}
		else{
			sendto(fd, "ERR\n", strlen("ERR\n")+1, 0, (struct sockaddr*)&clientaddr, addrlen);
		}
	}
	close(fd);
	
	exit(EXIT_SUCCESS);
}
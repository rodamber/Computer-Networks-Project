#include "ecp.h"

void TQR_command(char* awtString){
    FILE *fp;
    int i;
    char topic[TOPIC_SIZE];
    char nrTopics[2];

    fp = fopen("topics.txt", "r");
    if(fp == NULL){
        fprintf(stderr, "Error opening topics.txt\n");
        exit(EXIT_FAILURE);
    }

    memset(awtString, 0, strlen(awtString));
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

    fclose(fp);
}

void TER_command(char* awtesString, int topicID){
    FILE *fp;
    int linesRead = 1;
    char lineRead[LINE_READ_SIZE];
    char *pch;

    fp = fopen("topics.txt", "r");
    if(fp == NULL){
        fprintf(stderr, "Error opening topics.txt\n");
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

    memset(awtesString, 0, strlen(awtesString));
    strcpy(awtesString, "AWTES ");
    pch = strtok (lineRead," "); //topic name (ignored)
    pch = strtok (NULL, " "); //topic id address
    strcat(awtesString, pch);
    strcat(awtesString, " ");
    pch = strtok (NULL, " "); //topic port number (with \n)
    strcat(awtesString, pch);

    fclose(fp);
}

void IQR_command(char* buffer, char* awiString){
    FILE* fp;
    char* substr = NULL;
    char *pch;

    fp = fopen("stats.txt", "a");
    if(fp == NULL){
        fprintf(stderr, "Error opening stats.txt\n");
        exit(EXIT_FAILURE);
    }
    substr = strndup(buffer+4, strlen(buffer)-3);
    fprintf(fp, "%s", substr);

    strcpy(awiString, "AWI ");
    pch = strtok (buffer," "); //IQR (ignored)
    pch = strtok (NULL, " "); //SID (ignored)
    pch = strtok (NULL, " "); //QID
    strcat(awiString, pch);
    strcat(awiString, "\n");

    free(substr);
    fclose(fp);
}

void printRequest(char* request, struct sockaddr_in* clientaddr){
    char *clientIP;
    int clientPort;

    clientIP = inet_ntoa(clientaddr->sin_addr);
    clientPort = clientaddr->sin_port;
    printf("%sIP: %s\nPort: %d\n", request, clientIP, clientPort);
}

int main(int argc, char *argv[]){
    int option = 0, ecpPort = ECP_PORT + GN;
    //struct hostent *hostptr;
    struct sockaddr_in serveraddr, clientaddr;
    int fd, addrlen, ret, topicID, nread;
    char buffer[BUFFER_SIZE], awtString[AWT_STRING], awtesString[AWT_STRING], awiString[AWT_STRING];

    while ((option = getopt(argc, argv, "p:")) != -1){
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
        fprintf(stderr, "Error creating UDP socket\n");
        exit(EXIT_FAILURE);
    }

    memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)ecpPort);

    ret = bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(ret==-1){
        fprintf(stderr, "Error binding\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        addrlen = sizeof(clientaddr);
        nread = recvfrom(fd, buffer, sizeof(buffer)+1, 0, (struct sockaddr*)&clientaddr, (unsigned*)&addrlen);
        if (nread==-1){
            fprintf(stderr, "Error receiving user request\n");
            exit(EXIT_FAILURE);
        }
        buffer[nread] = '\0';

        if (strncmp(buffer, "TQR", 3) == 0){
            printRequest(buffer, &clientaddr);
            TQR_command(awtString);
            ret = sendto(fd, awtString, strlen(awtString)+1, 0, (struct sockaddr*)&clientaddr, addrlen);
            if(ret==-1){
                fprintf(stderr, "Error sending reply to user\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strncmp(buffer, "TER", 3) == 0){
            printRequest(buffer, &clientaddr);
            topicID = buffer[TOPICID_INDEX]-'0';
            TER_command(awtesString, topicID);
            ret = sendto(fd, awtesString, strlen(awtesString)+1, 0, (struct sockaddr*)&clientaddr, addrlen);
            if(ret==-1){
                fprintf(stderr, "Error sending reply to user\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(strncmp(buffer, "IQR", 3) == 0){
            printRequest(buffer, &clientaddr);
            IQR_command(buffer, awiString);
            ret = sendto(fd, awiString, strlen(awiString), 0, (struct sockaddr*)&clientaddr, addrlen);
            if(ret==-1){
                fprintf(stderr, "Error sending reply to user\n");
                exit(EXIT_FAILURE);
            }
        }
        else{
            printRequest(buffer, &clientaddr);
            ret = sendto(fd, "ERR\n", strlen("ERR\n")+1, 0, (struct sockaddr*)&clientaddr, addrlen);
            if(ret==-1){
                fprintf(stderr, "Error sending reply to user\n");
                exit(EXIT_FAILURE);
            }
        }
        memset(buffer, 0, strlen(buffer));
    }
    close(fd);

    exit(EXIT_SUCCESS);
}

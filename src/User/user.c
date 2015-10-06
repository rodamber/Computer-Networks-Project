#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../utils.h"

#define QID_SIZE 24

#define PORTUDP 58000   /*porto para UDP*/
#define PORTTCP 59000   /*porto para TCP*/

extern int errno;     /*erro*/

int main(int argc, char **argv){

    char sid[10];   /*SID*/
    char ecpname[128];   /*ECPname*/
    char ecpport[10];   /*ECPport*/

    char command[30]; /*comando do programa*/
    int flag; /*permite saber se o comando é correto*/

    /*UDP*/

    int udp, n,addrlen_udp;
    struct hostent *hostptr_udp;
    struct sockaddr_in serveraddr_udp;

    /*usados no list*/

    char buffer_udp[500];
    int n1;
    char* list[99];
    int ntopics;
    char* tqrreply;
    char* token1;
    int indice;

    /*usados no request*/

    char topic[30];
    char msg[30]="";
    char qid[25];
    char answers[5][5];
    char qid_prov[1];

    /*usados no aqs*/

    char aqs[5];
    char qid2[30];
    char score[20];

    /* AQT */

    char aqt[6];
    char time[20];
    char size[30];

    /* PDF */

    char pdf_name[20]="ficheiro.pdf";
    int file;
    size_t n_bytes;
    ssize_t n_bytes_written;
    int close_success;
    int iteracoes;

    /*TCP*/

    int tcp;
    struct hostent *hostptr_tcp;
    struct sockaddr_in serveraddr_tcp;
    char ip[128];
    char tcpport[10];

    /* Verificação de introdução de input */

    if(argc>=2){
        if((strlen(argv[1])/sizeof(char))==5){
            strcpy(sid,argv[1]);
        }
        else{
            fprintf(stderr, "error: sid must have 5 digits\n");
            return 0;
        }
        if(argc==2){
            if(gethostname(ecpname,128)==-1){
                perror("error: ");
                exit(1);
            }
            strcpy(ecpport,"58009");
        }
        if(argc==3){
            fprintf(stderr, "error: insert ECPname\n");
            return 0;
        }
        if(argc>=4){
            if(strcmp(argv[2],"-n")==0)
                strcpy(ecpname,argv[3]);
            else{
                fprintf(stderr, "error: -n not inserted\n");
                return 0;
            }
            if(argc==4){
                strcpy(ecpport,"58009");
            }
      if(argc==5){
                fprintf(stderr, "error: insert ECPport\n");
                return 0;
            }
            if(argc==6){
                if(strcmp(argv[4],"-p")==0)
                    strcpy(ecpport,argv[5]);
                else{
                    fprintf(stderr, "error: -p not inserted\n");
                    return 0;
                }
            }
        }
    }
    else{
        fprintf(stderr, "error: insert sid\n");
        return 0;
    }


    /* UDP UDP UDP UDP UDP UDP UDP UDP UDP UDP */
    /*Criar socket UDP*/

    udp=socket(AF_INET,SOCK_DGRAM,0);
    if(udp==-1){
        perror("error: ");
        exit(1);
    }

    /*Buscar adress do servidor ECP*/

    if((hostptr_udp=gethostbyname(ecpname))==NULL){
        perror("error: ");
        exit(1);
    }

    /*Associar socket a adress do servidor ECP*/

    memset((void*)&serveraddr_udp, (int)'\0',sizeof(serveraddr_udp));
    serveraddr_udp.sin_family=AF_INET;
    serveraddr_udp.sin_addr.s_addr=((struct in_addr *)(hostptr_udp->h_addr_list[0]))->s_addr;
    serveraddr_udp.sin_port=htons((unsigned short)(atoi(ecpport)));
    addrlen_udp=sizeof(serveraddr_udp);

    /*Comandos do programa*/

    do {scanf("%s",command);
        flag=1;

        if (strcmp(command,"list")==0){

            flag=0;

            /* Envia TQR para o ECP atraves do socket UDP */


            sendto(udp,"TQR\n",strlen("TQR\n"),0,(struct sockaddr*)&serveraddr_udp,addrlen_udp);

            /* Recebe topicos do ECP por UDP */

            n1=recvfrom(udp,buffer_udp,sizeof(buffer_udp),0,(struct sockaddr*)&serveraddr_udp,&addrlen_udp);
            if(n1==-1){
          perror("error: ");
                exit(1);
            }

            /* parte string em string mais pequenas e preenche lista com strings */

            token1=strtok(buffer_udp," ");
            tqrreply=token1;

            if(strcmp(tqrreply,"EOF")==0)
                fprintf(stderr, "error: Reply not sent by ECP\n");
            if(strcmp(tqrreply,"ERR")==0)
                fprintf(stderr, "error: TQR not correctly formulated\n");
            if(strcmp(tqrreply,"AWT")==0){
                token1=strtok(NULL," ");
                ntopics=atoi(token1);
                token1=strtok(NULL," ");
                indice=0;
                while(token1!=NULL){
                    list[indice]=token1;
                    token1=strtok(NULL," ");
                    indice++;
                }
                if(indice!=ntopics){
                    fprintf(stderr, "error: Number of topics received is incorrect\n");
                    exit(1);
                }
            }
            else
                fprintf(stderr, "error: Incorrect reply from ECP\n");

            /*imprime topicos de questinario*/

            indice=1;
            while(indice<(ntopics+1)){
                printf("%d- %s\n",indice,list[indice-1]);
                indice++;
            }

        }

        if (strcmp(command,"request")==0){

            /* le numero do topico */

            scanf("%s",topic);

            /* pede ao TES detalhes do seu endereco por UDP */

            strcpy(msg,"");
            strcat(msg,"TER ");
            strcat(msg,topic);
            strcat(msg,"\n");
            sendto(udp,msg,strlen(msg),0,(struct sockaddr*)&serveraddr_udp,addrlen_udp);

            /* recebe IP e Port do TES */

            n1=recvfrom(udp,buffer_udp,sizeof(buffer_udp),0,(struct sockaddr*)&serveraddr_udp,&addrlen_udp);
            if(n1==-1){
          perror("error: ");
                exit(1);
            }
            token1=strtok(buffer_udp," ");
            tqrreply=token1;

            if(strcmp(tqrreply,"EOF")==0)
                fprintf(stderr, "error: Reply not sent by ECP\n");
            if(strcmp(tqrreply,"ERR")==0)
                fprintf(stderr, "error: TER not correctly formulated\n");
            if(strcmp(tqrreply,"AWTES")==0){
                token1=strtok(NULL," ");
                strcpy(ip,token1);
                printf("%s\n",ip);
                token1=strtok(NULL," ");
                strcpy(tcpport,token1);
                printf ("%s\n",tcpport);
            }
            else
                fprintf(stderr, "error: Incorrect reply from ECP\n");

            /* criar socket TCP */

            tcp=socket(AF_INET,SOCK_STREAM,0);
            if(tcp==-1){
                perror("error: ");
                exit(1);
            }

            /*Buscar adress do servidor TCP*/

            if((hostptr_tcp=gethostbyname(ip))==NULL){
                perror("error: ");
                exit(1);
            }

            /*Associar socket a adress do servidor TCP*/

            memset((void*)&serveraddr_tcp,(int)'\0',sizeof(serveraddr_tcp));
            serveraddr_tcp.sin_family=AF_INET;
            serveraddr_tcp.sin_addr.s_addr=((struct in_addr *)(hostptr_tcp->h_addr_list[0]))->s_addr;
            serveraddr_tcp.sin_port=htons((unsigned short)(atoi(tcpport)));

            /* conexao TCP */

            n=connect(tcp,(struct sockaddr*)&serveraddr_tcp,sizeof(serveraddr_tcp));
            if(n==-1){
                perror("error: ");
                exit(1);
            }

            /* enviar RQT */

            strcpy(msg,"");
            strcat(msg,"RQT ");
            strcat(msg,sid);
            strcat(msg,"\n");

            n=write(tcp,msg,strlen(msg));
            if(n<=0){
                perror("error: ");
                exit(1);
            }

            /* recebe AQT */

            if(read_bytes(tcp, 4, aqt) == -1){
                perror("error: ");
                exit(1);
            }

            if (strncmp(aqt, "AQT ", 4) != 0) {
                printf("ERR\n");
                exit(1);
            }

            /* recebe QID */
            if ((n = read_bytes(tcp, QID_SIZE, qid)) == -1 || n != QID_SIZE) {
                printf("ERR\n");
                perror("error: ");
                exit(1);
            }

            /* recebe time */
            if((n = read_bytes(tcp, (19 * sizeof(char)), time)) == -1){
                perror("error: ");
                exit(1);
            } else if (n != 19) {
                printf("ERR\n");
                exit(1);
            }

            /* recebe size */
            n=read(tcp,qid_prov,sizeof(char));
            if(n==-1){
                perror("error: ");
                exit(1);
            }
            n1=0;
            while(qid_prov[0]!=' '){
                size[n1]=qid_prov[0];
                n=read(tcp,qid_prov,sizeof(char));
                if(n==-1){
                    perror("error: ");
                    exit(1);
                }
                n1++;
            }
            size[n1]='\0';

            /*recebe data e escreve no ficheiro*/


            file = open(pdf_name, O_WRONLY | O_CREAT,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );

            if (file == -1)
                return -1;

            /* escrever no ficheiro */
            n1 = 0;
            int size_i = atoi(size);
            char pdf_conteud[1];

            while(n1 < size_i){
                n = read(tcp, pdf_conteud, sizeof(pdf_conteud));

                if(n == -1){
                    perror("error: ");
                    exit(1);
                }

                n_bytes_written = write(file, pdf_conteud, sizeof(pdf_conteud));

                if(n_bytes_written != sizeof(pdf_conteud) || close(file) == -1) {
                    return -1;
                }
                n1++;
            }

            if(close(file) == -1)
                return -1;

            printf("PDF transfered\n");

            char buffer_tcp[500];
            while(strcmp(buffer_tcp, "") != 0){
                n = read(tcp, buffer_tcp, sizeof(buffer_tcp));
                if(n == -1) {
                    perror("error: ");
                    exit(1);
                }
                printf("%s\n",buffer_tcp);
            }
            flag = 0;
        }



        if (strcmp(command,"submit")==0){

            /* receber respostas do utilizador */

            n=0;
            while(n<5){
                scanf("%s",answers[n]);
                if((strcmp(answers[n],"A")!=0)&&(strcmp(answers[n],"B")!=0)&&(strcmp(answers[n],"C")!=0)&&(strcmp(answers[n],"D")!=0))
                    fprintf(stderr, "error: answer must be A, B, C or D\n");
                if(strcmp(answers[n],"")==0)
                    strcpy(answers[n],"N");
                n++;
            }

            if(n>5){
                fprintf(stderr, "error: only 5 answers are accepted\n");
                exit(1);
            }

            /* construir e enviar RQS */

            strcpy(msg,"");
            strcat(msg,"RQS ");
            strcat(msg,sid);
            strcat(msg," ");
            strcat(msg,qid);
            n=0;
            while(n<5){
                strcat(msg," ");
                strcat(msg,answers[n]);
                n++;
            }
            strcat(msg,"\n");

            n=write(tcp,msg,strlen(msg));
            if(n<=0){
                perror("error: ");
                exit(1);
            }

            /* receber AQS */

            n=read(tcp,aqs,(4*sizeof(char)));
            if(n==-1){
                perror("error: ");
                exit(1);
            }

            token1=strtok(aqs," ");
            strcpy(aqs,token1);

            printf("%s\n",aqs);

            if(strcmp(aqs,"AQS")!=0){
                printf("ERR\n");
                exit(1);
            }

            /* recebe QID */

            n=read(tcp,qid_prov,sizeof(char));
            if(n==-1){
                perror("error: ");
                exit(1);
            }


            n1=0;
            while(qid_prov[0]!=' '){

                qid[n1]=qid_prov[0];
                n=read(tcp,qid_prov,sizeof(char));
                if(n==-1){
                    perror("error: ");
                    exit(1);
                }
                n1++;
            }
            qid[n1]='\0';

            /* recebe time */

            n=read(tcp,score,(3*sizeof(char)));
            if(n==-1){
                perror("error: ");
                exit(1);
            }
            token1=strtok(score," ");
            strcpy(score,token1);

            /* imprimir score */

            printf("Obtained score: %s\n",score);

            flag=0;

        }

  if (strcmp(command,"exit")==0)
     flag=0;

  if (flag)
            fprintf(stderr, "error: unknown command\n");

    }while(strcmp(command,"exit")!=0);

    close(udp);
    close(tcp);

    return 0;
}

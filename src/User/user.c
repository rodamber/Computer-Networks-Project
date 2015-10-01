#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

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

    char* topic;
    char* msg="";

    /*TCP*/

    int tcp;
    struct hostent *hostptr_tcp;
    struct sockaddr_in serveraddr_tcp;
    char* ip;
    char* tcpport;

    /* Verificação de introdução de input */

    if(argc>=2){
        if((strlen(argv[1])/sizeof(char))==5){
            strcpy(sid,argv[1]);
        }
        else{
            printf("error: sid must have 5 digits\n");
            return 0;
        }
        if(argc==2){
            if(gethostname(ecpname,128)==-1){
                printf("error: %s\n",strerror(errno));
                exit(1);
            }
            strcpy(ecpport,"58000");
        }
        if(argc==3){
            printf("error: insert ECPname\n");
            return 0;
        }
        if(argc>=4){
            if(strcmp(argv[2],"-n")==0)
                strcpy(ecpname,argv[3]);
            else{
                printf("error: -n not inserted\n");
                return 0;
            }
            if(argc==4){
             /*buscar ECPport que sera 58000 + GN (group number)*/
                strcpy(ecpport,"58000");
            }
	    if(argc==5){
                printf("error: insert ECPport\n");
                return 0;
            }
            if(argc==6){
                if(strcmp(argv[4],"-p")==0)
                    strcpy(ecpport,argv[5]);
                else{
                    printf("error: -p not inserted\n");
                    return 0;
                }
            }       
        }      
    }
    else{
        printf("error: insert sid\n");
        return 0;
    }


    /* UDP UDP UDP UDP UDP UDP UDP UDP UDP UDP */
    /*Criar socket UDP*/
    
    udp=socket(AF_INET,SOCK_DGRAM,0);
    if(udp==-1){
        printf("error: %s\n",strerror(errno));
        exit(1);
    }
    
    /*Buscar adress do servidor ECP*/
 
    if((hostptr_udp=gethostbyname(ecpname))==NULL){
        printf("error: %s\n",strerror(errno));
        exit(1);
    }

    /*Associar socket a adress do servidor ECP*/
    
    memset((void*)&serveraddr_udp, (int)'\0',sizeof(serveraddr_udp));
    serveraddr_udp.sin_family=AF_INET;
    serveraddr_udp.sin_addr.s_addr=((struct in_addr *)(hostptr_udp->h_addr_list[0]))->s_addr;
    serveraddr_udp.sin_port=htons((u_short)(atoi(ecpport)));
    addrlen_udp=sizeof(serveraddr_udp);

    printf("%d\n",atoi(ecpport));
        
    /* TCP TCP TCP TCP TCP TCP TCP TCP TCP TPC */
    /*Criar socket TCP*/
    


    /*Comandos do programa*/
    
    do {scanf("%s",command);
        flag=1;
           
        if (strcmp(command,"list")==0){

            flag=0;

            /* Envia TQR para o ECP atraves do socket UDP */


            sendto(udp,"TQR\n",strlen("TQR\n"),0,(struct sockaddr*)&serveraddr_udp,addrlen_udp);
            
            /* Recebe topicos do ECP por UDP */
  
            recvfrom(udp,buffer_udp,sizeof(buffer_udp),0,(struct sockaddr*)&serveraddr_udp,&addrlen_udp);
            
            
           

            /* parte string em string mais pequenas e preenche lista com strings */
            
            token1=strtok(buffer_udp," ");
            tqrreply=token1;

            if(strcmp(tqrreply,"EOF")==0)
                printf("Error: Reply not sent by ECP\n");
            if(strcmp(tqrreply,"ERR")==0)
                printf("Error: TQR not correctly formulated\n");
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
                    printf("Error: Number of topics received is incorrect\n");
                    exit(1);
                }
            }
            else
                printf("Error: Incorrect reply from ECP\n");

            /*imprime topicos de questinario*/            

            indice=0;
            while(indice<ntopics){
                printf("%d- %s\n",indice,list[indice]);
                indice++;
            }
            break;

            printf("%s\n",buffer_udp);
        }
            
        if (strcmp(command,"request")==0){

            /* le numero do topico */

            scanf("%s",topic);

            /* pede ao TES detalhes do seu endereco por UDP */

            strcat(msg,"TER ");
            strcat(msg,topic);
            sendto(udp,msg,strlen(msg)+1,0,(struct sockaddr*)&serveraddr_udp,addrlen_udp);

            /* recebe IP e Port do TES */
            
            n1=recvfrom(udp,buffer_udp,1000,0,(struct sockaddr*)&serveraddr_udp,&addrlen_udp);
            if(n1==-1){
	        printf("error: %s\n",strerror(errno));
                exit(1);
            }
            token1=strtok(buffer_udp," ");
            tqrreply=token1;

            if(strcmp(tqrreply,"EOF")==0)
                printf("Error: Reply not sent by ECP\n");
            if(strcmp(tqrreply,"ERR")==0)
                printf("Error: TER not correctly formulated\n");
            if(strcmp(tqrreply,"AWTES")==0){
                token1=strtok(NULL," ");
                ip=token1;
                token1=strtok(NULL," ");
                tcpport=token1;    
            }
            else
                printf("Error: Incorrect reply from ECP\n");

            /* parte TCP */
            
            /* criar socket TCP */

            tcp=socket(AF_INET,SOCK_STREAM,0);

            hostptr_tcp=gethostbyname(ip);
            
            memset((void*)&serveraddr_tcp,(int)'\0',sizeof(serveraddr_tcp));
            serveraddr_tcp.sin_family=AF_INET;
            serveraddr_tcp.sin_addr.s_addr=((struct in_addr *)(hostptr_tcp->h_addr_list[0]))->s_addr;
            serveraddr_tcp.sin_port=htons((u_short)(atoi(tcpport)));
            
            /* conexao TCP */

            connect(tcp,(struct sockaddr*)&serveraddr_tcp,sizeof(serveraddr_tcp));

            /* enviar RQT */ 
            
            flag=0;
            break;
        }
          
        if (strcmp(command,"submit")==0){

            /* Chama a funcao responsavel pela execucao do comando submit */

            






            flag=0;
            break;
           
        }
	
	if (strcmp(command,"exit")==0)
	   flag=0;

	if (flag)
            printf("error: unknown command\n");
        
    }while(strcmp(command,"exit")!=0);

    close(udp);
    close(tcp);

    return 0;
}

                



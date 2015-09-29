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

    char* sid;   /*SID*/
    char* ecpname;   /*ECPname*/
    char* ecpport;   /*ECPport*/

    char* command; /*comando do programa*/
    int flag; /*permite saber se o comando é correto*/

    /*UDP*/

    int udp, n,addrlen_udp;
    struct hostent *hostptr_udp;
    struct sockaddr_in serveraddr_udp;

    /*usados no list*/

    char buffer_udp[1000];
    int n1;
    char* list[99];
    int ntopics;
    char* tqrreply;
    char* token1;
    int indice;

    /*TCP*/

 


    /* Verificação de introdução de input */

    if(argc>=2){
        if((strlen(argv[1])/sizeof(char))==5){
            sid=argv[1];
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
        }
        if(argc==3){
            printf("error: insert ECPname\n");
            return 0;
        }
        if(argc>=4){
            if(strcmp(argv[2],"-n")==0)
                ecpname=argv[3];
            else{
                printf("error: -n not inserted\n");
                return 0;
            }
            if(argc==4){
             /*buscar ECPport que sera 58000 + GN (group number)*/
            }
	    if(argc==5){
                printf("error: insert ECPport\n");
                return 0;
            }
            if(argc==6){
                if(strcmp(argv[4],"-p")==0)
                    ecpport=argv[5];
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
        
    /* TCP TCP TCP TCP TCP TCP TCP TCP TCP TPC */
    /*Criar socket TCP*/
    


    /*Comandos do programa*/
    
    do {scanf("%s",command);
        flag=1;
           
        if (strcmp(command,"list")==0){

            flag=0;

            /* Envia TQR para o ECP atraves do socket UDP */

            sendto(udp,"TQR\n",strlen("TQR\n")+1,0,(struct sockaddr*)&serveraddr_udp,addrlen_udp);
            
            /* Recebe topicos do ECP por UDP */
  
            n1=recvfrom(udp,buffer_udp,1000,0,(struct sockaddr*)&serveraddr_udp,&addrlen_udp);
            if(n1==-1){
	        printf("error: %s\n",strerror(errno));
                exit(1);
            }

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
            }
            else
                printf("Error: Incorrect reply from ECP\n");

            /*imprime topicos de questinario*/            

            indice=0;
            while(indice<ntopics){
                printf("%d- %s\n",indice,list[indice]);
                indice++;
            }
        }
            
        if (strcmp(command,"request")==0){

            /* Chama a funcao responsavel pela execucao do comando request */
            
            
            flag=0;
        }
          
        if (strcmp(command,"submit")==0){

            /* Chama a funcao responsavel pela execucao do comando submit */

            flag=0;
           
        }
	
	if (strcmp(command,"exit")==0)
	   flag=0;

	if (flag)
            printf("error: unknown command\n");
        
    }while(strcmp(command,"exit")!=0);

    close(udp);




    return 0;
}

                



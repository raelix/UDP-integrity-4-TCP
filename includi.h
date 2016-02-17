#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#define MAXSIZE 64000
#define SOCKET_ERROR   ((int)-1)
struct packet *elemento;
char buf[MAXSIZE]; 
int ordino=0; /* ordino i pacchetti nel Proxyreciver */
int a=0; 
uint16_t porta=61000;
uint16_t porta2=62000;
struct icmp{
	 int id;
	 char i;
	 uint32_t id_packet_lost;};
 
 
  struct packet{
	  int id;
	  char b;
	  char body[64000];
	  int dim;
	  struct packet *next;
	  };
/*-------------------------- INIZIO FUNZIONI PROXY SENDER-----------------------------------------------------------*/
int socketcp(int *socketfd,struct sockaddr_in *Local,uint16_t *portatcp,int *swich){
  
  int OptVal,ris;
  printf ("socket()\n");
  if (*swich==1){ *socketfd = socket(AF_INET, SOCK_DGRAM, 0);}  /* utilizzo uno swich per inizializzare 2 tipi di socket */
  if (*swich==0){ *socketfd = socket(AF_INET, SOCK_STREAM, 0);}
  if (*socketfd == SOCKET_ERROR) {
    printf ("socket() failed, Err: %d \"%s\"\n", errno,strerror(errno));
    exit(1);
  }

  /* evito che l indirizzo sia già in uso */
  OptVal = 1;
  printf ("setsockopt()\n");
  ris = setsockopt(*socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&OptVal, sizeof(OptVal));
  if (ris == SOCKET_ERROR)  {
    printf ("setsockopt() SO_REUSEADDR failed, Err: %d \"%s\"\n", errno,strerror(errno));
    exit(1);
  }

  memset ( Local, 0, sizeof(*Local) );
  Local->sin_family		=	AF_INET;
  Local->sin_addr.s_addr	=	htonl(INADDR_ANY);         
  Local->sin_port		=	htons(*portatcp);
  printf ("bind()\n");
  ris = bind(*socketfd, (struct sockaddr*) Local, sizeof(*Local));
    if (ris == SOCKET_ERROR)  {
    printf ("bind() failed, Err: %d \"%s\"\n",errno,strerror(errno));
    exit(1);
		}
	return(0);
			}
/*---------------------------LISTEN E ACCEPT TCP-------------------------------------------------------------------------*/

int acceptcp(int *socketfd,int *newsocketfd,struct sockaddr_in *Cli){
  int ris;
  int len;
  printf ("listen()\n");
  ris = listen(*socketfd, 10 );
    if (ris == SOCKET_ERROR)  {
    printf ("listen() failed, Err: %d \"%s\"\n",errno,strerror(errno));
    return(1);
  }
  memset ( Cli, 0, sizeof(*Cli) );
  len=sizeof(Cli);
  printf ("accept()\n");
  *newsocketfd = accept(*socketfd, (struct sockaddr*) Cli, (socklen_t*) &len);
    if (*newsocketfd == SOCKET_ERROR)  {
    printf ("accept() failed, Err: %d \"%s\"\n",errno,strerror(errno));
    return(1);
  }

  printf("connection from %s : %d\n", 
  	inet_ntoa(Cli->sin_addr), 
  	ntohs(Cli->sin_port)
  );
  
return(0);}
/*------------------------RICEVO E IMPACCHETTO---------------------------------------------------------------------------*/

int ricevotcp(int *newsocketfd,struct packet *s,int *socketud,struct sockaddr_in *Locale,char *p){
	
struct packet *support=(struct packet *)malloc(sizeof(struct packet)); /* support sarà il pacchetto che creo ogni volta*/
int n,nread=0;                                                         /* che ricevo dal tcp,con nread conto byte*/
int addr_size;

	
 if((n=read(*newsocketfd, &(support->body[nread]), sizeof(support->body) ))>0) /*copio direttamente dentro il body*/
	nread+=n;
	support->id=htonl(a);              /*id del pacchetto*/
    support->b='B';
    if(n<=0) {
    
   return(2);
  }
	while(s->next != NULL){                /*cerco l' ultimo elemento */
		s=s->next;
		}
	s->next=support;                      /*copio dopo l'ultimo elemento,,,nel primo caso inserito dopo la sentinella*/
	support->next=NULL;
	
    support->dim=nread;       /*passo al campo dim del pacchetto la dimensione del body*/
   
	addr_size               =sizeof(struct sockaddr_in);
    Locale->sin_addr.s_addr	=inet_addr(p);

Locale->sin_port        =htons(porta);

	sendto(*socketud, support, sizeof(struct packet), 0, (struct sockaddr *)Locale, addr_size); /* spedisco ogni volta che impacchetto*/
	printf("spedito pacchetto %d sent to (%d:%s) \n",
             ntohl(support->id), ntohs(Locale->sin_port),inet_ntoa(Locale->sin_addr));
      a++; /* incremento l'id del pacchetto */
          
return(0);

  }

  
/*-----------------------SPEDISCO UDP ----------------------------------------------------------------------*/

int spediscoudp(int *socketud,struct sockaddr_in *Locale,struct packet *nuovo,char *p){
	int addr_size;
	addr_size = sizeof(struct sockaddr_in);
    Locale->sin_addr.s_addr	=inet_addr(p); 


if((porta == 61000)||(porta==61001)||(porta=61002))
{
if (porta>=61002)  porta=61000;
else{ 
porta++;
}} 

Locale->sin_port =htons(porta);
	while(nuovo->b == 'B'  ){                                                  /*spedisco solo pacchetti di tipo BODY */
	sendto(*socketud, nuovo, sizeof(struct packet), 0, (struct sockaddr *)Locale, addr_size);
	printf("spedito pacchetto %d sent to (%d:%s)\n",
             ntohl(nuovo->id), ntohs(Locale->sin_port),inet_ntoa(Locale->sin_addr));
               
           if( nuovo->next != NULL && nuovo->next != nuovo) nuovo=nuovo->next;  /*controllo successivo non sia nullo o uguale */
           else break;
            }
             
	return(0);
}
/*-------------------------RICEVO UDP------------------------------------------------------------------------------------*/


int ricevoudp(int *socketfd,char *buf[MAXSIZE],struct sockaddr_in *Locale,uint16_t *portatcp){
	
    int addr_size;
    int msglen;
   addr_size = sizeof(struct sockaddr_in); /* ricevo nel buffer per controllare che tipo di pacchetto sia */
    Locale->sin_port =htons(*portatcp);
	
    msglen=recvfrom ( *socketfd, buf, sizeof(struct packet), 0, (struct sockaddr*) Locale, (socklen_t *)&addr_size);
if (msglen<0) {
  	  char msgerror[1024];
  	  sprintf(msgerror,"recvfrom() failed [err %d] ", errno);
  	  perror(msgerror); 
	  
  	}
  	if((msglen>0) && (((struct packet*)buf)->b == 'B')){
printf("  ricevuto  pacchetto\"%d\" len %d, from host %s, \n",
              ntohl(((struct packet*)buf)->id), msglen, inet_ntoa(Locale->sin_addr) );
             
            
 }
if((msglen>0) && (((struct icmp*)buf)->i == 'I')){
printf("ricevuto  icmp scartato: \"%u\" id= %d , from host %s, \n",
          ((struct icmp*)buf)->id_packet_lost,ntohl(((struct icmp*)buf)->id),  inet_ntoa(Locale->sin_addr) );
           return(2);  
 }
     
return(0);
}

/*-------------------------ELIMINA PACCHETTO-----------------------------------------------------------------------*/

int eliminapacchetto(char *buf[MAXSIZE],struct packet *nuovo){
	
struct packet *root; /* creo root per tenere in memoria il puntatore precedente a quello controllato x eliminare il pacchetto*/

root=nuovo;
	if(((struct packet*)buf)->b == 'B'){
		while((nuovo->id != ((struct packet*)buf)->id) && (nuovo->next != NULL) ){ /*1° controllo x trovare il pacchetto escludendo i nulli successivi*/
			root=nuovo; /*tengo in memoria il precedente puntato*/
			nuovo=nuovo->next;
			}
			if ((nuovo->id == ((struct packet*)buf)->id) ) { /* se il pacchetto confrontato con quello ricevuto è uguale*/
														      
				 if(nuovo->next == NULL){             /*controllo se il successivo è null*/ 
					root->next=NULL; 					/*il precedente del succ è null*/
					}
			else root->next=nuovo->next;               /* altrimenti collego il precedente col successivo del pacchetto trovato*/
				printf("elimino pacchetto b %d\n",ntohl(nuovo->id));
					
				free(nuovo);         /*rimuovo il pacchetto*/
				nuovo=NULL;
				printf("eliminato pacchetto ricevuto \n");
				}
			 return(0);
		}
return(0);}

/*-------------------------- INIZIO FUNZIONI PROXY RECIVER-----------------------------------------------------------*/
int acceptcpreciver(int *socketfd,struct sockaddr_in *Cli,uint16_t *portatcp){
  int ris;

 memset ( Cli, 0, sizeof(*Cli) );

 Cli->sin_family	 =	AF_INET;
Cli->sin_addr.s_addr  =	htonl(INADDR_ANY);
Cli->sin_port		 =	htons(*portatcp);

  /* connection request */
  printf ("connect()\n");
  ris = connect(*socketfd, (struct sockaddr*) Cli, sizeof(*Cli));
  if (ris == SOCKET_ERROR)  {
    printf ("connect() failed, Err: %d \"%s\"\n",errno,strerror(errno));
    exit(1);
  }
  return(0);
}
/*------------------------------------------------------------------------------------------------_*/
int writetcp(int *socketfd,struct packet *support){

	int n;
		if((support->b)=='S')return(0);      /*ricevo il puntatore ordinato del pacchetto modificando il carattere in S se già spedito al server TCP */
	if( (n=write(*socketfd, &(support->body), support->dim)) >0 ) /*spedisco il body del pacchetto con la dimensione scritta in precendenza nel campo dim*/
    printf("Spedisco al server TCP\n");
  
  if(n<0) {
    char msgerror[1024];
    sprintf(msgerror,"write() failed [err %d] ",errno);
    perror(msgerror);
    fflush(stdout);
    return(1);
  }
  
support->b='S';   /* Spedito quindi modifico il carattere del pacchetto*/
return(0);
}

/*-----------------------SPEDISCO UDP -------------------------------------------------------------------------------*/
int spediscoudpreciver(int *socketud,struct sockaddr_in *Locale,char *p,struct packet *root){
	int addr_size;
	addr_size = sizeof(struct sockaddr_in);
	
Locale->sin_addr.s_addr	=inet_addr(p);
  if((porta2 == 62000)||(porta2==62001)||(porta2=62002))
{
if (porta2>=62002)  porta2=62000;
else{ 
porta2++;
}} 

Locale->sin_port =htons(porta2);
if(root->b != 'B' && root->b!='S' && root->next != NULL)root=root->next;/*caso in cui sia elemento sentinella vado al primo pacchetto*/
if(root != NULL && root->b == 'B'){  /*se il carattere è b mando */
	sendto(*socketud, &root->id, sizeof(struct packet), 0, (struct sockaddr *)Locale, addr_size);
	printf("spedito pacchetto %d sent to (%d:%s)\n",
             ntohl(root->id), ntohs(Locale->sin_port),inet_ntoa(Locale->sin_addr));
   root->b='X'; /*spedito quindi diventa X il carattere così sò di averlo spedito */
}
             
	return(0);
}
/*-------------------------RICEVO UDP---------------------------------------------------------------------------------*/


int ricevoudpreciver(int *socketfd,struct packet *s,struct sockaddr_in *Locale){
int i;
int msglen;	
struct packet *support;    
int addr_size;
addr_size = sizeof(struct sockaddr_in);

support=(struct packet *)malloc(sizeof(struct packet)); /*alloco memoria x il pacchetto da ricevere*/

for(i=0;i<64000;i++)support->body[i]='\0';  /*prima lo inizializzo di zero terminatori*/

  
   msglen=recvfrom ( *socketfd, support, sizeof(struct packet), 0, (struct sockaddr*) Locale, (socklen_t *)&addr_size);

if (msglen<0) {
  	  char msgerror[1024];
  	  sprintf(msgerror,"recvfrom() failed [err %d] ", errno);
  	  perror(msgerror); 
	  return(1);
  	}
  	if((msglen>0) && (support->b == 'B')){
printf("ricevuto pacchetto\"%d\" len %d, from host %s, \n",
              ntohl(support->id), msglen, inet_ntoa(Locale->sin_addr) );
            
 }
if((msglen>0) && (((struct icmp*)support)->i == 'I')){
printf("ricevuto  icmp scartato: \"%u\" id= %d , from host %s, \n",
          ((struct icmp*)support)->id_packet_lost,ntohl(((struct icmp*)support)->id),  inet_ntoa(Locale->sin_addr) );
             
 }
 while(s->next != NULL){     /* metto in coda ricercando l'ultimo pacchetto e inserisco */
		s=s->next;
		}
	s->next=support;
	support->next=NULL;
 

return(0);
}

/*--------------------------ORDINO I PACCHETTI PER LA SPEDIZIONE AL TCP SERVER----------------------------------------*/

int ordinoreciver(struct packet *s,struct packet **elemento){
	
	struct packet *support;
	
support=s->next;								/*punto al successivo della sentinella*/
while(ntohl(support->id) != ordino  ){  	    /*finchè l'elemento non è uguale alla variabile globale ordino che mi funziona da incrementatore*/
if( support->next != NULL)support=support->next;/*vado al successivo se non è nullo*/
else break;										/*altrimenti mi fermo*/
}
if(ntohl(support->id) == ordino){				/*se uguale*/
	*elemento=support;							/*passo il puntatore alla write del Server TCP*/
	ordino++;} 									/*incremento il numero di pacchetto da cercare al prossimo giro*/
else return(1);

return(0);}

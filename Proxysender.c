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
#include "includi.h"

int main(int argc, char *argv[])
{
uint16_t portatc=59000;
uint16_t portaudp=60000;
struct sockaddr_in Local;
struct sockaddr_in Cli;
struct sockaddr_in Locale;
struct packet root; /*sentinella*/
struct packet *temp;
int ris;
int fdmax;
int n;
int er;
int socketfd,newsocketfd;
int socketud;
int flags;
int swich0=0;
int swich1=1;
int k=0;
char *buff[MAXSIZE];
struct timeval tv;
fd_set Rd;
fd_set rd;
char p[100];
   if((argc!=2)) { printf ("necessario IP ritardatore come secondo parametro\n");  exit(1);  }
  else {
   strncpy(p, argv[1], 99);
}

root.id=999; /* inizializzo la sentinella e puntatore a successivo*/
root.next=NULL;
FD_ZERO(&rd);
FD_ZERO(&Rd);
/*inizializzo i due socket con la stessa funzione che switcha tra tcp e udp*/
socketcp(&socketud,&Locale,&portaudp,&swich1);	 /*inizializzo socket udp*/
socketcp(&socketfd,&Local,&portatc,&swich0);  	 /*inizializzo socket tcp*/
acceptcp(&socketfd,&newsocketfd,&Cli);    		 /*passo alla accept tcp*/


/*setto i bit dei socket*/
FD_SET(socketud,&Rd);    
FD_SET(newsocketfd,&Rd);
FD_SET(socketfd,&Rd);
temp=&root;
    
		
		for(k=0;;k++){ /*inizio for della select*/
		rd=Rd;
		if(socketud>newsocketfd && socketud>socketfd)fdmax=socketud;  
		else if(socketud<newsocketfd && socketfd<newsocketfd)fdmax=newsocketfd; 
		else if(socketud<socketfd && socketfd>newsocketfd)fdmax=socketfd; /*controllo quale socket è in ascolto*/ 
	          
ris=select(fdmax+1,&rd,NULL,NULL,&tv);	

if( FD_ISSET(newsocketfd,&rd) ) 
			{
		    n=ricevotcp(&newsocketfd,&root,&socketud,&Locale,p); /*ricevo in tcp memorizzo nei pacchetti e spedisco al Proxyreciver*/
			if(n==2){close(newsocketfd);						 /*se la funzione restituisce 2 la connessione è stata chiusa e chiudo il socket*/
			FD_CLR(newsocketfd,&Rd);}							 /*azzero il bit del socket chiuso*/
		}
tv.tv_usec = 700000;	 											
if( FD_ISSET(socketfd,&rd) ) 
			{
			if(n==2 ){   								/*se la connessione dal client tcp è stata chiusa*/
			spediscoudp(&socketud,&Locale,temp,p);		/* continuo a spedire i pacchetti restanti */
			close(newsocketfd);							/*chiudo il socket tcp */
			FD_CLR(newsocketfd,&Rd);					/*azzero il bit*/
			acceptcp(&socketfd,&newsocketfd,&Cli); 		/*mi rimetto in ascolto da parte del client*/
	        FD_SET(newsocketfd,&Rd);					/*risetto il bit del socket*/
	        ricevotcp(&newsocketfd,&root,&socketud,&Locale,p); /*ritorno alla ricezione dei dati da parte del client*/
		    n=0;
			}}
			
if( FD_ISSET(socketud,&rd) ) 
			{
			
			er=ricevoudp(&socketud,buff,&Locale,&portaudp);  /*ricevo in dietro x essere sicuro che il Proxyreciver abbia ricevuto tutto*/
			temp=&root;								
			if(er!=2)							/* se ho ricevuto il pacchetto di tipo body*/
		    eliminapacchetto(buff,temp);		/* lo cerco e lo elimino */
		    k=0;
		   }
			
if (ris==0 ){									/*fine del timeout rispedisco tutti i pacchetti che non ho ricevuto in dietro */
		 printf("\n         Timeout Ricezione\n");
temp=&root;
if(temp->b!='B' && temp->next != NULL )temp=temp->next;	 /*1° caso in cui mi trovo sull elemento sentinella*/
if(temp->b=='B' ){
spediscoudp(&socketud,&Locale,temp,p);					/*spedisco tutti i pacchetti restanti*/

}}


}
return(0);}



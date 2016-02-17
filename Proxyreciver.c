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
uint16_t portatcp=64000;
uint16_t portaudp=63000;
uint16_t portatp=0;
int newsocketfd,socketud; 
int swich0=0;
int swich1=1;
int k;
int fdmax;
int ris;
struct timeval tv;
struct sockaddr_in Cli,Locale;
struct packet sentinella;
struct packet *root;
struct packet *temp;
fd_set Rd;
fd_set rd;
int l;
char p[100];
  if((argc!=2)) { printf ("necessario IP del ritardatore come secondo parametro\n");  exit(1);  }
  else {
   strncpy(p, argv[1], 99);
}
/*creo la sentinella*/
sentinella.id=999;
sentinella.next=NULL;
temp=&sentinella;
FD_ZERO(&rd);
FD_ZERO(&Rd);
socketcp(&socketud,&Locale,&portaudp,&swich1); /*inizializzo socket udp */
socketcp(&newsocketfd,&Locale,&portatp,&swich0);/*inizializzo socket tcp*/
acceptcpreciver(&newsocketfd,&Cli,&portatcp);   /*avvio la accept tcp x il Server TCP*/
FD_SET(socketud,&Rd);    
fdmax=socketud;  
	
  for(k=0;;k++){
		rd=Rd;

ris=select(fdmax+1,&rd,NULL,NULL,&tv);	
tv.tv_usec = 140000;	                            /*imposto timeout*/
if( FD_ISSET(socketud,&rd) ) 
			{  
				ricevoudpreciver(&socketud,&sentinella,&Locale);  /*ricevo tutti i pacchetti dal Proxysender */
					 

		     }
		 
	

	
if(ris==0 && sentinella.next != NULL){   /*se il timer è scaduto e ho ricevuto almeno un pacchetto */
	 printf("\n         Timeout Ricezione\n");
	 root=&sentinella;
while(root!=NULL)
{
	spediscoudpreciver(&socketud,&Locale,p,root);     /*spedisco in dietro al Proxysender i pacchetti ricevuti*/
  if(root->next != NULL)root=root->next;			/*vado al successivo se non è nullo*/
  else if(root->next == root || root->next == NULL)break;   
}
		
	for(l=0;l<100;l++){            /*ciclo 100 volte x spedire + velocemente i dati al Server TCP in ascolto*/
	
	temp=&sentinella;
	k=ordinoreciver(temp,&elemento);   /*cerco gli elementi in ordine e li passo alla write con il puntatore elemento*/
	if(k!=1)
	writetcp(&newsocketfd,elemento);  /*ho già il pacchetto ordinato mi resta che spedire al Server*/
     }}
	
}/*fine for*/

return(0);}




















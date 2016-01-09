/**client forza 4
 ** Author: Bachechi Andrea
 **/
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"myLib.h"

int main (int argc  ,char*argv[] )
{
  struct sockaddr_in srvr_addr;
  int ret, sk;
  if(argc < 3)
    {
      printf("argomenti mancanti!!");
      return 1;
    }
  else
    {
      ret = init_srvr_addr(&srvr_addr , argv[1], argv[2]);
      if(ret != 1)
	{
	  printf("impossibile inizializzare l'indirizzo del server\r\n");
	  return 1;
	}
      sk = socket(AF_INET , SOCK_STREAM , 0);
      ret = connect(sk,(struct sockaddr *)&srvr_addr , sizeof(srvr_addr));
      if(ret == 0 )
	{
	  printf("connessione con il server %s (%u) effettuata con successo\r\n",argv[1],atoi(argv[2]));
	  char usrName[20];
	  int udpPort;
	  int len;
	  int partita_avviata = 0;
	  char cmnd_string[25];
	  printf("Inserisci il tuo nome (max 20 caratteri):\r\n> ");
	  fgets(cmnd_string,20,stdin);
	  sscanf(cmnd_string,"%19s%n",usrName,&len);
	  printf("Inserisci la porta UDP di ascolto:\r\n> ");
	  fgets(cmnd_string,24,stdin);
	  sscanf(cmnd_string,"%4i",&udpPort);
	  //srvrSend(1 ,usrName ,len ,sk);
	  //send_op(1,sk);
	  // printf("%i\r\n",len);
	  //invio la porta
	  send_len(udpPort,sk);
	  //invio la lunghezza
	  send_len(len,sk);
	  //invio il messaggio
	  send_msg(len,sk,usrName);
	  int loopCond = 0;
	  fd_set fd_master,fd;
	  FD_ZERO(&fd_master);
	  FD_ZERO(&fd);
	  FD_SET(0,&fd_master);
	  FD_SET(sk,&fd_master);
	  helper();
	  while(loopCond == 0)
	     {
	       if(partita_avviata == 0)
		 printf("> ");
	       else
		 printf("# ");
	       fflush(stdout);
	       fd = fd_master;
	       if(select(sk+1,&fd,NULL,NULL,NULL) == -1)
		 {
		   perror("select() error");
		   exit(1);
		 }
	       if(FD_ISSET(sk,&fd))
		 {
		   int len;
		   char nome[20];
		   //chiedere all'utente se vuole accettare la partita
		   recv(sk,,&len,sizeof(int),0);
		   recv(sk,nome,len*sizeof(char),0);
		   printf("\r\n%s ti ha invitato a giocare , vuoi accettare la partita?(y/n)\r\n>");
		   fflush(stdout);
		   int risposta_corretta = 0;
		   char risposta; 
		   while(risposta_corretta == 0)
		     {
		       scanf("%c",risposta);
		       if(risposta == 'y' || risposta == 'Y' || risposta == 'n' || risposta == 'N')
			 risposta_corretta = 1;
		       else
			 {
			   printf("risposta non valida , inserire y se si vuole accettare o n altrimenti \r\n>");
			   fflush("stdout");
			 }
		     }
		   int codifica = codifica_risposta();
		   send(sk,codifica,sizeof(int),0);
		   if(codifica == 1)
		     {
		       int ip ;
		       recv(sk,&ip,sizeof(int),0);
		       
		     }
		 }
	       if(FD_ISSET(0,&fd))
		 {
		   fgets(cmnd_string , 25 , stdin);
		   int action ;
		   char argument[10];
		   action = parse_cmd_strng(cmnd_string);
		   switch (action)
		     {
		     case 1:
		       printf("disconnessione in corso \n\r");
		       loopCond = 1;
		       break;
		       
		     case 2:
		       helper();
		       break;
		    
		     case 3:
		       {
		       //!who
		       int number , len;
		       send_op(3,sk);
		       recv(sk,&number,sizeof(int),0);
		       if(number >0)
			 {
			   printf("sono disponibili %i  utenti:\r\n",number);
			   for(int i = 0;i<= number ;i++)
			     {
			       recv(sk,&len,sizeof(int),0);
			       //printf("len : %i",len);
			       recv(sk,usrName,len*sizeof(char),0);
			       printf("%s\r\n",usrName);
			     }
			 }
		       else
			 {
			   printf("non ci sono altri utenti connessi\r\n");
			 }
		       /** questo fa parte della connect!!!!
		       printf("Inserire il nome dell'utente che intendiamo sfidare\r\n");
		       scanf("%19s%n",userName,len);
		       send_len(len,sk);
		       send_msg(len,sk,userName);**/
		       break;
		       }
		     case 4:
		       {
		       //!connect
		       sscanf(cmnd_string,"*%s%s%n",argument,len);
		       send_op(4,sk);
		       send_len(len,sk);
		       send_msg(len,sk,argument);
		       int risposta ;
		       recv(sk,&risposta,sizeof(int),0);
		       if(risposta == -1)
			 printf("%s nome inesistente\r\n",argument);
		       else if(risposta == -2)
			 printf("%s gia' impegnato in una partita\r\n");
		       else if(risposta == 0)
			 printf("l'utente %s ha rifiutato la partita\r\n",argument);
		       else
			 {
			   printf("l'utente %s ha accettato la partita\r\n");
			   
			 }
		       break;
		       }
		     case 5:
		       break;
		       
		     case 6:
		       break;
		       
		     case 7:
		       break;
		       
		     case 0:
		       printf("comando non valido , digitare !help per visualizzare una lista di comandi validi\r\n");
		       break;
		       
		     default :
		       printf("comando non valido , digitare !help per visualizzare una lista di comandi validi\r\n");
		       break;
		     }
	       // printf("l'utente ha digitato :%s il comando tradotto è: %i \r\n",cmnd_string,action);

		 }
	     }
	}
      else
	{
	  printf("impossibile connettersi al server\r\n");
	  return 1;
	}
      
    }
}

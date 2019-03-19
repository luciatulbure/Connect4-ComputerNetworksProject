#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
//#include <graphics.h>

/* portul folosit */
#define PORT 2907
#define nr_echipe 3
#include<stdlib.h>
#define latime 6
#define lungime 7


/* codul de eroare returnat de anumite apeluri */
extern int errno;
typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

static void treat(void *); / functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int diagonala(char joc[10][10], int linie, int coloana)
{
	int ok = 0;
	if (joc[linie][coloana] == 'N')return 0;
	if (linie+3<=latime && coloana >3) if (joc[linie][coloana] == joc[linie + 1][coloana - 1] && joc[linie][coloana] == joc[linie + 2][coloana - 2] && joc[linie][coloana] == joc[linie + 3][coloana - 3])ok = 1;
	if (linie+3<=latime && coloana + 3<=lungime) if (joc[linie][coloana] == joc[linie + 1][coloana + 1] && joc[linie][coloana] == joc[linie + 2][coloana + 2] && joc[linie][coloana] == joc[linie + 3][coloana + 3])ok = 1;
	return ok;
}
int orizontala(char joc[10][10], int linie, int coloana)
{
	int ok = 0;
	if (joc[linie][coloana] == 'N')return 0;
	if ( coloana-3 >0) if (joc[linie][coloana] == joc[linie ][coloana - 1] && joc[linie][coloana] == joc[linie ][coloana-2] && joc[linie][coloana] == joc[linie ][coloana-3 ])ok = 1;
	return ok;
}
int verticala(char joc[10][10], int linie, int coloana)
{
	int ok = 0;
	if (joc[linie][coloana] == 'N')return 0;
	if (linie +3 <=latime) if (joc[linie][coloana] == joc[linie+1][coloana ] && joc[linie][coloana] == joc[linie+2][coloana ] && joc[linie][coloana] == joc[linie+3][coloana ])ok = 1;
	return ok;
}

int main ()
{
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  int a,b;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i=0;
  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  /* servim in mod concurent clientii...folosind thread-uri */
	
  while (1)
    {
      int client;
      thData * td; //parametru functia executata de thread     
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */

      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0 || i/2>= nr_echipe )
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
	

        /* s-a realizat conexiunea, se astepta mesajul */
    
	 //int idThread; //id-ul threadului
	 //int cl; //descriptorul intors de accept
	
	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;
	
	//printf("\nadresa e %d\n",client); 
	if((i-1)%2==1 && (i-1)/2< nr_echipe) {pthread_create(&th[i], NULL, &treat, td); printf("S-a conectat echipa cu numarul : %d \n", i/2); }	      
	
	}//while    
};				
static void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= ((struct thData)arg);			 
		pthread_detach(pthread_self());		
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
};


void raspunde(void *arg)
{
	struct thData tdL; 
	tdL= ((struct thData)arg);
	int linie,coloana,a,b;
	int inca_un_joc;
	int i,col,ok,jucator=0;int ii,jj;int pierdut=-1;
	int nr_echipa=((tdL.idThread-1)/2)+1;
	char  joc[10][10];
	char* culoare;
	culoare=(char*)malloc(2); culoare[0]='R'; culoare[1]='G';
	char scor[5];
	scor[0]='0';scor[1]='0';
	//initializam tabla de joc
	for (a = 1; a <= latime; a++)
	{
		for (b = 1; b <= lungime; b++)
		{
			joc[a][b] = '*';
		}
	}
	joc[1][1]='F';joc[1][2]='E';
	while (pierdut==-1)
	{	//transmitem tabla jucatorului
		
		 if (write (tdL.cl-jucator, &joc,100* sizeof(char)) <= 0)
		{
		 perror ("[Thread]Eroare la write() catre client.\n");
		}	
	 	if(joc[1][1]=='F')joc[1][1]='*';
		else if(joc[1][2]=='E') joc[1][2]='*';
	
		//citim coloana introdusa de client
		read(tdL.cl-jucator,&col,sizeof(int));
		
		//verificam daca mutarea e castigatoare
		for (i = latime; i >= 1; i--)
		{

			if (joc[i][col ] == '*') { joc[i][col ] = culoare[jucator];  break; }
			
			
		}
		if (diagonala(joc, i, col)) { pierdut = 0; }
		if (verticala(joc, i , col)){ pierdut = 0; }
		if (orizontala(joc, i, col)){pierdut = 0; }
		//mutarea e castigatoare
		if(pierdut==0){ if(jucator==0){joc[1][1]='A'; scor[0]++; }
				else {scor[1]++;joc[1][1]='B';}
				if (write (tdL.cl-jucator, &joc,100* sizeof(char)) <= 0)
				{
					 perror ("[Thread]Eroare la write() catre client.\n");
				}
				//verificam daca mai doreste un joc
				read(tdL.cl-jucator,&inca_un_joc,sizeof(int));
				//daca da
				if(inca_un_joc==1){
					for (a = 1; a <= latime; a++)
					{
						for (b = 1; b <= lungime; b++)
							{
								joc[a][b] = '*';
							}
					}
					pierdut=-1; 
							}//if inca un joc
				else { joc[1][1]=scor[0]; joc[1][2]=scor[1];
					if (write (tdL.cl-jucator,&joc,100* sizeof(char)) <= 0)
					{
					 perror ("[Thread]Eroare la write() catre client.\n");
					}jucator=!jucator;
				      if (write (tdL.cl-jucator, &joc,100* sizeof(char)) <= 0)
					{
					 perror ("[Thread]Eroare la write() catre client.\n");
					}
				}

				}//if
		else {  if (write (tdL.cl-jucator, &joc,100* sizeof(char)) <= 0)
			{
		 		perror ("[Thread]Eroare la write() catre client.\n");
			}
	 	     }
		jucator = !jucator;//randul celuilat jucator
	}//while
	
}//functie
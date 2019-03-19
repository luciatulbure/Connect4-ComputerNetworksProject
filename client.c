#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include<ctype.h>
#define lungime 7
#define latime 6
#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"
#define BLU   "\x1B[34m"

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;
int verif=1;
int ceva=0;
void afisare_scor(char a, char b){
if(a>b){printf("\nScorul este : %c - %c pentru jucatorul cu culoarea rosie\n",a,b);}
		    else if(a==b){printf("\nEgalitate  %c - %c\n",a,b);}
		    	else {printf("\nScorul este : %c - %c pentru jucatorul cu culoarea galbena\n",a,b);}
}
void afisare_tabla(char buf[10][10])
{
int i,j;
printf(BLU"   _      _      _      _      _      _      __   "RESET);printf("\n\n");
	for(i=1;i<=latime && verif==1 ;i++)
		{for(j=1;j<=lungime && verif==1;j++)
			{if(buf[i][j]=='*')printf(BLU" |    " RESET BLU"| "RESET);
			if(buf[i][j]=='R')printf(BLU " ┃" RED " ⬤  " BLU "┃ " RESET);
			if(buf[i][j]=='G')printf(BLU " ┃" YEL " ⬤  " BLU "┃ " RESET);}
			
	printf("\n");		
	printf(BLU"   _      _      _      _      _      _      __   "RESET);printf("\n\n");
		}
}


int main (int argc, char *argv[])
{//printf("\n%d\n",ascii( ⬤  ));
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  		// mesajul trimis
  int nr=0;int i,j,poz,verif=1;
  char buf[10][10];int rasp=-1;int score[5];

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

while(verif==1)
{	//citim tabla
	
	//printf("buf: %d\n",sd);
	bzero(buf,100);

	if (read (sd, buf,sizeof(char)*100) < 0)
   	 {
      		perror ("[client]Eroare la read() de la server.\n");
      		return errno;
   	 }
	if(isdigit(buf[1][1])!=0 && isdigit(buf[1][2])!=0){printf("Ati pierdut\n");afisare_scor(buf[1][1],buf[1][2]);verif=0;break;}
	
	//afisam tabla jucatorului
	system("clear");	
	if(buf[1][1]=='F'){printf("Sunteti primul care face mutarea si aveti culoarea rosie.\n");buf[1][1]='';buf[1][2]='';}
	else if(buf[1][2]=='E'){printf("Sunteti al doilea care face mutarea si aveti culoarea galbena.\n");buf[1][2]='*';}
	printf("Tabla jocului la momentul actual este aceasta:\n");
	afisare_tabla(buf);
	//citim coloana si o transmitem serverului
     
    printf("In ce coloana vreti sa introduceti piesa?(un numar de la 1 la 7) \n");
    scanf("%d",&poz);
	while(poz<1 || poz>7 || buf[1][poz]!='*')
		{ printf("Coloana nu este valida! In ce coloana vreti sa introduceti piesa?(un numar de la 1 la 7) \n");
 		scanf("%d",&poz);
		}
	printf("Asteptati mutarea urmatorului jucator\n");
    	write( sd, &poz, sizeof(int)); 
	//vedem daca e castigatoare
	if (read (sd, buf,sizeof(char)*100) < 0)
    	{
     	   perror ("[client]Eroare la read() de la server.\n");
    	   return errno;
   	}
	//daca da	
	if(buf[1][1]=='A'){system("clear");afisare_tabla(buf);
		printf("\n A castigat jucatorul cu culoarea rosie! Mai doriti sa jucati un joc? Apasati 1 pentru da si 0 pentru nu. \n");
			   scanf("%d",&rasp); 
			   write( sd, &rasp, sizeof(int));
			   }
	if(buf[1][1]=='B'){system("clear");afisare_tabla(buf);
		printf("\n A castigat jucatorul cu culoarea galbena! Mai doriti sa jucati un joc? Apasati 1 pentru da si 0 pentru nu. \n");
			   scanf("%d",&rasp); 
			   write( sd, &rasp, sizeof(int)); 
			   }			   
	if(rasp==0){if (read (sd, buf,sizeof(char)*100) < 0)
   	 {
      		perror ("[client] Eroare la read() de la server.\n");
      		return errno;
   	 }
		   	afisare_scor(buf[1][1],buf[1][2]);	
  		    //close (sd);
 verif=0;}
	if(rasp==1)printf("Asteptati mutarea urmatorului jucator\n");
}//while
 	 
}
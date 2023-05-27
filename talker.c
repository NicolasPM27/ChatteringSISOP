/* 
Autor: M. Curiel
funcion: Ilustra la creaci'on de pipe nominales y el uso de señales 
Nota: No todas las llamadas al sistema  estàn validadas. Deben validarlas siempre en su proyecto y cuando usen este còdigo. 
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "nom.h"

#define TAMMENSAJE 10
#define TAMNOMBRE 10

int fd1;
char mensaje[TAMMENSAJE];
typedef void (*sighandler_t)(int);

sighandler_t signalHandler (void)
{

   // Este read debe ir en un manejador de señales. En su lugar el cliente va a esperar en un pause 
   printf("Desde el manejador:\n");
   read(fd1,mensaje,TAMMENSAJE);
   printf("El proceso cliente termina y lee %s \n", mensaje);
  
}



int main (int argc, char **argv)
{

  int  fd, pid, creado = 0, res;
  datap datos;
  char nombre[TAMNOMBRE];
  int banid=0, banpipe=0;
   
  mode_t fifo_mode = S_IRUSR | S_IWUSR;
    signal (SIGUSR1,(sighandler_t) signalHandler);

    if(argc!=5){
      printf("\nEstructura de la instrucción:  %s -i IDTalker -p  NombrePipe\n",argv[0]);
		exit(1);
    }
    for(int i=1;i<argc -1;i++){
      if(strcmp(argv[i], "-i")==0){
        printf("%d",atoi(argv[i+1]));
         if(atoi(argv[i+1])<=0){
            printf("\nEl ID del Talker debe ser un número positivo\n");
            exit(1);
      }
      banid=1;
          /*
      Asignación de id
      */
   }else if(strcmp(argv[i], "-p")==0){
      if(strcmp(argv[i+1], "pipem")!=0){
         printf("\nEl nombre del pipe debe ser el mismo del manager\n");
         exit(1);
      }
      banpipe=1;
         /*
         Asignación de nombre pipe
         */
    }
    }
    if(banid==0 || banpipe==0){
      printf("\nEstructura de la instrucción:  $./%s -i IDTalker -p  NombrePipe\n",argv[0]);
      exit(1);
    }
  // Se abre el pipe cuyo nombre se recibe como argumento del main. 
  do { 
     fd = open(argv[1], O_WRONLY);
     if (fd == -1) {
         perror("pipe");
         printf(" Se volvera a intentar despues\n");
	 sleep(10);        
     } else creado = 1;
  } while (creado == 0);


  // Se va crear una estructura con el PID del proceso y el nombre del segundo pipe. El nombre llevarà el pid del proceso
  
  pid = getpid();
  datos.pid = pid;
  
  // Nombre de un segundo pipe
  sprintf(nombre, "proceso-%d", (int) getpid());
 
  strcpy(datos.segundopipe, nombre);
 
  // Se crea un segundo pipe para la comunicacion con el server.
  unlink(nombre); 
  if (mkfifo (datos.segundopipe, fifo_mode) == -1) {
     perror("Client mkfifo:");
     exit(1);
  }
  // se envia el nombre del pipe al otro proceso. 
   write(fd, &datos , sizeof(datos));

   // Se abre el segundo pipe
   
     if ((fd1 = open(datos.segundopipe, O_RDONLY)) == -1) {
        perror(" Cliente  Abriendo el segundo pipe: ");
        exit(1);
     }  
   
 
   // Se lee un mensaje por el segundo pipe. 
  pause();

 
  exit(0);
    
  
}


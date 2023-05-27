/* 
Autor: M. Curiel
funcion: Ilustra la creaci'on de pipe nominales. 
Nota: todas las llamadas al sistema no estan validadas. Siempre que puedan retornar error deben validarse
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "nom.h"


#define TAMNOM 20

int main (int argc, char **argv)
{
  int  fd, fd1,  pid, n, cuantos,res,creado=0;
  datap datos; 
  int bann=0, banpipe=0; 
   mode_t fifo_mode = S_IRUSR | S_IWUSR;
      if(argc!=5){
      printf("\nEstructura de la instrucción:  %s -n NumMaxUsuarios -p  NombrePipe\n",argv[0]);
		exit(1);
    }
    for(int i=1;i<argc -1;i++){
      if(strcmp(argv[i], "-n")==0){
         if(atoi(argv[i+1])<=0){
            printf("\nEl número de usuarios debe ser mayor a cero\n");
            exit(1);
      }
      bann=1;
          /*
      Asignación de id
      */
   }else if(strcmp(argv[i], "-p")==0){
      if(strcmp(argv[i+1], "")==0){
         printf("\nEl nombre del pipe no puede ser vacío\n");
         exit(1);
      }
      banpipe=1;
         /*
         Asignación de nombre pipe
         */
    }
    }
    if(bann==0 || banpipe==0){
      printf("\nEstructura de la instrucción:  $./%s -n numMaxUsuarios -p  NombrePipe\n",argv[0]);
      exit(1);
    }
  // Creacion del pipe inicial, el que se recibe como argumento del main
  unlink(argv[1]);
	      
  if (mkfifo (argv[1], fifo_mode) == -1) {
     perror("Server mkfifo:");
     exit(1);
  }
   while(1){
  // Abre el pipe. 
  if ((fd = open (argv[1], O_RDONLY)) == -1) {
        perror(" Servidor abriendo el pipe: ");
        exit(1);

  }
  // El otro proceso (cliente) le envia el nombre para el nuevo pipe y el pid. 
  cuantos = read (fd, &datos, sizeof(datos));
  if (cuantos == -1) {
     perror("proceso lector:");
     exit(1);
  }
       
   printf ("Server lee el string %s\n", datos.segundopipe);
   printf ("Server el pid %d\n", datos.pid );
   
   }


   do { 
      if ((fd1 = open(datos.segundopipe, O_WRONLY)) == -1) {
         perror(" Server Abriendo el segundo pipe ");
         printf(" Se volvera a intentar despues\n");
         sleep(5); //los unicos sleeps que deben colocar son los que van en los ciclos para abrir los pipes que han creado o deben crear otros procesos         
      } else creado = 1; 
   }  while (creado == 0);


    // Se escribe un mensaje para el  proceso (client)
   
   write(fd1, "hola", 5);
   sleep(2);
   kill(datos.pid, SIGUSR1);

      
   exit(0);

}






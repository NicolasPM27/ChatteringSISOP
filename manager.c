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

int main(int argc, char **argv)
{
   int fd, fd1, pid, n, cuantos, res, creado = 0;
   dataman datosMan;
   datatalk datosTalk;
   int bann = 0, banpipe = 0;
   mode_t fifo_mode = S_IRUSR | S_IWUSR;

   //-------------------------------------------------Validación de argumentos---------------------------------------------------------
   if (argc != 5)
   {
      printf("\nEstructura de la instrucción:  %s -n NumMaxUsuarios -p  NombrePipe\n", argv[0]);
      exit(1);
   }
   for (int i = 1; i < argc - 1; i++)
   {
      if (strcmp(argv[i], "-n") == 0)
      {
         if (atoi(argv[i + 1]) <= 0)
         {
            printf("\nEl número de usuarios debe ser mayor a cero\n");
            exit(1);
         }
         datosMan.numMaxUsuarios = atoi(argv[i + 1]);
         bann = 1;
      }
      else if (strcmp(argv[i], "-p") == 0)
      {
         if (strcmp(argv[i + 1], "") == 0)
         {
            printf("\nEl nombre del pipe no puede ser vacío\n");
            exit(1);
         }
         else if (strlen(argv[i + 1]) > TAMNOM)
         {
            printf("\nEl nombre del pipe no puede ser mayor a %d caracteres\n", TAMNOM);
            exit(1);
         }
         strcpy(datosMan.nombrePipeInicial, argv[i + 1]);
         banpipe = 1;
      }
   }
   if (bann == 0 || banpipe == 0)
   {
      printf("\nEstructura de la instrucción:  $./%s -n numMaxUsuarios -p  NombrePipe\n", argv[0]);
      exit(1);
   }
   //-------------------------------------------------------------Inicio del programa----------------------------------------------
   
   // Creacion del pipe inicial, el que se recibe como argumento del main
   unlink(datosMan.nombrePipeInicial);
   if (mkfifo(datosMan.nombrePipeInicial, fifo_mode) == -1)
   {
      perror("Error creando el pipe por parte del manager");
      exit(1);
   }
   printf("Creación del pipe '%s' exitosa con máximo %d Talkers\n", datosMan.nombrePipeInicial, datosMan.numMaxUsuarios);

   //Ciclo infinito para esperar a que se conecten los Talkers
   while(1){
   printf("Manager esperando a que se conecte un Talker\n");
   // Se abre el pipe
   if((fd=open(datosMan.nombrePipeInicial,O_RDONLY))==-1){
      perror("Error del manager al abrir el pipe: ");
      exit(1);
   }
   cuantos=read(fd,&datosTalk,sizeof(datosTalk));
   if(cuantos==-1){
      perror("Error del manager leyendo el pipe: ");
      exit(1);
   }
      if(datosTalk.idTalker>datosMan.numMaxUsuarios){
         printf("ID de usuario superior al máximo\n");
         
      }
      //Se abre el segundo pipe
      fd1=open("o",O_WRONLY);
      if(fd1==-1){
         perror("Error abriendo el segundo pipe por parte del Manager: ");
         exit(1);
      } 
      write(fd1,&datosMan,sizeof(datosMan));
   }
}

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
   int fd_t, fd_m, pid, n, cuantos, res, creado = 0;
   dataman datosMan;
   datatalk datosTalk;
   int bann = 0, banpipe = 0;
   mode_t fifo_mode = S_IRUSR | S_IWUSR;
   const char *pipet_m; // Variables que almacenan el nombre de los pipes a utilizar (Unicamente para facilitar la lectura del código)
   const char *pipem_t;

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
   //Inicializar variables de pipes
   pipet_m = datosMan.nombrePipeInicial;
   pipem_t = PIPE2;
   
   // Creacion del pipe inicial, el que se recibe como argumento del main
   unlink(pipet_m);
   if(mkfifo(pipet_m, fifo_mode) == -1){
      perror("Error creando el pipe Talker->Manager: ");
      exit(1);
   }
   // Creacion del pipe secundario, para enviar datos del manager al talker  
   unlink(pipem_t);
   if (mkfifo(pipem_t, fifo_mode) == -1)
   {
      perror("Error creando el pipe por parte del manager");
      exit(1);
   }

   printf("Manager iniciado y el sistema podrá tener como máximo %d usuarios\n",datosMan.numMaxUsuarios);

   //*************COMUNICACIÓN TALKER->MANAGER*************
   while(1){   //Ciclo infinito para esperar a que se conecten los Talkers
    printf("Manager esperando a que se conecte un Talker\n");
   //Abrir el pipe para comunicación talker->manager  
   fd_t=open(pipet_m,O_RDONLY);
   if(fd_t==-1){
      perror("Error abriendo el pipe Talker->Manager: ");
      exit(1);
   }
   //Leer la estructura del talker
   cuantos=read(fd_t,&datosTalk,sizeof(datosTalk));
   if(cuantos==-1){
      perror("Error leyendo información enviada por el Talker: ");
      exit(1);
   }
   //*************FIN COMUNICACIÓN TALKER->MANAGER*************
   
   //*************COMUNICACIÓN MANAGER->TALKER*****************

   //Abrir el pipe para comunicación manager->talker
   fd_m=open(pipem_t,O_WRONLY);
   if(fd_m==-1){
      perror("Error abriendo el pipe Manager->Talker: ");
      exit(1);
   }
   //Enviar la estructura del manager
   write(fd_m,&datosMan,sizeof(datosMan));
   //*************FIN COMUNICACIÓN MANAGER->TALKER*************
   
   //Validación para registro Talker
      if(datosMan.numMaxUsuarios<datosTalk.idTalker){
         printf("ID de usuario superior al máximo\n");
         close(fd_t);
         close(fd_m);
      }else {
         printf("Talker (%d) registrado\n",datosTalk.idTalker);
      }

   
   }
   close(fd_m);
   close(fd_t);
   unlink(pipem_t);
   unlink(pipet_m);
}

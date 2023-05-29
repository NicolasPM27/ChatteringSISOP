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

int fd_m;
char mensaje[TAMMENSAJE];
typedef void (*sighandler_t)(int);

sighandler_t signalHandler(void)
{

   // Este read debe ir en un manejador de señales. En su lugar el cliente va a esperar en un pause
   printf("Desde el manejador:\n");
   read(fd_m, mensaje, TAMMENSAJE);
   printf("El proceso cliente termina y lee %s \n", mensaje);
}

int main(int argc, char **argv)
{
   // Se declaran las variables necesarias para la comunicacion con el manager
   int fd_t, pid, creado = 0, res, cuantos;
   dataman datosMan;
   datatalk datosTalk;
   char nombre[TAMNOMBRE];
   int banid = 0, banpipe = 0;
   mode_t fifo_mode = S_IRUSR | S_IWUSR;
   const char *pipet_m; // Variables que almacenan el nombre de los pipes a utilizar (Unicamente para facilitar la lectura del código)
   const char *pipem_t;
   signal(SIGUSR1, (sighandler_t)signalHandler);
   //-------------------------------------------------Validación de argumentos---------------------------------------------------------
   // Se valida que el numero de argumentos sea el correcto
   if (argc != 5)
   {
      printf("\nEstructura de la instrucción:  %s -i IDTalker -p  NombrePipe\n", argv[0]);
      exit(1);
   }
   for (int i = 1; i < argc - 1; i++)
   {
      if (strcmp(argv[i], "-i") == 0)
      {
         if (atoi(argv[i + 1]) <= 0)
         {
            printf("\nEl ID del Talker debe ser un número positivo\n");
            exit(1);
         }
         datosTalk.idTalker = atoi(argv[i + 1]);
         banid = 1;
      }
      else if (strcmp(argv[i], "-p") == 0)
      {
         strcpy(datosTalk.nombrePipeInicial, argv[i + 1]);
         banpipe = 1;
      }
   }
   if (banid == 0 || banpipe == 0)
   {
      printf("\nEstructura de la instrucción:  $./%s -i IDTalker -p  NombrePipe\n", argv[0]);
      exit(1);
   }
   //-------------------------------------------------Inicio del programa---------------------------------------------------------
   // Inicializar variables de pipes
   pipet_m = datosTalk.nombrePipeInicial;
   pipem_t = PIPE2;

   //*****************COMUNICACIÓN TALKER->MANAGER*****************
   // Abrir el pipe para la comunicación Talker->Manager
   fd_t = open(pipet_m, O_WRONLY);
   if (fd_t == -1)
   {
      perror("Error abriendo pipe Talker->Manager: ");
      exit(1);
   }
   // Enviar la estructura al manager
   write(fd_t, &datosTalk, sizeof(datosTalk)); // Se envia el id del talker al manager

   //*****************FIN COMUNICACIÓN TALKER->MANAGER*****************

   //*****************COMUNICACIÓN MANAGER->TALKER*****************
   // Abrir el pipe para la comunicación Manager->Talker
   fd_m = open(pipem_t, O_RDONLY);
   if (fd_m == -1)
   {
      perror("Error abriendo pipe Manager->Talker: ");
      exit(1);
   }
   // Leer la estructura del manager
   cuantos = read(fd_m, &datosMan, sizeof(datosMan));
   if (cuantos == -1)
   {
      perror("Error leyendo información enviada por el Manager: ");
      exit(1);
   }

   if (datosMan.numMaxUsuarios < datosTalk.idTalker)
   {
      printf("El ID del talker supera el número máximo de usuarios determinado por el manager, no se puede inicializar\n");
      close(fd_t);
      exit(1);
   }
   else
   {
      printf("Comunicación creada con éxito");
      printf("---MENÚ----");
   }
   exit(0);
}

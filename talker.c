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

sighandler_t signalHandler(void)
{

   // Este read debe ir en un manejador de señales. En su lugar el cliente va a esperar en un pause
   printf("Desde el manejador:\n");
   read(fd1, mensaje, TAMMENSAJE);
   printf("El proceso cliente termina y lee %s \n", mensaje);
}

int main(int argc, char **argv)
{
   // Se declaran las variables necesarias para la comunicacion con el manager
   int fd, pid, creado = 0, res, cuantos, opcion = 0;
   bool conectado = false;
   dataman datosMan;
   datatalk datosTalk;
   char nombre[TAMNOMBRE];
   int banid = 0, banpipe = 0;
   mode_t fifo_mode = S_IRUSR | S_IWUSR;
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
   // Se abre el pipe cuyo nombre se recibe como argumento del main.
   fd = open(datosTalk.nombrePipeInicial, O_WRONLY);
   if (fd == -1)
   {
      perror("Error abriendo pipe por parte del Talker");
      exit(1);
   }
   else
   {
      pid = getpid();
      datosTalk.pidTalker = pid;
      datosTalk.conectado = conectado;
      datosTalk.opcion = opcion;
      printf("El pipe %s ha sido creado\n", datosMan.nombrePipeInicial);
   }
   // Crear segundo pipe para recibir la respuesta del servidor
   unlink("o");
   if (mkfifo("o", fifo_mode) == -1)
   {
      perror("Error creando pipe por parte del Talker");
      exit(1);
   }
   if(write(fd, &datosTalk, sizeof(datosTalk)) == -1)
   {
      perror("Error escribiendo en el pipe por parte del Talker");
      exit(1);
   }
   printf("El proceso Talker envia el ID del talker %d y el nombre del pipe %s\n", datosTalk.idTalker, datosTalk.nombrePipeInicial);
   if ((fd1 = open("o", O_RDONLY)) == -1)
   {
      perror("Error abriendo pipe por parte del Talker");
      exit(1);
   }
   cuantos = read(fd1, &datosMan, sizeof(datosMan));
   if (cuantos == -1)
   {
      perror("Error leyendo pipe por parte del Talker");
      exit(1);
   }
   if (datosMan.numMaxUsuarios < datosTalk.idTalker)
   {
      printf("El ID del talker supera el número máximo de usuarios determinado por el manager\n");
      unlink("o");
      exit(1);
   }
   else
   {
      do
      {
         // menu
         printf("1. List\n");
         printf("2. List GID\n");
         printf("3. Group ID1, ID2, ID3, ... IDN\n");
         printf("4. Sent msg IDi\n");
         printf("5. Sent msg GroupID\n");
         printf("6. Salir\n");
         scanf("%d", &opcion);
         switch (opcion)
         {

         case 1:
            datosTalk.opcion = opcion;
            if(write(fd, &datosTalk, sizeof(datosTalk)) == -1)
            {
               perror("Error escribiendo en el pipe por parte del Talker");
               exit(1);
            }
            break;
         }
         

      } while (opcion != 6);
   }
}

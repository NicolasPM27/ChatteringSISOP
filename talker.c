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

#define TAMNOMBRE 10

int fd_m;
char mensaje[TAMMENSAJE];
typedef void (*sighandler_t)(int);

sighandler_t signalHandler(void)
{

   // Este read debe ir en un manejador de señales. En su lugar el cliente va a esperar en un pause
   read(fd_m, mensaje, sizeof(mensaje));
   printf("%s \n", mensaje);
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
   char input[NUMMAX];
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
   datosTalk.pid = getpid();
   datosTalk.opcion = 0;
   write(fd_t, &datosTalk, sizeof(datosTalk)); // Se envia el id del talker al manager

   //*****************FIN COMUNICACIÓN TALKER->MANAGER*****************

   //*****************COMUNICACIÓN MANAGER->TALKER*****************
   // Abrir el pipe para la comunicación Manager->Talker
   sleep(1);
   char pipe_pers[20];
   sprintf(pipe_pers, "pipe%d", getpid());
   fd_m = open(pipe_pers, O_RDONLY);
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
   else if (datosMan.estaregistrado == 1)
   {
      printf("El ID del talker ya está registrado, no se puede inicializar\n");
      close(fd_t);
      exit(1);
   }
   else
   {
      printf("Comunicación creada con éxito\n");
      // Menú
      do
      {
         printf("1. List\n");
         printf("2. List GID\n");
         printf("3. Group ID1, ID2, ID3, ... IDN\n");
         printf("4. Sent msg IDi\n");
         printf("5. Sent msg GroupID\n");
         printf("6. Salir\n");
         // scanf("%[^\n]", input);
         fgets(input, sizeof(input), stdin);
         // Tokenizar
         char *tokens[NUMMAX];
         memset(tokens, '\0', sizeof(tokens));
         char *token = strtok(input, " ");
         // Hacer un arreglo con los tokens
         for (int i = 0; token != NULL && i < NUMMAX; i++)
         {
            tokens[i] = token;
            token = strtok(NULL, " ");
         }
         if (input[strlen(input) - 1] == '\n')
         {
            input[strlen(input) - 1] = '\0';
         }

         // Validar la opción

         if (strcmp(tokens[0], "List") == 0 && tokens[1] == NULL) // Opcion 1
         {                                                        // Listar todos los usuarios
            datosTalk.opcion = 1;
            printf("Lista de usuarios conectados: \n");
            for (int i = 0; i < datosMan.numMaxUsuarios; i++)
            {
               if (datosMan.registrados[i] == 1)
               {
                  printf("\nID: %d, ", i);
               }
            }
         }
         else if (strcmp(tokens[0], "List") == 0 && tokens[1] != NULL) // Opcion 2
         {                                                             // Listar usuarios de un grupo
            datosTalk.opcion = 2;
            datosTalk.grupoAListar = atoi(tokens[1]);
         }
         else if (strcmp(tokens[0], "Group") == 0 && tokens[1] != NULL) // Opción 3 crear grupo
         {                                                              // Crear un grupo
            datosTalk.opcion = 3;
            datosTalk.idsgrupos[0] = datosTalk.idTalker;
            datosTalk.numintegrantes = 1;
            for (int i = 1; i < NUMMAX && tokens[i] != NULL && tokens[i][0] != '\0'; i++)
            {
               if (tokens[i] != NULL && tokens[i][0] != '\0' && strcmp(tokens[i], " ") != 0)
               {
                  datosTalk.idsgrupos[i] = atoi(tokens[i]);
                  datosTalk.numintegrantes++;
               }
            }
         }
         else if (strcmp(tokens[0], "Sent") == 0 && tokens[1] != NULL && tokens[2] != NULL) // Opcion 4 y 5
         {                                                                                  // Enviar mensaje a un usuario o grupo
            if (strncmp(tokens[2], "G", 1) == 0)
            {
               datosTalk.opcion = 5;
               char numero[3];
               strncpy(numero, tokens[2] + 1, 2);
               numero[2] = '\0';
               int idDest = atoi(numero);
               datosTalk.idDestino = idDest;
               strcpy(datosTalk.mensaje, tokens[1]);
            }
            else
            {
               datosTalk.opcion = 4;
               datosTalk.idDestino = atoi(tokens[2]);
               strcpy(datosTalk.mensaje, tokens[1]);
            }
         }
         else if (strcmp(tokens[0], "Salir") == 0) // Opcion 6
         {                                         // Salir
            datosTalk.opcion = 6;
         }
         else
         {
            printf("Opción no válida\n");
         }
         //**********************************-Se envían los datos del datatalker y se reciben los datos del manager-********************************************
         // Enviar opción
         if (write(fd_t, &datosTalk, sizeof(datosTalk)) == -1)
         {
            perror("Error enviando datos: ");
            exit(1);
         }
         else
         {
            printf("Opcion %d enviada\n", datosTalk.opcion);
            // Recibir datos
            if (datosTalk.opcion == 1)
            {
               cuantos = read(fd_m, &datosMan, sizeof(datosMan));
               if (cuantos == -1)
               {
                  perror("Error leyendo información enviada por el Manager: ");
                  exit(1);
               }
            }
            else if (datosTalk.opcion == 2 || datosTalk.opcion == 3)
            {
               //Espera ocupada
               while (datosMan.grupocreado != -1)
               {
                  cuantos = read(fd_m, &datosMan, sizeof(datosMan));
               }
               if (datosTalk.opcion == 2)
               {
                  if (datosMan.grupocreado == 0)
                  {
                     printf("No existe el grupo G%d", datosTalk.grupoAListar + 1);
                  }
                  else
                     printf("Grupo G%d:\n", datosTalk.grupoAListar + 1);
                  for (int i = 0; i < datosTalk.numintegrantes; i++)
                  {
                     printf("%d,", datosMan.usuariosXGrupo[i]);
                  }
                  printf("\n");
               }
               else if (datosTalk.opcion == 3)
               {
                  if (datosMan.grupocreado == 0)
                     printf("No se pudo crear el grupo\n");
                  else
                     printf("Grupo creado con exito\n");
               }
            }
            else if (datosTalk.opcion == 3)
            {
            }
            else if (datosTalk.opcion == 4)
            {
            }
            else if (datosTalk.opcion == 5)
            {
            }
            else if (datosTalk.opcion == 4)
            {
               if (datosMan.mensajeenviado == 0)
                  printf("No se pudo enviar el mensaje\n");
               else
                  printf("Mensaje enviado con exito\n");
            }
            else if (datosTalk.opcion == 5)
            {
               if (datosMan.mensajeenviadogrupo == 0)
                  printf("No se pudo enviar el mensaje al grupo\n");
               else
                  printf("Mensaje enviado con exito\n");
            }
         }
      }while (datosTalk.opcion != 6);
      exit(0);
   }
}


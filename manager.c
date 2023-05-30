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
   int fd_t, fd_m[NUMMAX], cuantos;
   dataman datosMan;//Estructura que almacena los datos del manager
   datatalk datosTalk;//Estructura que almacena los datos del talker
   int bann = 0, banpipe = 0;//Variables que indican si se ingresaron los argumentos -n y -p respectivamente
   mode_t fifo_mode = S_IRUSR | S_IWUSR;
   const char *pipet_m; // Variable que almacena el nombre del pipes a utilizar (Unicamente para facilitar la lectura del código)
   groups grupos[NUMMAX];//Arreglo de estructuras que almacena los datos de los grupos
   int numg = 0;//Variable que almacena el número de grupos creados

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
   // Inicializar variables de pipes
   pipet_m = datosMan.nombrePipeInicial; 

   // Creacion del pipe inicial, el que se recibe como argumento del main
   unlink(pipet_m);
   if (mkfifo(pipet_m, fifo_mode) == -1)
   {
      perror("Error creando el pipe Talker->Manager: ");
      exit(1);
   }
   // Abrir el pipe para comunicación talker->manager
   printf("Manager iniciado y el sistema podrá tener como máximo %d usuarios\n", datosMan.numMaxUsuarios);
   fd_t = open(pipet_m, O_RDONLY);
   if (fd_t == -1)
   {
      perror("Error abriendo el pipe Talker->Manager: ");
      exit(1);
   }
   //*************COMUNICACIÓN TALKER->MANAGER*************
   while (1)
   {
      // datosTalk.opcion = -1;
      // Ciclo infinito para esperar a que se conecten los Talkers
      printf("Manager esperando solicitudes del talker...\n");
      // Leer la estructura del talker
      cuantos = read(fd_t, &datosTalk, sizeof(datosTalk));
      if (cuantos == -1)
      {
         perror("Error leyendo información enviada por el Talker: ");
         exit(1);
      }
      //*************FIN COMUNICACIÓN TALKER->MANAGER*************
      printf("Talker %d opcion %d\n", datosTalk.idTalker, datosTalk.opcion);
      switch (datosTalk.opcion)
      {
      case 0: // Registrar al talker
         // Crear el pipe personalizado según el pid del talker
         char pipe_pers[20];
         sprintf(pipe_pers, "pipe%d", datosTalk.pid);
         unlink(pipe_pers);
         if (mkfifo(pipe_pers, fifo_mode) == -1)
         {
            perror("Error creando el pipe personalizado: ");
            exit(1);
         }

         // Abrir el pipe para comunicación manager->talker
         fd_m[datosTalk.idTalker] = open(pipe_pers, O_WRONLY);
         if (fd_m[datosTalk.idTalker] == -1)
         {
            perror("Error abriendo el pipe Manager->Talker: ");
            exit(1);
         }
         printf("%d,%s", fd_m[datosTalk.idTalker], pipe_pers);
         // Validación para registro Talker
         if (datosMan.numMaxUsuarios < datosTalk.idTalker)
         {
            printf("ID de usuario superior al máximo\n");
         }
         else
         {
            if (datosMan.registrados[datosTalk.idTalker] == 1)
            {
               printf("ID de usuario ya registrado\n");
               datosMan.estaregistrado = 1;
            }
            else
            {
               // Registrar al talker
               printf("Talker (%d) registrado con pid %d y fd%d\n", datosTalk.idTalker, datosTalk.pid, fd_m[datosTalk.idTalker]);
               datosMan.listaConectados[datosTalk.idTalker] = datosTalk.pid;
               datosMan.registrados[datosTalk.idTalker] = 1;
            }
         }
         // Enviar la estructura del manager
         write(fd_m[datosTalk.idTalker], &datosMan, sizeof(datosMan));
         break;
      case 1: // Enviar lista de usuarios conectados
         write(fd_m[datosTalk.idTalker], &datosMan, sizeof(datosMan));
         printf("Lista de usuarios enviada al talker (%d)\n", datosTalk.idTalker);
         break;
      case 2:
         printf("Talker %d solicita los integrantes del grupo G%d", datosTalk.idTalker,datosTalk.grupoAListar);
         // Comprobar que el grupo existe
         if(grupos[datosTalk.grupoAListar-1].gid != datosTalk.grupoAListar){
            printf("El grupo %d no existe\n",datosTalk.grupoAListar);
            datosMan.grupocreado = 0;
         }else{
            printf("El grupo G%d existe, se envian los integrantes al talker %d\n",datosTalk.grupoAListar,datosTalk.idTalker);
            datosMan.grupocreado = 1;
           datosMan.numintegrantes=grupos[datosTalk.grupoAListar-1].numintegrantes;
           for(int i=0;i<datosMan.numintegrantes;i++){
            datosMan.usuariosXGrupo[i]=grupos[datosTalk.grupoAListar-1].idUser[i];
           }
         }
         write(fd_m[datosTalk.idTalker], &datosMan, sizeof(datosMan));
         break;
      case 3:
         printf("Talker %d solicita cración de grupo con talkers ", datosTalk.idTalker);
         for (int i = 0; i < datosTalk.numintegrantes; i++)
         {
            printf("%d,", datosTalk.idsgrupos[i]);
         }
         printf("\n");
         // Comprobar que los talkers existen
         datosMan.grupocreado = 1;
         for (int i = 0; i < datosTalk.numintegrantes; i++)
         { // Recorre el arreglo de registrados y cuenta los que están registrados
            if (datosMan.registrados[datosTalk.idsgrupos[i]] != 1)
            {
               printf("No se puede crear el grupo %d porque no todos los talkers existen\n", numg + 1);
               datosMan.grupocreado = 0;
               write(fd_m[datosTalk.idTalker], &datosMan, sizeof(datosMan));
               break;
            }
         }
         if(datosMan.grupocreado == 0){//Break para salir del switch
            break;
         }
         // Crear el grupo
         grupos[numg].numintegrantes = datosTalk.numintegrantes; // Asigna el número de integrantes
         grupos[numg].gid = numg + 1; // Asigna el id del grupo

         grupos[numg].idUser[0] = datosTalk.idTalker; // Asigna el id del talker que crea el grupo

         for (int i = 0; i < datosTalk.numintegrantes; i++)
         {
            grupos[numg].idUser[i + 1] = datosTalk.idsgrupos[i]; // Asigna los id de los talkers que se unen al grupo
         }
         char mensaje[TAMMENSAJE];
         sprintf(mensaje, "Talker forma parte del grupo %d", grupos[numg].gid);
         for (int i = 0; i < datosTalk.numintegrantes; i++)//Numintegrantes no cuenta el integrante que crea el grupo
         {                                                                          // Numintegrantes sin el id del que crea el grupo
            int proccessid = datosMan.listaConectados[grupos[numg].idUser[i + 1]]; // Se le suma uno porque la primera posición corresponde al que mandó el mensaje
            write(fd_m[grupos[numg].idUser[i + 1]], mensaje, sizeof(mensaje));
            printf("Enviando mensaje por pipe %d", fd_m[grupos[numg].idUser[i + 1]]);
            sleep(1);
            printf("Enviando señal SIGUSR1 a %d\n", proccessid);
            kill(proccessid, SIGUSR1);
         }
         datosMan.grupocreado = 1;
         write(fd_m[datosTalk.idTalker], &datosMan, sizeof(datosMan));
         printf("Se crea el grupo con identificador G%d\n", numg + 1);
         numg++;

         break;
      default:
         break;
      }
   }
   // close(fd_m);
   close(fd_t);
   unlink(pipet_m);
}

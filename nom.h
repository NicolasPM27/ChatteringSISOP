#define TAMNOM 20
#define PIPE2 "man2Talk"
#define NUMMAX 100
#define TAMMENSAJE 100
typedef  struct datam {
    char nombrePipeInicial[TAMNOM];
    int numMaxUsuarios;
    int listaConectados[NUMMAX];//Pid de los usuarios conectados
    int registrados[NUMMAX];//0 no registrado, 1 registrado
    int estaregistrado;
    int usuariosXGrupo[NUMMAX];//Numero de usuarios por grupo
    int numintegrantes;
    int grupocreado;//0 no creado, 1 creado
    int mensajeenviado;//0 no enviado, 1 enviado
    int mensajeenviadogrupo;//0 no enviado, 1 enviado

} dataman;
typedef struct datat {
    char nombrePipeInicial[TAMNOM];
    int idTalker;
    int pid;
    int opcion;
    int idsgrupos[NUMMAX];
    int numintegrantes;
    int grupoAListar;
    char mensaje[TAMMENSAJE];
    int idDestino;
} datatalk;
typedef struct group{
  int gid;
  int numintegrantes;
  int idUser[NUMMAX];
}groups;


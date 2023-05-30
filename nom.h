#define TAMNOM 20
#define PIPE2 "man2Talk"
#define NUMMAX 100
typedef  struct datam {
    char nombrePipeInicial[TAMNOM];
    int numMaxUsuarios;
    int listaConectados[NUMMAX];//Pid de los usuarios conectados
    int registrados[NUMMAX];//0 no registrado, 1 registrado
    int estaregistrado;

} dataman;
typedef struct datat {
    char nombrePipeInicial[TAMNOM];
    int idTalker;
    int pid;
    int opcion;
    int idsgrupos[NUMMAX];
    int numintegrantes;
} datatalk;
typedef struct group{
  int gid;
  int pidUser[NUMMAX];
}groups;


#define TAMNOM 20
#define PIPE2 "man2Talk"
#define NUMMAX 100
typedef  struct datam {
    char nombrePipeInicial[TAMNOM];
    int numMaxUsuarios;
    int listaConectados[NUMMAX];
    int estaregistrado;

} dataman;
typedef struct datat {
    char nombrePipeInicial[TAMNOM];
    int idTalker;
    int pid;
    int opcion;
} datatalk;
typedef struct group{
  int gid;
  int pidUser[NUMMAX];
}groups;


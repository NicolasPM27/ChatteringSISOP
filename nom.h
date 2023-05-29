#define TAMNOM 20
typedef  struct datam {
    char nombrePipeInicial[TAMNOM];
    int numMaxUsuarios;

} dataman;
typedef struct datat {
    char nombrePipeInicial[TAMNOM];
    int idTalker;
    int pidTalker;
    bool conectado; // 0 no conectado, 1 conectado
    int opcion;
} datatalk;


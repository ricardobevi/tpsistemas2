#ifndef T_PROTOCOLO_H_
#define T_PROTOCOLO_H_


// -------------------------- define para memoria compartida --------------------------- //
// trafico de datos:
//                      SAC : servidor a cliente
//                      CAS : cliente a servidor

#define SERVIDOR   0
#define CLIENTE    1

#define S_INICIO_SERV 0
#define S_INICIO_CLI  1

#define S_BLOCK_CLI_SAC   2
#define S_BLOCK_SERV_SAC  3

#define S_BLOCK_CLI_CAS   4
#define S_BLOCK_SERV_CAS  5

// -------------------------- define para memoria compartida --------------------------- //

typedef struct
{
  char id;
  unsigned int  posicion; 
  int  x;
  int  y;
  
}t_protocolo;

#endif

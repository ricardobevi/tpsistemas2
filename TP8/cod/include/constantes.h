#ifndef CONSTANTES_MEM_H
#define CONSTANTES_MEM_H

// -------------------------- define para memoria compartida --------------------------- //
// trafico de datos:
//                      SAC : servidor a cliente
//                      CAS : cliente a servidor

#define SERVIDOR          0
#define CLIENTE           1

#define CANT_SEM          7

#define PATH_KEY         "/"
#define SEM_KEY          'O'
#define MEM_SAC_KEY      'I'
#define MEM_CAS_KEY      'U'

#define SERVIDOR_ACTIVO   6
#define CLIENTE_ACTIVO    7

#define S_INICIO_SERV     0
#define S_INICIO_CLI      1

#define S_BLOCK_CLI_SAC   2
#define S_BLOCK_SERV_SAC  3

#define S_BLOCK_CLI_CAS   4
#define S_BLOCK_SERV_CAS  5

// --------------------- fin de define para memoria compartida ------------------------ //

#endif
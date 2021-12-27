#ifndef UDP_H
#define UDP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct tm TM;

#define AUTHENTIFICATION_ERROR 0
#define AUTHENTIFICATION_SUCCESS 1
#define BUFF_SIZE 100
#define NO_OPERATION -1

typedef struct operation{
  int type; //1=ajout/0=retrait/-1=no_operation
  time_t date;
  float montant;
} Operation_t;

typedef struct compte{
  int id_compte;
  float solde;
  Operation_t operations[10];
} Compte_t;

typedef struct client{
  int id_client;
  int nb_compte;
  char* password;
  Compte_t* tab_compte;
} Client_t;

#endif

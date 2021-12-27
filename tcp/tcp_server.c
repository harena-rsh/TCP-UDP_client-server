#include "tcp.h"

/*Méthode du serveur*/
int main(){
  /*Création et initialisation du client*/
  Client_t test;
  test.id_client=7; //ID client sur 4 chiffres
  test.nb_compte=2;
  test.password="123456"; //Password sur 6 caractères

  Compte_t tab_compte_test[test.nb_compte];
  test.tab_compte = tab_compte_test;
  for (int i=0; i<test.nb_compte; i++){
    test.tab_compte[i].id_compte = i;
    test.tab_compte[i].solde = 0;
    time_t rawtime;
    time(&rawtime);
    for (int j=0; j<10; j++){
      test.tab_compte[i].operations[j].type = NO_OPERATION;
      test.tab_compte[i].operations[j].date = rawtime;
      test.tab_compte[i].operations[j].montant = 0;
    }
  }

  int status;

  /*Initialisation des buffers côté serveur*/
  SOCKADDR_IN addrport_server;
  char rcv_buff[BUFF_SIZE], send_buff[BUFF_SIZE];

  // Clean buffers:
  memset(send_buff, '\0', sizeof(send_buff));
  memset(rcv_buff, '\0', sizeof(rcv_buff));

  /*Initialisation côté client*/
  int client_sock;
  SOCKADDR_IN addrport_client;
  int size_addrport_client = sizeof(addrport_client);

  int sock_server=socket(PF_INET, SOCK_STREAM,0); //Création du socket serveur
  /*Vérifier si le socket s'est bien créé*/
  if(sock_server==-1){
    printf("Impossible de créer le socket!\n");
  }
  else{
    printf("Socket créé\n");
  }

  /*Initialisation de l'adresse du serveur*/
  addrport_server.sin_family=AF_INET;
  addrport_server.sin_port=htons(5100);
  addrport_server.sin_addr.s_addr=htonl(INADDR_ANY);

  /*Associer et réserver un port pour le socket qu'on vient de créer*/
  status=bind(sock_server,(SOCKADDR*)&addrport_server,sizeof(addrport_server));
  /*Vérification si la liaison s'est bien faite*/
  if(status==-1){
    printf("La liaison a échoué !\n");
  }
  else{
    printf("La liaison a réussi !\n");
  }

  /*Le serveur écoute les connexions*/
  status=listen(sock_server,2); //Ici le serveur peut acceuillir jusqu'à deux participants
  if(status==-1){
    printf("L'écoute a échoué !\n");
  }
  else{
    printf("En attente des connexions entrantes\n");
  }

  //Accepter une connexion
  client_sock=accept(sock_server,(SOCKADDR*)&addrport_client,(socklen_t*)&size_addrport_client);
  if(client_sock==SOCKET_ERROR){
    perror("Connexion échouée !\n");
    exit(EXIT_FAILURE);
  }
  else{
    printf("Connexion réussie !\n");
  }

  while(1){

    //Le serveur reçoit quelque chose
    if (recv(client_sock,rcv_buff,BUFF_SIZE,0) == SOCKET_ERROR){
      perror("recv() failed");
      exit(EXIT_FAILURE);
    }
    else{
      //printf("Reception OK !\n");
      /*printf("Message received: ");
      for(int i=0; i<BUFF_SIZE; i++)*/{
        //printf("%c",rcv_buff[i]);
      }
      //printf("\n");

      /* Le premier caractère du buffer est utilisé comme code d'opération*/
      char op_code_char = rcv_buff[0];
      //printf("op_code_char = %c\n",op_code_char);
      int op_code = op_code_char - '0';
      /*Variables nécessaires à l'Authentification*/
      char id_client_string[4];
      char pswd[6];
      int id_client;
      char id_compte_string[4];
      int id_compte;
      //printf("OPCODE=%d\n",op_code );

      switch(op_code){
        /*Authentification*/
        case 0:
                for (int i=0; i<4; i++){
                  id_client_string[i]=rcv_buff[i+1];
                }
                for (int i=0; i<6; i++){
                  pswd[i]=rcv_buff[i+5];
                }
                id_client = atoi(id_client_string);
                if(!strcmp(test.password,pswd)){
                  strcpy(send_buff,"1");
                }
                else {strcpy(send_buff,"0");}
                break;
        /* Ajout */
        case 1:
                for (int i=0; i<4; i++){
                  id_compte_string[i]=rcv_buff[i+1];
                }
                id_compte = atoi(id_compte_string);
                //printf("Accès au compte %d\n", id_compte);
                if (id_compte<0 || id_compte>test.nb_compte){
                  strcpy(send_buff,"Numéro de compte invalide !\n");
                }
                else{
                  char sum_int_str[6]="00000\0";
                  for (int i=0; i<5; i++){
                    sum_int_str[i] = rcv_buff[i+5];
                  }
                  printf("\n");
                  int sum_int = atoi(sum_int_str);
                  char sum_tenth_str[2] = "0\0";
                  sum_tenth_str[0]=rcv_buff[11];
                  int sum_tenth = atoi(sum_tenth_str);
                  char sum_hndrth_str[2] = "0\0";
                  sum_hndrth_str[0]=rcv_buff[12];
                  int sum_hndrth = atoi(sum_hndrth_str);
                  // Calcul du montant
                  float sum = sum_int + 0.1*sum_tenth + 0.01*sum_hndrth;
                  test.tab_compte[id_compte].solde += sum;
                  for (int i=0; i<9; i++){
                      test.tab_compte[id_compte].operations[i]=test.tab_compte[id_compte].operations[i+1];
                  }
                  test.tab_compte[id_compte].operations[9].type = 1;
                  time_t rawtime;
                  time(&rawtime);
                  test.tab_compte[id_compte].operations[9].date = rawtime;
                  test.tab_compte[id_compte].operations[9].montant = sum;
                  strcpy(send_buff,"Ajout effectué !\n");
                }
                break;
        /* Retrait */
        case 2:
                for (int i=0; i<4; i++){
                  id_compte_string[i]=rcv_buff[i+1];
                }
                id_compte = atoi(id_compte_string);
                if (id_compte<0 || id_compte>test.nb_compte){
                  strcpy(send_buff,"Numéro de compte invalide !\n");
                }
                else{
                  char sum_int_str[5]="0000\0";
                  for (int i=0; i<4; i++){
                    sum_int_str[i] = rcv_buff[i+5];
                  }
                  printf("\n");
                  int sum_int = atoi(sum_int_str);
                  char sum_tenth_str[2] = "0\0";
                  sum_tenth_str[0]=rcv_buff[10];
                  int sum_tenth = atoi(sum_tenth_str);
                  char sum_hndrth_str[2] = "0\0";
                  sum_hndrth_str[0]=rcv_buff[11];
                  int sum_hndrth = atoi(sum_hndrth_str);
                  // Calcul du montant
                  float sum = sum_int + 0.1*sum_tenth + 0.01*sum_hndrth;
                  test.tab_compte[id_compte].solde -= sum;
                  for (int i=0; i<9; i++){
                      test.tab_compte[id_compte].operations[i]=test.tab_compte[id_compte].operations[i+1];
                  }
                  test.tab_compte[id_compte].operations[9].type = 2;
                  time_t rawtime;
                  time(&rawtime);
                  test.tab_compte[id_compte].operations[9].date = rawtime;
                  test.tab_compte[id_compte].operations[9].montant = sum;
                  strcpy(send_buff,"Retrait effectué\n");
                }
                break;

        /* Solde */
        case 3:
                for (int i=0; i<4; i++){
                  id_compte_string[i]=rcv_buff[i+1];
                }
                id_compte = atoi(id_compte_string);
                if (id_compte<0 || id_compte>test.nb_compte){
                  strcpy(send_buff,"Numéro de compte invalide !\n");
                }
                else{
                  sprintf(send_buff,"Votre solde est de : %.2f€\n", test.tab_compte[id_compte].solde);
                }
                break;
        /* Operations */
        case 4:
                for (int i=0; i<4; i++){
                  id_compte_string[i]=rcv_buff[i+1];
                }
                id_compte = atoi(id_compte_string);
                if (id_compte<0 || id_compte>test.nb_compte){
                  strcpy(send_buff,"Numéro de compte invalide !\n");
                }
                else{
                  for(int i=0; i<10; i++){
                    sprintf(send_buff,"Type: %d| Date: %s | Montant: %08.2f\n", test.tab_compte[id_compte].operations[i].type,
                      ctime(&test.tab_compte[id_compte].operations[i].date) ,test.tab_compte[id_compte].operations[i].montant);
                    if (send(client_sock,send_buff,BUFF_SIZE,0) == SOCKET_ERROR){
                        perror("send() failed");
                        exit(EXIT_FAILURE);
                    }
                    else{
                          printf("My response: ");
                          for(int i=0; i<BUFF_SIZE; i++){
                            printf("%c",send_buff[i]);
                          }
                          printf("\n");
                          printf("Send OK !");
                    }
                  }
                }
                break;
        case 5: strcpy(send_buff,"Goodbye!");break;
        default: strcpy(send_buff,"Not yet implemented");break;
      }
    }
    //printf("Envoi terminé!\n");

    //Le serveur envoie quelque chose si ce n'est pas déjà fait (cas de OPERATIONS)
    if (rcv_buff[0]!='4'){
      if (send(client_sock,send_buff,BUFF_SIZE,0) == SOCKET_ERROR){
        perror("send() failed");
        exit(EXIT_FAILURE);
      }
      else{
        printf("My response: ");
        for(int i=0; i<BUFF_SIZE; i++){
            printf("%c",send_buff[i]);
          }
          printf("\n");
          //printf("Send OK !\n");
        }
      }
   }
   /*Terminer la conexion*/
   close(sock_server);

}

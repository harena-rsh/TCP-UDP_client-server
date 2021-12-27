#include "udp.h"

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


  int sock;
      struct sockaddr_in server_addr, client_addr;
      char buffer_send[2000], buffer_rcv[2000];
      int client_struct_length = sizeof(client_addr);

      // Clean buffers:
      memset(buffer_send, '\0', sizeof(buffer_send));
      memset(buffer_rcv, '\0', sizeof(buffer_rcv));

      // Create UDP socket:
      sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

      if(sock < 0){
          printf("Error while creating socket\n");
          return -1;
      }
      printf("Socket created successfully\n");

      // Set port and IP:
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(2000);
      server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

      // Bind to the set port and IP:
      if(bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
          printf("Couldn't bind to the port\n");
          return -1;
      }
      printf("Done with binding\n");

      printf("Listening for incoming messages...\n\n");

  while(1){

    //Le serveur reçoit quelque chose
    if (recvfrom(sock, buffer_rcv, sizeof(buffer_rcv), 0,
         (struct sockaddr*)&client_addr, (socklen_t*)&client_struct_length) < 0){
        printf("Couldn't receive\n");
        return -1;
    }
    else{
      //printf("Reception OK !\n");
      /*printf("Message received: ");
      for(int i=0; i<BUFF_SIZE; i++)*/{
        //printf("%c",buffer_rcv[i]);
      }
      //printf("\n");

      /* Le premier caractère du buffer est utilisé comme code d'opération*/
      char op_code_char = buffer_rcv[0];
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
                  id_client_string[i]=buffer_rcv[i+1];
                }
                for (int i=0; i<6; i++){
                  pswd[i]=buffer_rcv[i+5];
                }
                id_client = atoi(id_client_string);
                if(!strcmp(test.password,pswd)){
                  strcpy(buffer_send,"1");
                }
                else {strcpy(buffer_send,"0");}
                break;
        /* Ajout */
        case 1:
                for (int i=0; i<4; i++){
                  id_compte_string[i]=buffer_rcv[i+1];
                }
                id_compte = atoi(id_compte_string);
                //printf("Accès au compte %d\n", id_compte);
                if (id_compte<0 || id_compte>test.nb_compte){
                  strcpy(buffer_send,"Numéro de compte invalide !\n");
                }
                else{
                  char sum_int_str[6]="00000\0";
                  for (int i=0; i<5; i++){
                    sum_int_str[i] = buffer_rcv[i+5];
                  }
                  printf("\n");
                  int sum_int = atoi(sum_int_str);
                  char sum_tenth_str[2] = "0\0";
                  sum_tenth_str[0]=buffer_rcv[11];
                  int sum_tenth = atoi(sum_tenth_str);
                  char sum_hndrth_str[2] = "0\0";
                  sum_hndrth_str[0]=buffer_rcv[12];
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
                  strcpy(buffer_send,"Ajout effectué !\n");
                }
                break;
        /* Retrait */
        case 2:
                for (int i=0; i<4; i++){
                  id_compte_string[i]=buffer_rcv[i+1];
                }
                id_compte = atoi(id_compte_string);
                if (id_compte<0 || id_compte>test.nb_compte){
                  strcpy(buffer_send,"Numéro de compte invalide !\n");
                }
                else{
                  char sum_int_str[5]="0000\0";
                  for (int i=0; i<4; i++){
                    sum_int_str[i] = buffer_rcv[i+5];
                  }
                  printf("\n");
                  int sum_int = atoi(sum_int_str);
                  char sum_tenth_str[2] = "0\0";
                  sum_tenth_str[0]=buffer_rcv[10];
                  int sum_tenth = atoi(sum_tenth_str);
                  char sum_hndrth_str[2] = "0\0";
                  sum_hndrth_str[0]=buffer_rcv[11];
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
                  strcpy(buffer_send,"Retrait effectué\n");
                }
                break;

        /* Solde */
        case 3:
                for (int i=0; i<4; i++){
                  id_compte_string[i]=buffer_rcv[i+1];
                }
                id_compte = atoi(id_compte_string);
                if (id_compte<0 || id_compte>test.nb_compte){
                  strcpy(buffer_send,"Numéro de compte invalide !\n");
                }
                else{
                  sprintf(buffer_send,"Votre solde est de : %.2f€\n", test.tab_compte[id_compte].solde);
                }
                break;
        /* Operations */
        case 4:
                for (int i=0; i<4; i++){
                  id_compte_string[i]=buffer_rcv[i+1];
                }
                id_compte = atoi(id_compte_string);
                if (id_compte<0 || id_compte>test.nb_compte){
                  strcpy(buffer_send,"Numéro de compte invalide !\n");
                }
                else{
                  for(int i=0; i<10; i++){
                    sprintf(buffer_send,"Type: %d| Date: %s | Montant: %08.2f\n", test.tab_compte[id_compte].operations[i].type,
                      ctime(&test.tab_compte[id_compte].operations[i].date) ,test.tab_compte[id_compte].operations[i].montant);
                      if (sendto(sock, buffer_send, strlen(buffer_send), 0,
                           (struct sockaddr*)&client_addr, client_struct_length) < 0){
                          printf("Can't send\n");
                          return -1;
                      }
                    else{
                          printf("My response: ");
                          for(int i=0; i<BUFF_SIZE; i++){
                            printf("%c",buffer_send[i]);
                          }
                          printf("\n");
                          printf("Send OK !");
                    }
                  }
                }
                break;
        case 5: strcpy(buffer_send,"Goodbye!");break;
        default: strcpy(buffer_send,"Not yet implemented");break;
      }
    }
    //printf("Envoi terminé!\n");

    //Le serveur envoie quelque chose si ce n'est pas déjà fait (cas de OPERATIONS)
    if (buffer_rcv[0]!='4'){
      if (sendto(sock, buffer_send, strlen(buffer_send), 0,
           (struct sockaddr*)&client_addr, client_struct_length) < 0){
          printf("Can't send\n");
          return -1;
      }
      else{
        printf("My response: ");
        for(int i=0; i<BUFF_SIZE; i++){
            printf("%c",buffer_send[i]);
          }
          printf("\n");
          //printf("Send OK !\n");
        }
      }
   }
   /*Terminer la conexion*/
   close(sock);

}

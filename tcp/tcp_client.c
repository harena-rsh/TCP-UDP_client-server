#include "tcp.h"
/*Somme limite ajout: 10 000€
Somme limite retrait: 1 000€*/

/*Fonctions pour le client*/
int menu_choix(char*buffer_send,int socket_client){
  int req=0;
  int id_compte;
  float montant;
  printf("======== MENU ========\n");
  printf("Quelle opération voulez-vous effectuer?\n");
  printf("1.AJOUT\n2.RETRAIT\n3.SOLDE\n4.OPERATIONS\n5.QUITTER\n");
  scanf("%d",&req);

  switch(req){
    case 1:
      printf("Entrez le numéro du compte auquel vous voulez effectuer un ajout:\n");
      scanf("%d",&id_compte);
      printf("%d\n",id_compte);
      printf("Entrez le montant de l'ajout:\n");
      scanf("%f",&montant);
      sprintf(buffer_send,"1%.4d%08.2f",id_compte,montant);
      break;

    case 2:
      printf("Entrez le numéro du compte auquel vous voulez effectuer un retrait:\n");
      scanf("%d",&id_compte);
      printf("Entrez le montant du retrait:\n");
      scanf("%f",&montant);
      /*Modification de buffer_send*/
      sprintf(buffer_send,"2%.4d%07.2f",id_compte,montant);
      break;

    case 3:
      printf("Entrez le numéro du compte auquel dont vous voulez connaître le solde:\n");
      scanf("%d",&id_compte);
      sprintf(buffer_send,"3%.4d",id_compte);
      break;

    case 4:
      printf("Entrez le numéro du compte dont vous souhaitez voir les opérations:\n");
      scanf("%d",&id_compte);
      sprintf(buffer_send,"4%.4d",id_compte);
      break;
    case 5:
      sprintf(buffer_send,"5");
      break;
    default: printf("Veuillez sélectionner un numéro valide\n"); return 1;

  }

    /*printf("BUFFER SEND :");
    for(int i=0; i<BUFF_SIZE; i++){
        printf("%c",buffer_send[i]);
    }
    printf("\n");*/

    return 0;
}

/*Authetification*/
int auth(int socket_client,char*buffer_send,char*buffer_recv){
  int id_client;
  char password[6];

  printf("======== AUTHENTIFICATION ========\nEntrez votre numéro client:\n");
  scanf("%d",&id_client);
  printf("Entrez votre mot de passe:\n");
  scanf("%s",password);

  /*Remplir buffer send*/
  sprintf(buffer_send,"0%04d",id_client);
  strcat(buffer_send,password);

  if (send(socket_client,buffer_send,BUFF_SIZE,0)== SOCKET_ERROR){
      perror("send() failed");
      exit(EXIT_FAILURE);
    }

  /*Réponse du serveur*/
  if (recv(socket_client,buffer_recv, BUFF_SIZE, 0) == SOCKET_ERROR){
         perror("recv() failed");
         exit(EXIT_FAILURE);
  }
  else{
      printf("Server response: ");
      for(int i=0; i<BUFF_SIZE; i++){
          printf("%c",buffer_recv[i]);
      }
      printf("\n");
  }
  if (buffer_recv[0]=='1'){
      printf("AUTHENTIFICATION_SUCCESS\n");
      return 1;
    }
  else {
    printf("AUTHENTIFICATION_ERROR\n");
    return 0;
  }

}

int main(){

   int ret,a=0;
   char buffer_recv[BUFF_SIZE], buffer_send[BUFF_SIZE];

   // Clean buffers:
   memset(buffer_recv, '\0', sizeof(buffer_recv));
   memset(buffer_send, '\0', sizeof(buffer_send));

   // Create the client socket
   int socket_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if(socket_client == INVALID_SOCKET){
       perror("socket() failed"); // Socket creation failure
        exit(EXIT_FAILURE);
   }
   else{printf("Client socket created successfully\n");}

   // Initialize the server address
   SOCKADDR_IN server;

   server.sin_addr.s_addr = inet_addr("127.0.0.1");
   server.sin_family = AF_INET;
   server.sin_port = htons(5100);


 // Connect to the server
  if(connect(socket_client,(SOCKADDR*)&server,sizeof(server)) == SOCKET_ERROR){
     perror("connect()");
     exit(EXIT_FAILURE);
  }
  else{printf("Conection to the server established successfully\n");}

   //Communicate with the server
  while(!a){
    a=auth(socket_client,buffer_send,buffer_recv);
  }

   while(1){
        ret = menu_choix(buffer_send,socket_client); //Choix de la transaction à effectuer
        if (!ret){
          if (send(socket_client,buffer_send,BUFF_SIZE,0)== SOCKET_ERROR){
                 perror("send() failed");
                 exit(EXIT_FAILURE);
          }
          /* Si on envoie une requête de type OPERATIONS, on se met en attente de
           reception de 10 messages*/
          if (buffer_send[0]=='4'){
            for (int i=0; i<10; i++){
              if (recv(socket_client,buffer_recv, BUFF_SIZE, 0) == SOCKET_ERROR){
                 perror("recv() failed");
                 exit(EXIT_FAILURE);
               }
               else{
                    printf("Server response: ");
                    for(int i=0; i<BUFF_SIZE; i++){
                      printf("%c",buffer_recv[i]);
                    }
                printf("\n");
              }
             }
          }
          else{
            if (buffer_send[0]=='5'){
              if (recv(socket_client,buffer_recv, BUFF_SIZE, 0) == SOCKET_ERROR){
                 perror("recv() failed");
                 exit(EXIT_FAILURE);
               }
               else{
                    printf("Server response: ");
                    for(int i=0; i<BUFF_SIZE; i++){
                      printf("%c",buffer_recv[i]);
                    }
                printf("\n");
              }
              //Terminate the connection
              close(socket_client);
              return 0;
            }
            else{
              if (recv(socket_client,buffer_recv, BUFF_SIZE, 0) == SOCKET_ERROR){
                   perror("recv() failed");
                   exit(EXIT_FAILURE);
                 }
              else{
                   printf("Server response: ");
                   for(int i=0; i<BUFF_SIZE; i++){
                     printf("%c",buffer_recv[i]);
                   }
               printf("\n");
             }
            }
          }
        }
   }

   //Terminate the connection
   close(socket_client);
 }

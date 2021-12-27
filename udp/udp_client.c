#include "udp.h"

int menu_choix(char*buff_send,int sock){
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
      printf("Entrez le montant de l'ajout:\n");
      scanf("%f",&montant);
      sprintf(buff_send,"1%.4d%08.2f",id_compte,montant);
      break;

    case 2:
      printf("Entrez le numéro du compte auquel vous voulez effectuer un retrait:\n");
      scanf("%d",&id_compte);
      printf("Entrez le montant du retrait:\n");
      scanf("%f",&montant);
      /*Modification de buff_send*/
      sprintf(buff_send,"2%.4d%07.2f",id_compte,montant);
      break;

    case 3:
      printf("Entrez le numéro du compte auquel dont vous voulez connaître le solde:\n");
      scanf("%d",&id_compte);
      sprintf(buff_send,"3%.4d",id_compte);
      break;

    case 4:
      printf("Entrez le numéro du compte dont vous souhaitez voir les opérations:\n");
      scanf("%d",&id_compte);
      sprintf(buff_send,"4%.4d",id_compte);
      break;
    case 5:
      sprintf(buff_send,"5");
      break;
    default: printf("Veuillez sélectionner un numéro valide\n"); return 1;

  }

    /*printf("BUFFER SEND :");
    for(int i=0; i<BUFF_SIZE; i++){
        printf("%c",buff_send[i]);
    }
    printf("\n");*/

    return 0;
}

/*Authetification*/
int auth(int sock,char*buff_send,char*buff_rcv, struct sockaddr_in server_addr,int server_struct_length){
  int id_client;
  char password[6];

  printf("======== AUTHENTIFICATION ========\nEntrez votre numéro client:\n");
  scanf("%d",&id_client);
  printf("Entrez votre mot de passe:\n");
  scanf("%s",password);

  /*Remplir buffer send*/
  sprintf(buff_send,"0%04d",id_client);
  strcat(buff_send,password);

  if(sendto(sock, buff_send, strlen(buff_send), 0,(struct sockaddr*)&server_addr, server_struct_length) < 0){
      printf("Unable to send message\n");
      return -1;
  }

  // Receive the server's response:
  if(recvfrom(sock, buff_rcv, sizeof(buff_rcv), 0,(struct sockaddr*)&server_addr, (socklen_t*)&server_struct_length) < 0){
      printf("Error while receiving server's msg\n");
      return -1;
  }

  printf("Server's response: %s\n", buff_rcv);

  if (buff_rcv[0]=='1'){
      printf("AUTHENTIFICATION_SUCCESS\n");
      return 1;
    }
  else {
    printf("AUTHENTIFICATION_ERROR\n");
    return 0;
  }

}


int main(){

   int sock;
   int ret,a=0;
   struct sockaddr_in server_addr;
   char buff_rcv[BUFF_SIZE], buff_send[BUFF_SIZE];
   int server_struct_length = sizeof(server_addr);

   // Clean buffers:
   memset(buff_rcv, '\0', sizeof(buff_rcv));
   memset(buff_send, '\0', sizeof(buff_send));

   // Create socket:
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

   //Communicate with the server
  while(!a){
    a=auth(sock,buff_send,buff_rcv,server_addr,server_struct_length);
  }

   while(1){

        ret = menu_choix(buff_send,sock); //Choix de la transaction à effectuer
        if (!ret){
          if(sendto(sock, buff_send, strlen(buff_send), 0,(struct sockaddr*)&server_addr, server_struct_length) < 0){
              printf("Unable to send message\n");
              return -1;
          }
          /* Si on envoie une requête de type OPERATIONS, on se met en attente de
           reception de 10 messages*/
          if (buff_send[0]=='4'){
            for (int i=0; i<10; i++){
              if(recvfrom(sock, buff_rcv, sizeof(buff_rcv), 0,(struct sockaddr*)&server_addr, (socklen_t*)&server_struct_length) < 0){
                  printf("Error while receiving server's msg\n");
                  return -1;
              }

              printf("Server's response: %s\n", buff_rcv);
             }
          }
          else{
            if (buff_send[0]=='5'){
              if(recvfrom(sock, buff_rcv, sizeof(buff_rcv), 0,(struct sockaddr*)&server_addr, (socklen_t*)&server_struct_length) < 0){
                  printf("Error while receiving server's msg\n");
                  return -1;
              }

              printf("Server's response: %s\n", buff_rcv);
              //Terminate the connection
              close(sock);
              return 0;
            }
            else{
              if(recvfrom(sock, buff_rcv, sizeof(buff_rcv), 0,(struct sockaddr*)&server_addr, (socklen_t*)&server_struct_length) < 0){
                  printf("Error while receiving server's msg\n");
                  return -1;
              }

              printf("Server's response: %s\n", buff_rcv);
            }
          }
        }
   }

   //Terminate the connection
   close(sock);
 }

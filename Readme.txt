#Projet de RESEAUX ET APPLICATION

## Le projet

Le but de ce projet est d'implémenter une application permettant de gérer des
comptes bancaires en utilisant une architecture de type client/serveur.

Ce projet contient deux implémentations de cette application:
 1) une implémentation sous Linux d'un client/serveur en utilisant des sockets TCP
 2) une implémentation sous Linux d'un client/serveur en utilisant des sockets UDP

## Compilation

Pour compiler l'application utilisant TCP:
 - Ouvrir un terminal
 - Se mettre dans le dossier /tcp
 - Compiler le serveur avec la commande make tcp_server
 - Compiler le client avec la commande make tcp_client

 Pour compiler l'application utilisant UDP:
  - Ouvrir un terminal
  - Se mettre dans le dossier /udp
  - Compiler le serveur avec la commande make udp_server
  - Compiler le client avec la commande make udp_client

## Exécution

 Pour exécuter l'application:
  - ouvrir deux Terminaux
  - Se mettre dans le bon répertoire à exécuter (/tcp ou /udp)
  - Exécuter dans le premier terminal le serveur avec la commande ./tcp_server ou ./udp_server
  Le serveur est lancé et est en attente de connexions entrantes.
  - Exécuter dans le second terminal le client avec la commande ./tcp_client ou ./udp_client
  Le client se connecte au serveur.

  L'application est lancée. Il y a au début une phase d'authentification.
  Numéro client : 0007
  Mot de passe: 123456
  Une fois la phase d'authentification réussie, les demandes d'opérations peuvent êtres effectuées.

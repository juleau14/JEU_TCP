/* =================================================================== */
// Progrmame Serveur qui calcule le résultat d'un coup joué à partir
// des coordonnées reçues de la part d'un client "joueur".
// Version ITERATIVE : 1 seul client/joueur à la fois
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tresor.h"
#include "tresor.c"

/* =================================================================== */
/* FONCTION PRINCIPALE : SERVEUR ITERATIF                              */
/* =================================================================== */
int main(int argc, char **argv) {

    int socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // creation du socket d'ecoute

    if (socket_desc == -1) {
        perror("Erreur de création du socket.\n");
        return EXIT_FAILURE;
    }

    else {
      printf("Création socket ok\n");
    }

    // Binding (définition du numéro de port sur lequel le socket va écouter)

    struct sockaddr_in localaddr;                // on créée la struct qui va contenir les infos necessaires au binding
    memset(&localaddr, 0, sizeof(localaddr)); // init des valeurs à 0
    localaddr.sin_family = AF_INET;           // domaine AF_INET
    localaddr.sin_port = htons(5555);         // définition du port
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);   // définition de l'adresse

    int return_bind = bind(socket_desc, (struct sockaddr *) &localaddr, sizeof(localaddr)); // binding

    if (return_bind == -1) {
      perror("Erreur binding");
      close(socket_desc);
      return EXIT_FAILURE;
    }

    else {
      printf("Binding OK\n");
    }

    int list = listen(socket_desc, 10);        // mise du socket à l'etat d'ecoute

    if (list == -1) {
        perror("Erreur ecoute.");
        close(socket_desc);
        return EXIT_FAILURE;
    }

    else {      // si le socket est bien mis à l'écoute
        printf("Mise à l'ecoute ok\n");

        int newsock;      // création du socket qui va gérer les connexions entrantes
        struct sockaddr_in cli_adr; // struct qui va recevoir l'adresse du client
        socklen_t cli_adr_len = sizeof(cli_adr);
        newsock = accept(socket_desc, (struct sockaddr *)&cli_adr, &cli_adr_len);   // acceptation de la connexion TCP -> gestion de cette connexion bascule sur un nouveau socket
        printf("ok\n");
        if (newsock < 0) {
            perror("accept failed");
            close(socket_desc);
            exit(EXIT_FAILURE);
        }

        else {      // si aucune erreur, la partie commence
            printf("Connexion bien acceptée, début de la partie\n");
            int res = -1;     // initialisation du resultat des tours
            int lig_tresor = 5;
            int col_tresor = 5;
            char requete_recue[10];
            char res_char[10];

            while (res != 0) {    // tant que le joueur ne trouve pas le trésor
              if (recv(newsock, requete_recue, sizeof(requete_recue), 0) == -1) {
                perror("Erreur réception.");
                close(newsock);
                close(socket_desc);
                return EXIT_FAILURE;
              }

              else {
                printf("requete %s\n", requete_recue);
                char lig_recue_char[10];
                char col_recue_char[10];

                lig_recue_char[0] = requete_recue[0];
                col_recue_char[0] = requete_recue[2];

                int lig_recu = atoi(lig_recue_char);
                int col_recu = atoi(col_recue_char);

                res = recherche_tresor(10, lig_tresor, col_tresor, lig_recu, col_recu);
                sprintf(res_char, "%d", res);

                if (send((newsock), &res_char, sizeof(res_char), 0) == -1) {
                  perror("Erreur envoie.");
                  close(newsock);
                  close(socket_desc);
                  return EXIT_FAILURE;
                }
                else {
                  printf("\nresultat bien envoyé\n str : %s\n int : %d\n", res_char, res);
                }
              }

          }
        }
    }
} // end main

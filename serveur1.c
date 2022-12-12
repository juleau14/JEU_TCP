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
            int lig_tresor = rand() % 10;
            int col_tresor = rand() % 10;
            char requete_recue[10];

            while (res != 0) {    // tant que le joueur ne trouve pas le trésor
              if (recv(newsock, requete_recue, 2, 0) == -1) {
                perror("Erreur réception.");
                close(newsock);
                close(socket_desc);
                return EXIT_FAILURE;
              }

              else {
                int lig_choisie = requete_recue[0] - '0';
                int col_choisie = requete_recue[2] - '0';

                res = recherche_tresor(10, lig_tresor, col_tresor, lig_choisie, col_choisie);

                char c_res = res - '0';

                if (send((newsock), &c_res, sizeof(char), 0) == -1) {
                  perror("Erreur envoie.");
                  close(newsock);
                  close(socket_desc);
                  return EXIT_FAILURE;
                }
              }

          }
        }
    }
} // end main

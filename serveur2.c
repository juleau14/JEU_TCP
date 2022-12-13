/* =================================================================== */
// Progrmame Serveur qui calcule le résultat d'un coup joué à partir
// des coordonnées reçues de la part d'un client "joueur".
// Version CONCURRENTE : N clients/joueurs à la fois
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

int play(int socket) {
  int res = -1;     // initialisation du resultat des tours
  int lig_tresor = 5;
  int col_tresor = 5;
  char requete_recue[10];
  char res_char[10];

  while (res != 0) {    // tant que le joueur ne trouve pas le trésor
    if (recv(socket, requete_recue, sizeof(requete_recue), 0) == -1) {
      perror("Erreur réception.");
      close(socket);
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

      if (send((socket), &res_char, sizeof(res_char), 0) == -1) {
        perror("Erreur envoie.");
        close(socket);
        return EXIT_FAILURE;
      }
      else {
        printf("\nresultat bien envoyé\n str : %s\n int : %d\n", res_char, res);
      }
    }
  }
  close(socket);
  return EXIT_SUCCESS;
}




/* =================================================================== */
/* FONCTION PRINCIPALE : SERVEUR CONCURRENT                            */
/* =================================================================== */
int main(int argc, char **argv) {

    int main_sock = socket(AF_INET, SOCK_STREAM, 0);  // création du socket principal d'écoute

    if (main_sock == -1) {
      perror("Error socket creation.");
      return EXIT_FAILURE;
    }

    else {
      printf("Socket creation OK...\n");
    }

    /* BINDING */
    struct sockaddr_in local_addr;     // struct infos pour binding
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(5555);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int return_bind = bind(main_sock, (struct sockaddr *) &local_addr, sizeof(local_addr));

    if (return_bind == -1) {
      perror("Error binding.\n");
      return EXIT_FAILURE;
    }

    else {
      printf("Binding OK...\n");
    }

    int return_listen = listen(main_sock, 50);

    if (return_listen == -1) {
      perror("Error listening.\n");
      return EXIT_FAILURE;
    }

    else {
      printf("Socket listening OK...\n");
    }

    int secondary_socket;
    struct sockaddr_in client_addr;

    do {
      socklen_t client_len = sizeof(client_addr);
      secondary_socket = accept(main_sock, (struct sockaddr *) &client_addr, &client_len);

      if (secondary_socket < 0) {
        perror("Error accepting connection.\n");
        close(main_sock);
        return EXIT_FAILURE;
      }

      else {
        printf("Connection accepted OK...\n");

        switch(fork()) {
          case -1:
            perror("Error while creating child process.\n");
            close(main_sock);
            close(secondary_socket);
            return EXIT_FAILURE;

          case 0:
            play(secondary_socket);
            break;

          default:
            break;

        }
      }
    } while(1);

    return 0;
} // end main

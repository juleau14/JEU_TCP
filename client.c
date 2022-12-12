/* =================================================================== */
// Progrmame Client à destination d'un joueur qui doit deviner la case
// du trésor. Après chaque coup le résultat retourné par le serveur est
// affiché. Le coup consite en une abcsisse et une ordonnée (x, y).
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define N 10
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"

/* ====================================================================== */
/*                  Affichage du jeu en mode texte brut                   */
/* ====================================================================== */
void afficher_jeu(int jeu[N][N], int res, int points, int coups) {

    printf("\n************ TROUVEZ LE TRESOR ! ************\n");
    printf("    ");
    for (int i=0; i<10; i++)
        printf("  %d ", i+1);
    printf("\n    -----------------------------------------\n");
    for (int i=0; i<10; i++){
        printf("%2d  ", i+1);
        for (int j=0; j<10; j++) {
            printf("|");
            switch (jeu[i][j]) {
                case -1:
                    printf(" 0 ");
                    break;
                case 0:
                    printf(GREEN " T " RESET);
                    break;
                case 1:
                    printf(YELLOW " %d " RESET, jeu[i][j]);
                    break;
                case 2:
                    printf(RED " %d " RESET, jeu[i][j]);
                    break;
                case 3:
                    printf(MAGENTA " %d " RESET, jeu[i][j]);
                    break;
            }
        }
        printf("|\n");
    }
    printf("    -----------------------------------------\n");
    printf("Pts dernier coup %d | Pts total %d | Nb coups %d\n", res, points, coups);
}


/* ====================================================================== */
/*                    Fonction principale                                 */
/* ====================================================================== */
int main(int argc, char **argv) {

    int jeu[N][N];
    int lig, col;
    int res = -1, points = 0, coups = 0;

    /* Init args */
    char * adresse_ip = argv[1];
    u_short port = atoi(argv[2]);

    /* Init jeu */
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            jeu[i][j] = -1;

    /* Creation socket TCP */

    int socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_desc == -1) {
      perror("Erreur de création du socket.");
      return EXIT_FAILURE;
    }

    /* Init caracteristiques serveur distant (struct sockaddr_in) */


    /* Etablissement connexion TCP avec process serveur distant */
    struct sockaddr_in dst_serv_addr;

    memset(&dst_serv_addr, 0, sizeof(dst_serv_addr));
    dst_serv_addr.sin_family = AF_INET;
    dst_serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, adresse_ip, &(dst_serv_addr.sin_addr));

    int return_connect = connect(socket_desc, (struct sockaddr *) &dst_serv_addr, sizeof(dst_serv_addr)) ;

    if (return_connect == -1) {
      perror("Erreur connect.");
      close(socket_desc);
      return EXIT_FAILURE;
    }



    /* Tentatives du joueur : stoppe quand tresor trouvé */
    do {
        afficher_jeu(jeu, res, points, coups);
        printf("\nEntrer le numéro de ligne : ");
        scanf("%d", &lig);
        printf("Entrer le numéro de colonne : ");
        scanf("%d", &col);

        /* Construction requête (serialisation en chaines de caractères) */
        char requete[10];
        sprintf(requete, "%d %d", lig, col);


        /* Envoi de la requête au serveur (send) */
        int return_send = send(socket_desc, requete, 4, 0);

        if (return_send == -1) {
          perror("Erreur d'envoi au serveur.");
          close(socket_desc);
          return EXIT_FAILURE;
        }

        /* Réception du resultat du coup (recv) */
        char msg_recu[3];
        int return_recv = recv(socket_desc, msg_recu, 1, 0);

        if (return_recv == -1) {
          perror("Erreur réception.");
          close(socket_desc);
          return EXIT_FAILURE;
        }

        /* Deserialisation du résultat en un entier */
        // TODO
        res = atoi(msg_recu);

        /* Mise à jour */
        if (lig>=1 && lig<=N && col>=1 && col<=N)
            jeu[lig-1][col-1] = res;
        points += res;
        coups++;

    } while (res);

    /* Fermeture connexion TCP */
    close(socket_desc);

    /* Terminaison du jeu : le joueur a trouvé le tresor */
    afficher_jeu(jeu, res, points, coups);
    printf("\nBRAVO : trésor trouvé en %d essai(s) avec %d point(s)"
            " au total !\n\n", coups, points);
    return 0;
}

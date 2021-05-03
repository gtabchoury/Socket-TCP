#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>


//#include "shared_mem.h"  // Altera funcionamento do malloc, calloc e free
#include "tictactoe.h"

#define LISTENQ 10
#define MAXLINE 4096
#define MAXPLAYERS 10
#define MAXPOLL 30
#define TIMEOUT 1000
#define MAXNAME 50

struct sock_wrapper {
   int sock;
   char name[MAXNAME];
};

// Envia lista de jogadores conectados
void send_player_list(int sock, struct player** connected_players, int conn_players);
void new_connection(int sock, struct pollfd pfds[], char pdfs_name[MAXPOLL][MAXNAME], int* p_size);
void handle_command(int src, int n, char* command, struct player** connected_players, int conn_players);

int main (int argc, char **argv) {
   int    listenfd;
   // int connfd;
   struct sockaddr_in servaddr;
   char   recvline[MAXLINE + 1];

   struct sockaddr_in conn_settings;
   socklen_t sock_len;

   // Aloca o array de jogadores (compartilha a memoria entre os processos)
   struct player** connected_players = calloc(MAXPLAYERS, sizeof(struct player*));
   int conn_players = 0;

   // Cria o socket
   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
   }

   // Preenche informações da conexao
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = INADDR_ANY;
   servaddr.sin_port        = 0;   

   // Associa o socket com as configs
   if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
      perror("bind");
      exit(1);
   }

   // Verifica as informações de conexão
   sock_len = sizeof conn_settings;
   getsockname(listenfd, (struct sockaddr *) &conn_settings, &sock_len);
   printf("Socket usando o IP e PORTA %s %d\n", inet_ntoa(conn_settings.sin_addr), ntohs(conn_settings.sin_port));

   // Libera o socket para conexões
   if (listen(listenfd, LISTENQ) == -1) {
      perror("listen");
      exit(1);
   }

   // Cria estruturas para controle de conexões
   char pdfs_name[MAXPOLL][MAXNAME]; 
   struct pollfd pfds[MAXPOLL];
   int p_size = 0;

   // Insere socket a ser observado (para novas conexões)
   pfds[p_size].fd = listenfd;
   pfds[p_size].events = POLLIN;
   strcpy(pdfs_name[p_size], "LISTENFD");
   p_size++;

   while(1) {
      switch(poll(pfds, p_size, TIMEOUT)) {
         case -1: case 0:
            break;

         default:
            for(int i=0; i<p_size; i++) {
               if(pfds[i].revents & (POLLIN)) {
                  // Caso haja uma nova conexão
                  if(strcmp(pdfs_name[i], "LISTENFD") == 0)
                     new_connection(listenfd, pfds, pdfs_name, &p_size);

                  // Caso seja um comando de um cliente
                  else
                     if(pdfs_name[i][0] == 'c') {
                        int n = read(pfds[i].fd, recvline, MAXLINE);
                        recvline[n-1] = 0;

                        printf("Player %s joined\n", recvline);

                        struct player* player = create_player(pfds[i].fd, recvline);
                        connected_players[conn_players++] = player;
                        pdfs_name[i][0] = 'C';

                        send_player_list(pfds[i].fd, connected_players, conn_players);
                     } else if(pdfs_name[i][0] == 'C') {
                        int n = read(pfds[i].fd, recvline, MAXLINE);
                        handle_command(pfds[i].fd, n, recvline, connected_players, conn_players);
                     }
               }
            }
            break;
      }
   }

   close(listenfd);
   return 0;
}

void new_connection(int sock, struct pollfd pfds[], char pdfs_name[MAXPOLL][MAXNAME], int* p_size) {
   int connfd = accept(sock, (struct sockaddr *) NULL, NULL);

   if (connfd == -1 ) {
      perror("accept");
      exit(1);
   }

   pfds[*p_size].fd = connfd;
   pfds[*p_size].events = POLLIN;
   sprintf(pdfs_name[*p_size], "c-%d", connfd);
   (*p_size)++;

   write(connfd, "Who are you? ", 13);
}

void handle_command(int src, int n, char* command, struct player** connected_players, int conn_players) {
   if(n == 1 && command[0] == '\n') {
      send_player_list(src, connected_players, conn_players);
      return;
   }

   char cmd[10], p1[10], p2[10];
   sscanf(command, "%s %s %s", cmd, p1, p2);

   if(strcmp(cmd, "START") == 0){
      printf("Starting game with %s...\n", p1);
   }
   else {
      printf("Command %s %s %s not found\n", cmd, p1, p2);
   }
}

void send_player_list(int sock, struct player** connected_players, int conn_players) {
   char line[MAXLINE] = {0};
   char* cursor = line;

   cursor += sprintf(cursor, "\nConnected players:\n");

   for(int i=0; i<conn_players; i++) {
      struct player* player = connected_players[i];
      cursor += sprintf(cursor, "\t%2d. %s\n", i+1, player->name);
   }

   cursor += sprintf(cursor, "\n");
   cursor += sprintf(cursor, "Press ENTER to update connected players list\n");
   cursor += sprintf(cursor, "Type START <player> to start a game\n");
   cursor += sprintf(cursor, "\n");

   write(sock, line, cursor - line);
}

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

#include "arquivo.h"

#define LISTENQ 10
#define MAXLINE 4096
#define MAXPOLL 30
#define TIMEOUT 1000
#define MAXNAME 50

struct sock_wrapper {
   int sock;
   char name[MAXNAME];
};

char perfis[MAXPOLL][10][1000];

// Envia lista de jogadores conectados
void new_connection(int sock, struct pollfd pfds[], char pdfs_name[MAXPOLL][MAXNAME], int* p_size);
void handle_command(int src, int n, char* command, char* pdfs_name, int i);

char menu[] = "\nOlá! Escolha uma opção abaixo: \n\n1-Cadastrar novo perfil\n2-Adicionar experiência profissional\n3-Busca por curso\n4-Busca por habilidade\n5-Busca por ano de formação\n6-Busca por email\n7-Listar todos perfis\n8-Apagar perfil\nOpção: ";

int main (int argc, char **argv) {
   int    listenfd;
   struct sockaddr_in servaddr;
   char   recvline[MAXLINE + 1];
   struct sockaddr_in conn_settings;
   socklen_t sock_len;

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
                  if(strcmp(pdfs_name[i], "LISTENFD") == 0){
                     new_connection(listenfd, pfds, pdfs_name, &p_size);
                  }else{
                     if(pdfs_name[i][0] == 'm') {
                        int n = read(pfds[i].fd, recvline, MAXLINE);
                        int opcao = atoi(recvline);
                        
                        switch (opcao){
                           case 1:
                              pdfs_name[i][0] = '1';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Email: ", 7);
                           break;
                           case 3:
                              pdfs_name[i][0] = '3';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite o curso: ", 16);
                           break;
                           case 4:
                              pdfs_name[i][0] = '4';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite a habilidade: ", 21);
                           break;
                           case 5:
                              pdfs_name[i][0] = '5';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite o ano de formação: ", 28);
                           break;
                           case 6:
                              pdfs_name[i][0] = '6';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite o email: ", 16);
                           break;
                           case 7:
                              listAll(pfds[i].fd);
                              write(pfds[i].fd, menu, strlen(menu));
                           break;
                           default:
                              write(pfds[i].fd, "Opção inválida\nOpção: ", 28);
                           break;
                        }
                     } else {
                        int n = read(pfds[i].fd, recvline, MAXLINE);
                        handle_command(pfds[i].fd, n, recvline, pdfs_name[i], i);
                     }
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
   sprintf(pdfs_name[*p_size], "m-%d", connfd);
   (*p_size)++;

   write(connfd, menu, strlen(menu));
}

void handle_command(int src, int n, char* command, char* pdfs_name, int i) {
   char input[1000];
   
   sscanf(command, "%[^\n]s", input);

   char menu_opt = pdfs_name[0];
   char opt_step = pdfs_name[1];

   switch (menu_opt){
      case '1':
         switch (opt_step){
            case 'a':
               strcpy(perfis[i][0], input);
               write(src, "Nome: ", 6);
               pdfs_name[1]='b';
            break;
            case 'b':
               strcpy(perfis[i][1], input);
               write(src, "Sobrenome: ", 11);
               pdfs_name[1]='c';
            break;
            case 'c':
               strcpy(perfis[i][2], input);
               write(src, "Cidade de residência: ", 23);
               pdfs_name[1]='d';
            break;
            case 'd':
               strcpy(perfis[i][3], input);
               write(src, "Formação acadêmica: ", 23);
               pdfs_name[1]='e';
            break;
            case 'e':
               strcpy(perfis[i][4], input);
               write(src, "Ano de formatura: ", 18);
               pdfs_name[1]='f';
            break;
            case 'f':
               strcpy(perfis[i][5], input);
               write(src, "Habilidades (separadas por ,): ", 31);
               pdfs_name[1]='g';
            break;
            case 'g':
               strcpy(perfis[i][6], input);
               write(src, "Experiências (separadas por ,): ", 33);
               pdfs_name[1]='h';
            break;
            case 'h':
               strcpy(perfis[i][7], input);

               write(src, "\nPerfil cadastrado com sucesso!!!\n\n", 33);
               pdfs_name[0]='m';
               pdfs_name[1]='a';
               write(src, menu, strlen(menu));
            break;
            default:
            break;
         }
      break;
      case '3':
         switch (opt_step){
            case 'a':
               filterByCourse(src, input);
               pdfs_name[0]='m';
               pdfs_name[1]='a';
               write(src, menu, strlen(menu));
            break;
            default:
            break;
         }
      break;
      case '4':
         switch (opt_step){
            case 'a':
               filterBySkill(src, input);
               pdfs_name[0]='m';
               pdfs_name[1]='a';
               write(src, menu, strlen(menu));
            break;
            default:
            break;
         }
      break;
      case '5':
         switch (opt_step){
            case 'a':
               filterByGraduateYear(src, input);
               pdfs_name[0]='m';
               pdfs_name[1]='a';
               write(src, menu, strlen(menu));
            break;
            default:
            break;
         }
      break;
      case '6':
         switch (opt_step){
            case 'a':
               filterByEmail(src, input);
               pdfs_name[0]='m';
               pdfs_name[1]='a';
               write(src, menu, strlen(menu));
            break;
            default:
            break;
         }
      break;
      default:
      break;
   }
}
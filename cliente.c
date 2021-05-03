#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

#define MAXLINE 4096
#define TIMEOUT 1000


int main(int argc, char **argv) {
   int    sockfd, n;
   char   buffer[MAXLINE + 1];
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;
   char* char_p;

   // Verifica os parametros
   if (argc != 3) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      strcat(error," <Port>");
      perror(error);
      exit(1);
   }

   // Cria o socket
   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      exit(1);
   }

   // Informações de conexão
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(strtol(argv[2], &char_p, 10));

   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
   }

   // Estabelece conexão
   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
      perror("connect error");
      exit(1);
   }

   // Criando array de file descriptors
   struct pollfd pfds[2] = {
      { .fd = fileno(stdin), .events = POLLIN },
      { .fd = sockfd, .events = POLLIN }
   };

   // flag para indicar que o arquivo acabou
   char file_has_ended = 0;

   // diferença entre o que foi enviado e o que foi lido do servidor, inicia = 0
   long diff = 0;

   while(1) {
      // Verifica se existe alguma chamada não blockante no momento
      switch(poll(pfds, 2, TIMEOUT)) {
         case -1:
            perror("Erro");
            break;

         // case 0:
         //    perror("Timeout");
         //    break;

         default:
            // Caso a chamada seja o read no socket (e tenha algo para ler)
            if(pfds[1].revents & POLLIN && (n = read(sockfd, buffer, MAXLINE)) > 0) {
               buffer[n] = '\0';
               printf("%s", buffer);
               fflush(stdout);
               diff -= strlen(buffer);
            }

            // Caso a chamada seja o read no stdin (e o arquivo não tenha terminado)
            else if(pfds[0].revents & POLLIN && !file_has_ended) {
               // Tenta ler uma linha do arquivo
               if(fgets(buffer, MAXLINE, stdin) != NULL){
                  // Caso de sucesso, envia a linha ao servidor e contabiliza os bytes enviados
                  write(sockfd, buffer, strlen(buffer));
                  diff += strlen(buffer);
               }
               else{
                  // Caso de erro, fecha a transmissão de dados do socket e marca que o arquivo acabou
                  shutdown(sockfd, 1);
                  file_has_ended = 1;
               }
            }

            // // Caso nao tenha nada a ser feito, verifica se ja recebeu o arquivo inteiro
            // else if(diff == 0){
            //    close(sockfd);
            //    return 0;
            // }
            break;
      }
   }

   return 0;
}

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

   // Verifica os parâmetros de entrada (ip e porta do servido)
   if (argc != 3) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      strcat(error," <Port>");
      perror(error);
      exit(1);
   }

   // Cria o socket e verifica se não houve erro
   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      exit(1);
   }

   // Seta algumas informações da conexão
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(strtol(argv[2], &char_p, 10));

   // converte o endereço de texto para binario
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
   }

   // Estabelece a conexão com o servidor e verifica se não houve erro
   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
      perror("connect error");
      exit(1);
   }

   // Cria um array de file descriptors
   struct pollfd pfds[2] = {
      { .fd = fileno(stdin), .events = POLLIN },
      { .fd = sockfd, .events = POLLIN }
   };

   // flag que indica o fim do arquivo
   char file_has_ended = 0;

   // diferença entre o que foi enviado e o que foi recebido pelo servidor
   long diff = 0;

   while(1) {
      // Verifica se existe alguma chamada
      switch(poll(pfds, 2, TIMEOUT)) {
         case -1:
            perror("Erro");
            break;

         default:
            // Chamada no read do socket
            // imprime o que foi recebido pelo servidor e limpa o buffer
            if(pfds[1].revents & POLLIN && (n = read(sockfd, buffer, MAXLINE)) > 0) {
               buffer[n] = '\0';
               printf("%s", buffer);
               fflush(stdout);
               diff -= strlen(buffer);
            }

            // Chamada no read do stdin
            // lê uma linha do arquivo e envia a linha ao servidor
            else if(pfds[0].revents & POLLIN && !file_has_ended) {
               if(fgets(buffer, MAXLINE, stdin) != NULL){
                  write(sockfd, buffer, strlen(buffer));
                  diff += strlen(buffer);
               }else{
                  // Em caso de erro, encerra a transmissão de dados
                  shutdown(sockfd, 1);
                  file_has_ended = 1;
               }
            }
            break;
      }
   }
   return 0;
}

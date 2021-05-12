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

char emails[MAXPOLL][1000];

int main (int argc, char **argv) {
   int    listenfd;
   struct sockaddr_in servaddr;
   char   recvline[MAXLINE + 1];
   struct sockaddr_in conn_settings;
   socklen_t sock_len;

   // Cria o socket e verifica se não houve erro
   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
   }

   // Seta algumas informações da conexao
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = INADDR_ANY;
   servaddr.sin_port        = 0;   

   // Associa as informações ao socket e verifica se não houve erro
   if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
      perror("bind");
      exit(1);
   }

   // Checa as informações da conexão
   sock_len = sizeof conn_settings;
   getsockname(listenfd, (struct sockaddr *) &conn_settings, &sock_len);
   
   // Imprime o ip e a porta que o servidor está rodando
   printf("Socket usando o IP e PORTA %s %d\n", inet_ntoa(conn_settings.sin_addr), ntohs(conn_settings.sin_port));

   // Libera o socket para as conexões e verifica se não houve erro
   if (listen(listenfd, LISTENQ) == -1) {
      perror("listen");
      exit(1);
   }

   // Inicializa as estruturas utilizadas para o controle das conexões
   char pdfs_name[MAXPOLL][MAXNAME]; 
   struct pollfd pfds[MAXPOLL];
   int p_size = 0;

   // Insere socket a ser observado
   pfds[p_size].fd = listenfd;
   pfds[p_size].events = POLLIN;
   // LISTENFD utilizado para identificar uma nova conexão
   strcpy(pdfs_name[p_size], "LISTENFD");
   p_size++;

   while(1) {
      switch(poll(pfds, p_size, TIMEOUT)) {
         case -1: case 0:
            break;

         default:
            for(int i=0; i<p_size; i++) {
               if(pfds[i].revents & (POLLIN)) {
                  // Se for uma nova conexão
                  if(strcmp(pdfs_name[i], "LISTENFD") == 0){
                     new_connection(listenfd, pfds, pdfs_name, &p_size);
                  
                  // Se for uma conexão existente
                  }else{
                     // Se o usuário está no menu
                     if(pdfs_name[i][0] == 'm') {
                        int n = read(pfds[i].fd, recvline, MAXLINE);
                        int opcao = atoi(recvline);
                        
                        // verifica qual opção do menu foi selecionada
                        // e atualiza o estado da conexão onde:
                        // pdfs_name[i][0] corresponde ao estado do item do menu (1,2,3,4,5,6,7)
                        // pdfs_name[i][1] corresponde a estado da etapa de uma opção
                        switch (opcao){
                           case 1:
                              // Opção 1 - cadastro, solicita o email
                              pdfs_name[i][0] = '1';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Email: ", 7);
                           break;
                           case 2:
                              // Opção 2 - add experiência, solicita o email
                              pdfs_name[i][0] = '2';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite o email: ", 16);
                           break;
                           case 3:
                              // Opção 3 - busca por curso, solicita o curso
                              pdfs_name[i][0] = '3';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite o curso: ", 16);
                           break;
                           case 4:
                              // Opção 4 - busca por habilidade, solicita habilidade
                              pdfs_name[i][0] = '4';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite a habilidade: ", 21);
                           break;
                           case 5:
                              // Opção 5 - busca por ano de formação, solicita o ano
                              pdfs_name[i][0] = '5';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite o ano de formação: ", 28);
                           break;
                           case 6:
                              // Opção 6 - busca por email, solicita o email
                              pdfs_name[i][0] = '6';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite o email: ", 16);
                           break;
                           case 7:
                              // Opção 7 - lista todos os perfis
                              listAll(pfds[i].fd);
                              write(pfds[i].fd, menu, strlen(menu));
                           break;
                           case 8:
                              // Opção 8 - remove perfil, solicita o email
                              pdfs_name[i][0] = '8';
                              pdfs_name[i][1] = 'a';
                              write(pfds[i].fd, "Digite o email do perfil a ser removido: ", 41);
                           break;
                           default:
                              // Opção inválida, retorna erro e solicita a opção novamente
                              write(pfds[i].fd, "Opção inválida\nOpção: ", 28);
                           break;
                        }
                     
                     // Caso o usuário esteja em uma opção do menu
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
   // aceita a conexão
   int connfd = accept(sock, (struct sockaddr *) NULL, NULL);
   
   if (connfd == -1 ) {
      perror("accept");
      exit(1);
   }

   // inicializa variaveis de controle
   pfds[*p_size].fd = connfd;
   pfds[*p_size].events = POLLIN;

   // Seta o estado da conexão para o menu
   sprintf(pdfs_name[*p_size], "m-%d", connfd);
   (*p_size)++;

   // imprime o menu
   write(connfd, menu, strlen(menu));
}

// função que trata uma entrada fora do menu principal
void handle_command(int src, int n, char* command, char* pdfs_name, int i) {
   char input[1000];

   // lê a string enviada pelo cliente
   sscanf(command, "%[^\n]s", input);

   char menu_opt = pdfs_name[0];
   char opt_step = pdfs_name[1];

   // verifica qual opção do menu o usuário se encontra
   // e para cada opção verifica qual etapa o usuário se encontra
   switch (menu_opt){
      case '1':
         switch (opt_step){
            case 'a':
               // Opção 1/Etapa a: a string recebida é o email do perfil

               // se o email já existir, retorna erro
               if (checkEmail(input)>0){
                  write(src, "Email já cadastrado. Informe um email diferente: ", 50);
               }else{
                  // senão, salva o email no array tridimensional temporariamente
                  strcpy(perfis[i][0], input);

                  // solicita o nome do perfil
                  write(src, "Nome: ", 6);

                  // atualiza o estado para a etapa b
                  pdfs_name[1]='b';
               }
            break;
            case 'b':
               // Opção 1/Etapa b: a string recebida é o nome do perfil
               strcpy(perfis[i][1], input);

               // solicita o sobrenome
               write(src, "Sobrenome: ", 11);

               // atualiza o estado para a etapa c
               pdfs_name[1]='c';
            break;
            case 'c':
               // Opção 1/Etapa c: a string recebida é o sobrenome do perfil
               strcpy(perfis[i][2], input);

               // solicita a cidade de residência
               write(src, "Cidade de residência: ", 23);

               // atualiza o estado para a etapa d
               pdfs_name[1]='d';
            break;
            case 'd':
               // Opção 1/Etapa a: a string recebida é cidade de residência do perfil
               strcpy(perfis[i][3], input);

               // solicita a formação acadêmica
               write(src, "Formação acadêmica: ", 23);

               // atualiza o estado para a etapa e
               pdfs_name[1]='e';
            break;
            case 'e':
               // Opção 1/Etapa e: a string recebida é a formação acadêmica do perfil
               strcpy(perfis[i][4], input);

               // solicita o ano de formatura
               write(src, "Ano de formatura: ", 18);

               // atualiza o estado para a etapa f
               pdfs_name[1]='f';
            break;
            case 'f':
               // Opção 1/Etapa f: a string recebida é ano de formatura do perfil
               strcpy(perfis[i][5], input);

               // solicita as habilidades separadas por ponto e vírgula
               write(src, "Habilidades (separadas por ,): ", 31);

               // atualiza o estado para a etapa g
               pdfs_name[1]='g';
            break;
            case 'g':
               // Opção 1/Etapa g: a string recebida são as habilidades do perfil
               strcpy(perfis[i][6], input);

               // solicita as experiẽncias separadas por ponto e vírgula
               write(src, "Experiências (separadas por ,): ", 33);

               // atualiza o estado para a etapa h
               pdfs_name[1]='h';
            break;
            case 'h':
               // Opção 1/Etapa h: a string recebida são as experiẽncias do perfil
               strcpy(perfis[i][7], input);

               // cria o perfil, ou seja, salva no arquivo
               create(src, perfis[i]);

               // volta para o menu principal
               pdfs_name[0]='m';
               pdfs_name[1]='a';
               write(src, menu, strlen(menu));
            break;
            default:
            break;
         }
      break;
      case '2':
         switch (opt_step){
            case 'a':
               // Opção 2/Etapa a: verifica se o email existe
               if (checkEmail(input)>0){
                  // se sim, solicita a nova experiência a ser adicionada
                  write(src, "Digite a nova experiência: ", 28);
                  strcpy(emails[src], input);

                  // atualiza o estado para a etapa b
                  pdfs_name[1]='b';
               }else{
                  // se não, informa erro e solicita novamente
                  write(src, "Email não encontrado. Digite novamente: ", 41);
               }
            break;
            case 'b':
               // Opção 2/Etapa b: adiciona a experiência e volta para o menu principal
               addExperience(src, emails[src], input);
               pdfs_name[0]='m';
               pdfs_name[1]='a';
               write(src, menu, strlen(menu));
            break;
            default:
            break;
         }
      break;
      case '3':
         // Opção 3: filtra os perfis pelo curso informado
         filterByCourse(src, input);

         // volta para o menu principal
         pdfs_name[0]='m';
         pdfs_name[1]='a';
         write(src, menu, strlen(menu));
      break;
      case '4':
         // Opção 4: filtra os perfis pela habilidade informada
         filterBySkill(src, input);

         // volta para o menu principal
         pdfs_name[0]='m';
         pdfs_name[1]='a';
         write(src, menu, strlen(menu));
      break;
      case '5':
         // Opção 5: filtra os perfis pelo ano infomrado
         filterByGraduateYear(src, input);

         // volta para o menu principal
         pdfs_name[0]='m';
         pdfs_name[1]='a';
         write(src, menu, strlen(menu));
      break;
      case '6':
         // Opção 6: busca um perfil pelo email informado
         filterByEmail(src, input);

         // volta para o menu principal
         pdfs_name[0]='m';
         pdfs_name[1]='a';
         write(src, menu, strlen(menu));
      break;
      case '8':
         // Opção 8: verifica se o perfil existe
         if (checkEmail(input)>0){
            // se sim, remove e volta para o menu principal
            removeProfile(src, input);
            pdfs_name[0]='m';
            pdfs_name[1]='a';
            write(src, menu, strlen(menu));
         }else{
            // se não, informa o erro e solicita o email novamente
            write(src, "Email não encontrado. Digite novamente: ", 41);
         }
      break;
      default:
      break;
   }
}
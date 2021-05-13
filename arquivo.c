/*
  Arquivo contendo todas as funções referentes à manipulação do arquivo de dados, como listagem de usuários,
  cadastro de usuários, remoção de usuários, etc.
*/

#include <assert.h>
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
#include "arquivo.h"

#define ARQ_NAME "perfis.txt"

/*A função auxiliar str_split é responsável por separar uma string de acordo com um delimitador, retornando um array contendo os elementos da string.
  É utilizada no tratamento dos dados provenientes do arquivo perfis.txt. Por se tratar de uma função auxiliar sem relação com o objetivo do projeto,
  foi importada da internet.

  Parâmetros: 1: String a ser dividida. 2: Delimitador
*/
char** str_split(char* a_str, const char a_delim){
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp){
        if (a_delim == *tmp){
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result){
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token){
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

/*Função responsável por listar os usuários de acordo com um curso fornecido.
  
  Parâmetros: 1: Identificador do arquivo. 2: String contendo o curso buscado
*/
void filterByCourse(int src, char course[1000]){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }

    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    int count=0;

    /*A string "result" irá guardar a mensagem de resposta para a requisição*/
    char result[2000]="\nListando perfis com formação em: ";
    strcat(result, course);
    strcat(result, "\n---------------------------------------------------------\n");

    /*Percorre a matriz de dados, e conceta na string "result" o email e nome os usuários que 
      possuem como formação o curso buscado.
    */
    for(int i = 0; i < 10; i++){
        if(strstr(dados[i][4], course) != NULL) {
            count++;
            char perfil_id[2];
            sprintf(perfil_id, "%d", i);
            strcat(result, "Perfil #");
            strcat(result, perfil_id);
            strcat(result, "\nEmail: ");
            strcat(result, dados[i][0]);
            strcat(result, "\nNome: ");
            strcat(result, dados[i][1]);
            strcat(result, "\n---------------------------------------------------------\n");
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    /*Caso não tenha encontrado nenhum usuário com o curso buscado, imprime mensagem de erro*/
    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }
    /*Caso tenha encontrado algum usuário com o curso buscado, imprime seus dados*/
    else{
        write(src, result, strlen(result));
    }
    
}

/*Função responsável por listar os usuários de acordo com uma habilidade fornecida.
  
  Parâmetros: 1: Identificador do arquivo. 2: String contendo a habilidade buscada
*/
void filterBySkill(int src, char skill[1000]){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    int count=0;
    
    /*A string "result" irá guardar a mensagem de resposta para a requisição*/
    char result[2000]="\nListando perfis com habilidade: ";
    strcat(result, skill);
    strcat(result, "\n---------------------------------------------------------\n");

    /*Percorre a matriz de dados, e conceta na string "result" o email e nome dos usuários que 
      possuem a habilidade buscada.
    */
    for(int i = 0; i < 10; i++){
        if(strstr(dados[i][6], skill) != NULL) {
            count++;
            char perfil_id[2];
            sprintf(perfil_id, "%d", i);
            strcat(result, "Perfil #");
            strcat(result, perfil_id);
            strcat(result, "\nEmail: ");
            strcat(result, dados[i][0]);
            strcat(result, "\nNome: ");
            strcat(result, dados[i][1]);
            strcat(result, "\n---------------------------------------------------------\n");
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    /*Caso não tenha encontrado nenhum usuário com a habilidade buscada, imprime mensagem de erro*/
    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }
    /*Caso tenha encontrado algum usuário com a habilidade buscada, imprime seus dados*/
    else{
        write(src, result, strlen(result));
    }
}

/*Função responsável por listar os usuários de acordo com um ano de graduação.

  Parâmetros: 1: Identificador do arquivo. 2: String contendo o ano buscado 
*/
void filterByGraduateYear(int src, char year[1000]){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    int count=0;

    /*A string "result" irá guardar a mensagem de resposta para a requisição*/
    char result[2000]="\nListando perfis com ano de formação em: ";
    strcat(result, year);
    strcat(result, "\n---------------------------------------------------------\n");

     /*Percorre a matriz de dados, e conceta na string "result" o nome, email e curso dos usuários que 
      possuem como ano de graduação o ano buscado.
    */
    for(int i = 0; i < 10; i++){
        if(strstr(dados[i][5],year) != NULL) {
            count++;
            char perfil_id[2];
            sprintf(perfil_id, "%d", i);
            strcat(result, "Perfil #");
            strcat(result, perfil_id);
            strcat(result, "\nEmail: ");
            strcat(result, dados[i][0]);
            strcat(result, "\nNome: ");
            strcat(result, dados[i][1]);
            strcat(result, "\nCurso: ");
            strcat(result, dados[i][4]);
            strcat(result, "\n---------------------------------------------------------\n");
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    /*Caso não tenha encontrado nenhum usuário com o ano de graduação buscado, imprime mensagem de erro*/
    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }
    /*Caso tenha encontrado algum usuário com o ano buscado, imprime seus dados*/
    else{
        write(src, result, strlen(result));
    }
}

/*Função responsável por listar todos os dados de todos os usuários
 
  Parâmetros: 1: Identificador do arquivo
*/
void listAll(int src){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    int count=0;

    /*A string "result" irá guardar a mensagem de resposta para a requisição*/
    char result[10000]="\nListando todos os perfis...";
    strcat(result, "\n---------------------------------------------------------\n");

    /*Percorre a matriz de dados, e concatena na resposta todos os dados referentes a todos os usuários*/
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], "") != 0){
            count++;
            char perfil_id[2];
            sprintf(perfil_id, "%d", i);
            strcat(result, "Perfil #");
            strcat(result, perfil_id);
            strcat(result, "\nEmail: ");
            strcat(result, dados[i][0]);
            strcat(result, "\nNome: ");
            strcat(result, dados[i][1]);
            strcat(result, ", Sobrenome: ");
            strcat(result, dados[i][2]);
            strcat(result, "\nResidência: ");
            strcat(result, dados[i][3]);
            strcat(result, "\nFormação Acadêmica: ");
            strcat(result, dados[i][4]);
            strcat(result, "\nAno de formatura: ");
            strcat(result, dados[i][5]);
            strcat(result, "\nHabilidades: ");
            strcat(result, dados[i][6]);
            strcat(result, "\nExperiências: ");
            strcat(result, dados[i][7]);
            strcat(result, "\n---------------------------------------------------------\n");
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);
    
    /*Caso não tenha encontrado nenhum usuário, imprime mensagem de erro*/
    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }
    /*Caso tenha encontrado algum usuário, imprime o resultado*/
    else{
        write(src, result, strlen(result));
    }
}

/*Função auxiliar que verifica já existe na base algum usuário com email igual aquele enviado como parâmetro

  Parâmetros: 1. String contendo o email buscado
*/
int checkEmail(char email[1000]){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    int count=0;

    /*Percorre a matriz de dados, e verifica se existe algum email equivalente ao passado por parâmetro*/
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], email) == 0){
            count=1;
            break;
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    /*Retorna 0 caso o email não exista na base, e 1 caso exista*/
    return count;
}

/* Função responsável por inserir um novo registro na base
   
   Parâmetros: 1. Identificador do arquivo. 2: Matriz contendo os dados do novo perfil a ser inserido
*/
void create(int src, char new_profile[10][1000]){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    /*Guarda no auxiliar "count" o indíce da primeira linha vazia na matriz de dados, onde o novo usuário
      será inserido.
    */
    int count=0;
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], "") != 0){
            count++;
        }
    }

    
    /*Insere os dados do novo usuário na primeira linha vazia*/
    strcpy(dados[count][0],new_profile[0]);
    strcpy(dados[count][1],new_profile[1]);
    strcpy(dados[count][2],new_profile[2]);
    strcpy(dados[count][3],new_profile[3]);
    strcpy(dados[count][4],new_profile[4]);
    strcpy(dados[count][5],new_profile[5]);
    strcpy(dados[count][6],new_profile[6]);
    strcpy(dados[count][7],new_profile[7]);

    /*Fecha o arquivo*/
    fclose(pont_arq);

    /*Abre o arquivo com permissão de escrita, para salvar os dados atualizados*/
    pont_arq = fopen(ARQ_NAME, "w");

    /*Concatena todos os dados no formato adequado e salva na string output*/
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], "") != 0){
            char output[1000];
            sprintf(output,"%s",dados[i][0]);
            strcat(output, ";");
            strcat(output, dados[i][1]);
            strcat(output, ";");
            strcat(output, dados[i][2]);
            strcat(output, ";");
            strcat(output, dados[i][3]);
            strcat(output, ";");
            strcat(output, dados[i][4]);
            strcat(output, ";");
            strcat(output, dados[i][5]);
            strcat(output, ";");
            strcat(output, dados[i][6]);
            strcat(output, ";");
            strcat(output, dados[i][7]);
            strcat(output, ";\n");

            /*Salva os dados no arquivo*/
            r = fputs(output, pont_arq);
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    write(src, "\nPerfil cadastrado com sucesso!!!\n\n", 33);
}

/*Função responsável por listar os dados de um usuário de acordo com um email informado
 
  Parâmetros: 1: Identificador do arquivo. 2: String contendo o email buscado
*/
void filterByEmail(int src, char email[1000]){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    int count=0;

    /*A string "result" irá guardar a mensagem de resposta para a requisição*/
    char result[2000]="\nListando perfil com email: ";
    strcat(result, email);
    strcat(result, "\n---------------------------------------------------------\n");

    /*Percorre a matriz de dados e concatena na string de resultado todos os dados do usuário buscado*/
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], email) == 0){
            count++;
            char perfil_id[2];
            sprintf(perfil_id, "%d", i);
            strcat(result, "Perfil #");
            strcat(result, perfil_id);
            strcat(result, "\nEmail: ");
            strcat(result, dados[i][0]);
            strcat(result, "\nNome: ");
            strcat(result, dados[i][1]);
            strcat(result, ", Sobrenome: ");
            strcat(result, dados[i][2]);
            strcat(result, "\nResidência: ");
            strcat(result, dados[i][3]);
            strcat(result, "\nFormação Acadêmica: ");
            strcat(result, dados[i][4]);
            strcat(result, "\nAno de formatura: ");
            strcat(result, dados[i][5]);
            strcat(result, "\nHabilidades: ");
            strcat(result, dados[i][6]);
            strcat(result, "\nExperiências: ");
            strcat(result, dados[i][7]);
            strcat(result, "\n---------------------------------------------------------\n");
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    /*Caso não tenha encontrado nenhum usuário com o perfil buscado, imprime mensagem de erro*/
    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }
    /*Caso tenha encontrado algum usuário com o email buscado, imprime seus dados*/
    else{
        write(src, result, strlen(result));
    }
}


/*Função responsável por remover os dados de um usuário que contenha o email passado por parâmetro

  Parâmetros: 1: Identificador do arquivo. 2: String contendo o email do usuário a ser removido
*/
void removeProfile(int src, char email[1000]){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    int count=0;
    
    /*Procura na matriz de dados o usuário que possua o email informado, e caso encontre, apaga esse email,
      o que pela sequência do fluxo significará que a linha foi apagada.
    */
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], email) == 0){
            strcpy(dados[i][0], "");
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    /*Abre o arquivo com permissões de escrita*/
    pont_arq = fopen(ARQ_NAME, "w");

    /*Concatena na string output todos os dados dos usuários que possuam email. 
      Ou seja, todos os usuários menos aquele que foi removido.*/
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], "") != 0){
            char output[1000];
            sprintf(output,"%s",dados[i][0]);
            strcat(output, ";");
            strcat(output, dados[i][1]);
            strcat(output, ";");
            strcat(output, dados[i][2]);
            strcat(output, ";");
            strcat(output, dados[i][3]);
            strcat(output, ";");
            strcat(output, dados[i][4]);
            strcat(output, ";");
            strcat(output, dados[i][5]);
            strcat(output, ";");
            strcat(output, dados[i][6]);
            strcat(output, ";");
            strcat(output, dados[i][7]);
            strcat(output, ";\n");
            /*Grava os dados no arquivo*/
            r = fputs(output, pont_arq);
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    write(src, "\nPerfil removido com sucesso!!!\n\n", 33);
}

/*Função responsável por adicionar uma nova experiência ao usuário que possua o email passado por parâmetro

  Parâmetros: 1: Email do usuário. 2: String contendo a experiência a ser inserida
*/
void addExperience(int src, char email[1000], char experience[1000]){
    //Abre o arquivo contendo os dados dos usuários
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;

    //Matriz utilizada para guardar os dados dos usuários e facilitar sua manipulação
    char dados[10][10][1000];

    //Limpando a matriz para evitar problemas de persistência de memória
    for(int i = 0; i < 10; i++){
        strcpy(dados[i][0], "");
        strcpy(dados[i][1], "");
        strcpy(dados[i][2], "");
        strcpy(dados[i][3], "");
        strcpy(dados[i][4], "");
        strcpy(dados[i][5], "");
        strcpy(dados[i][6], "");
        strcpy(dados[i][7], "");
    }
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    
    /*Laço que lê os dados do arquivo, sendo que os usuários estão separados entre si por quebras de linha,
      e os dados de cada usuário são separados entre si por ponto e vírgula */
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
        /*Para cada usuário j, preenche na matriz "dados" suas informações provenientes do arquivo*/
        if (tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                strcpy(dados[j][i], (*(tokens + i)));
                free(*(tokens + i));
            }
            free(tokens);
        }
        j++;
    }

    int count=0;

    /*Procura na matriz de dados o usuário que possua o email informado, e concatena na sua string de 
      experiências a experiência informada.
    */
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], email) == 0){
            strcpy(dados[i][7], strcat(strcat(dados[i][7], ","), experience));
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    /*Abre o arquivo em modo de escrita*/
    pont_arq = fopen(ARQ_NAME, "w");

    /*Guarda na string output as informações dos usuários, inclusive as informações atualizadas do usuário
      Em que uma nova experiência foi inserida
    */
    for(int i = 0; i < 10; i++){
        if(strcmp(dados[i][0], "") != 0){
            char output[1000];
            sprintf(output,"%s",dados[i][0]);
            strcat(output, ";");
            strcat(output, dados[i][1]);
            strcat(output, ";");
            strcat(output, dados[i][2]);
            strcat(output, ";");
            strcat(output, dados[i][3]);
            strcat(output, ";");
            strcat(output, dados[i][4]);
            strcat(output, ";");
            strcat(output, dados[i][5]);
            strcat(output, ";");
            strcat(output, dados[i][6]);
            strcat(output, ";");
            strcat(output, dados[i][7]);
            strcat(output, ";\n");
            /*Salva os dados novamente no arquivo*/
            r = fputs(output, pont_arq);
        }
    }

    /*Fecha o arquivo*/
    fclose(pont_arq);

    write(src, "\nExperiência adicionada com sucesso!!!\n\n", 41);
}
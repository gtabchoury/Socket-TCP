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
#include <signal.h>
#include <poll.h>

#include "arquivo.h"

#define ARQ_NAME "perfis.txt"

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

void filterByCourse(int src, char course[1000]){
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;
    char dados[10][10][1000];
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
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
    char result[2000]="\nListando perfis com formação em: ";
    strcat(result, course);
    strcat(result, "\n---------------------------------------------------------\n");

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

    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }else{
        write(src, result, strlen(result));
    }
    
}

void filterBySkill(int src, char skill[1000]){
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;
    char dados[10][10][1000];
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
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
    char result[2000]="\nListando perfis com habilidade: ";
    strcat(result, skill);
    strcat(result, "\n---------------------------------------------------------\n");

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

    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }else{
        write(src, result, strlen(result));
    }
}

void filterByGraduateYear(int src, char year[1000]){
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;
    char dados[10][10][1000];
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
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
    char result[2000]="\nListando perfis com ano de formação em: ";
    strcat(result, year);
    strcat(result, "\n---------------------------------------------------------\n");

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
            strcat(result, "\n---------------------------------------------------------\n");
        }
    }

    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }else{
        write(src, result, strlen(result));
    }
}

void listAll(int src){
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;
    char dados[10][10][1000];
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
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
    char result[3000]="\nListando todos os perfis...";
    strcat(result, "\n---------------------------------------------------------\n");

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

    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }else{
        write(src, result, strlen(result));
    }
}

void create(char dados[10][1000]){

}

void filterByEmail(int src, char email[1000]){
    FILE *pont_arq;
    int r;
    pont_arq = fopen(ARQ_NAME, "r");
    char** tokens;
    char dados[10][10][1000];
    char texto_str[1000];
    
    if (pont_arq == NULL){
        printf("Erro ao tentar abrir o arquivo!");
        exit(1);
    }

    int j = 0;
    while(fgets(texto_str, 2000, pont_arq) != NULL){
        tokens = str_split(texto_str, ';');
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
    char result[2000]="\nListando perfil com email: ";
    strcat(result, email);
    strcat(result, "\n---------------------------------------------------------\n");

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

    if (count==0){
        sprintf(result, "%s", "Nenhum perfil encontrado. \n");
        write(src, result, strlen(result));
    }else{
        write(src, result, strlen(result));
    }
}
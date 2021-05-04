#ifndef __ARQUIVO_H__
#define __ARQUIVO_H__

char** str_split(char* a_str, const char a_delim);
void filterByCourse(int src, char course[1000]);
void filterBySkill(int src, char skill[1000]);
void filterByGraduateYear(int src, char year[1000]);
void filterByEmail(int src, char email[1000]);
void listAll(int src);
void create(char dados[10][1000]);

#endif
#ifndef __ARQUIVO_H__
#define __ARQUIVO_H__

char** str_split(char* a_str, const char a_delim);
void filterByCourse(int src, char course[1000]);
void filterBySkill(char dados[10][10][1000]);
void filterByGraduateYear(char dados[10][10][1000]);
void listAll(int src);
void create(char dados[10][1000]);

#endif
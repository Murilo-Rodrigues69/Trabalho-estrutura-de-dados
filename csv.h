#ifndef CSV_H
#define CSV_H

#include <stdio.h>

#define MAX_STR 128
#define FIELDS  27  // número de colunas do CSV

typedef struct {
    int  id_processo;
    char numero_sigilo[MAX_STR];
    char sigla_grau[MAX_STR];
    char procedimento[MAX_STR];
    char ramo_justica[MAX_STR];
    char sigla_tribunal[MAX_STR];
    int  id_tribunal;
    int  recurso;
    int  id_ultimo_oj;
    char dt_recebimento[11];     // YYYY-MM-DD
    int  id_ultima_classe;
    int  flag_violencia_domestica;
    int  flag_feminicidio;
    int  flag_ambiental;
    int  flag_quilombolas;
    int  flag_indigenas;
    int  flag_infancia;
    char decisao[MAX_STR];
    char dt_resolvido[11];       // YYYY-MM-DD
    int  cnm1;
    int  primeirasentm1;
    int  baixm1;
    char decm1[MAX_STR];
    int  mpum1;
    char julgadom1[MAX_STR];
    int  desm1;                  // existe no CSV (antes do susm1)
    int  susm1;
} Processo;

/** Lê o CSV em `path`, aloca vetor dinâmico em `*out` e retorna a quantidade.
 *  Use `free(*out)` ao final. Retorna 0 se não leu nada ou em erro. */
size_t LerArquivo(const char *path, Processo **out);

size_t ContarProcessos(const char *path);

void UltimoOrgaoJulgado(int id,Processo *processos);

void dtRecebimentoMaisAntigo(Processo *processos, int qtd);

size_t ContarProcessosViolenciaDomestica(Processo *processos, int qtd);

size_t ContarProcessosFeminicidio(Processo *processos, int qtd);

size_t ContarProcessosAmbientais(Processo *processos, int qtd);

size_t ContarProcessosQuilombas(Processo *processos, int qtd);

size_t ContarProcessosIndigenas(Processo *processos, int qtd);

size_t ContarProcessosInfancia(Processo *processos, int qtd);

#endif

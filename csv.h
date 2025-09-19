#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include <stddef.h>

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

/* Utilidades em STREAMING (não guarda tudo em memória) */
size_t ContarProcessos(const char *path);

/* Retorna 1 e escreve em *out_oj se encontrar; 0 se não achar o id */
int UltimoOrgaoJulgadoStreaming(const char *path, int id, int *out_oj);

/* Retorna 1 e preenche out_id/out_date (YYYY-MM-DD) se encontrar alguma data válida; 0 caso contrário */
int DtRecebimentoMaisAntigoStreaming(const char *path, int *out_id, char out_date[11]);

size_t ContarFlagViolenciaDomesticaStreaming(const char *path);
size_t ContarFlagFeminicidioStreaming(const char *path);
size_t ContarFlagAmbientalStreaming(const char *path);
size_t ContarFlagQuilombolasStreaming(const char *path);
size_t ContarFlagIndigenasStreaming(const char *path);
size_t ContarFlagInfanciaStreaming(const char *path);

size_t DiferencaDiasPorIdStreaming(const char *path, int id);

double CalcularMeta1AgregadaStreaming(const char *path);

int ExportarProcessosJulgadosMeta1CSV(const char *in_path, const char *out_path, size_t *out_count);
#endif

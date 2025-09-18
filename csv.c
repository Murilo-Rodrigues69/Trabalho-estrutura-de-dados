#include "csv.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void rstrip(char *s){
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

/* Split que preserva campos vazios entre ;; e preenche fields[0..FIELDS-1].
   Retorna o número de campos encontrados (>= FIELDS esperado). */
static int split_semicolon(const char *line, char fields[][MAX_STR]) {
    int col = 0;
    size_t i = 0, j = 0;

    for (int k = 0; k < FIELDS; k++) fields[k][0] = '\0';

    while (line[i] != '\0' && col < FIELDS) {
        if (line[i] == ';') {
            fields[col][j] = '\0';
            col++; j = 0;
            i++;
            continue;
        }
        if (j + 1 < MAX_STR) fields[col][j++] = line[i];
        i++;
    }
    if (col < FIELDS) { fields[col][j] = '\0'; col++; }
    return col;
}

size_t LerArquivo(const char *path, Processo **out) {
    *out = NULL;

    FILE *arquivo = fopen(path, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    char linha[4096];

    // lê e descarta o cabeçalho
    if (!fgets(linha, sizeof(linha), arquivo)) {
        fclose(arquivo);
        return 0;
    }

    Processo *vet = NULL;
    size_t qtd = 0, cap = 0;

    char f[FIELDS][MAX_STR];

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        rstrip(linha);
        if (!linha[0]) continue;

        int cols = split_semicolon(linha, f);
        if (cols < FIELDS) continue; // linha incompleta

        if (qtd == cap) {
            cap = cap ? cap * 2 : 16;
            Processo *tmp = (Processo*)realloc(vet, cap * sizeof(Processo));
            if (!tmp) {
                perror("realloc");
                free(vet);
                fclose(arquivo);
                return 0;
            }
            vet = tmp;
        }

        Processo p = (Processo){0}; // zera tudo

        // números
        p.id_processo              = atoi(f[0]);
        p.id_tribunal              = atoi(f[6]);
        p.recurso                  = atoi(f[7]);
        p.id_ultimo_oj             = atoi(f[8]);
        p.id_ultima_classe         = atoi(f[10]);
        p.flag_violencia_domestica = atoi(f[11]);
        p.flag_feminicidio         = atoi(f[12]);
        p.flag_ambiental           = atoi(f[13]);
        p.flag_quilombolas         = atoi(f[14]);
        p.flag_indigenas           = atoi(f[15]);
        p.flag_infancia            = atoi(f[16]);
        p.cnm1                     = atoi(f[19]);
        p.primeirasentm1           = atoi(f[20]);
        p.baixm1                   = atoi(f[21]);
        p.mpum1                    = atoi(f[23]);
        p.desm1                    = atoi(f[25]);
        p.susm1                    = atoi(f[26]);

        // strings
        if (*f[1])  strncpy(p.numero_sigilo,  f[1],  MAX_STR-1);
        if (*f[2])  strncpy(p.sigla_grau,     f[2],  MAX_STR-1);
        if (*f[3])  strncpy(p.procedimento,   f[3],  MAX_STR-1);
        if (*f[4])  strncpy(p.ramo_justica,   f[4],  MAX_STR-1);
        if (*f[5])  strncpy(p.sigla_tribunal, f[5],  MAX_STR-1);
        if (*f[9])  strncpy(p.dt_recebimento, f[9],  sizeof p.dt_recebimento - 1);
        if (*f[17]) strncpy(p.decisao,        f[17], MAX_STR-1);
        if (*f[18]) strncpy(p.dt_resolvido,   f[18], sizeof p.dt_resolvido - 1);
        if (*f[22]) strncpy(p.decm1,          f[22], MAX_STR-1);
        if (*f[24]) strncpy(p.julgadom1,      f[24], MAX_STR-1);

        vet[qtd++] = p;
    }

    fclose(arquivo);
    *out = vet;
    return qtd;
}

size_t ContarProcessos(const char *path){
    FILE *f = fopen(path, "r");
    if (!f) {
        perror("Erro ao abrir CSV");
        return 0;
    }

    char linha[4096];
    size_t qtd = 0;

    // descarta cabeçalho
    if (!fgets(linha, sizeof(linha), f)) {
        fclose(f);
        return 0;
    }

    // conta quantas linhas tem até o final
    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] != '\n' && linha[0] != '\r') {
            qtd++;
        }
    }

    fclose(f);
    return qtd;
};

void UltimoOrgaoJulgado(int id,Processo *processos){
    for(int i = 0; i<10;i++){
        if(processos[i].id_processo == id ){
            printf("O ultimo orgao que julgou o processo de id : %i foi : %i",id,processos[i].id_ultimo_oj);
        }
    }
};

void dtRecebimentoMaisAntigo(Processo *processos, int qtd) {
    if (qtd <= 0) return;

    int indiceMaisAntigo = 0;

    for (int i = 1; i < qtd; i++) {
        if (strcmp(processos[i].dt_recebimento, processos[indiceMaisAntigo].dt_recebimento) < 0) {
            indiceMaisAntigo = i;
        }
    }

    printf("O processo de id: %d tem a data de recebimento mais antiga: %s\n",
           processos[indiceMaisAntigo].id_processo,
           processos[indiceMaisAntigo].dt_recebimento);
};

size_t ContarProcessosViolenciaDomestica(Processo *processos, int qtd){

    size_t quantidade = 0;
    for(int i = 0;i <qtd;i++){
        if(processos[i].flag_violencia_domestica == 1){
            quantidade++;
        }
    }
    return quantidade;
};

size_t ContarProcessosFeminicidio(Processo *processos, int qtd){
     size_t quantidade = 0;
    for(int i = 0;i <qtd;i++){
        if(processos[i].flag_feminicidio == 1){
            quantidade++;
        }
    }
    return quantidade;
};

size_t ContarProcessosAmbientais(Processo *processos, int qtd){
     size_t quantidade = 0;
    for(int i = 0;i <qtd;i++){
        if(processos[i].flag_ambiental == 1){
            quantidade++;
        }
    }
    return quantidade;
};

size_t ContarProcessosQuilombas(Processo *processos, int qtd){
     size_t quantidade = 0;
    for(int i = 0;i <qtd;i++){
        if(processos[i].flag_quilombolas == 1){
            quantidade++;
        }
    }
    return quantidade;
};

size_t ContarProcessosIndigenas(Processo *processos, int qtd){
    size_t quantidade = 0;
    for(int i = 0;i <qtd;i++){
        if(processos[i].flag_indigenas == 1){
            quantidade++;
        }
    }
    return quantidade;
};

size_t ContarProcessosInfancia(Processo *processos, int qtd){
    size_t quantidade = 0;
    for(int i = 0;i <qtd;i++){
        if(processos[i].flag_infancia == 1){
            quantidade++;
        }
    }
    return quantidade;
};

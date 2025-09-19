#include "csv.h"
#include <string.h>
#include <stdlib.h>

static int parse_ymd(const char *s, int *y, int *m, int *d) {
    if (!s || strlen(s) != 10) return 0;
    return sscanf(s, "%d-%d-%d", y, m, d) == 3;
};

static long days_from_ymd(int y, int m, int d) {
    y -= (m <= 2);
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);
    const unsigned doy = (153u * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;
    return era * 146097 + (int)doe - 719468; // 1970-01-01 como base
};

static void rstrip(char *s){
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
};

/* Split que preserva vazios entre ;; e preenche fields[0..FIELDS-1]. */
static int split_semicolon(const char *line, char fields[][MAX_STR]) {
    int col = 0;
    size_t i = 0, j = 0;

    for (int k = 0; k < FIELDS; k++) fields[k][0] = '\0';

    while (line[i] != '\0' && col < FIELDS) {
        if (line[i] == ';') {
            fields[col][j] = '\0';
            col++; j = 0; i++;
            continue;
        }
        if (j + 1 < MAX_STR) fields[col][j++] = line[i];
        i++;
    }
    if (col < FIELDS) { fields[col][j] = '\0'; col++; }
    return col;
};

size_t ContarProcessos(const char *path){
    FILE *f = fopen(path, "r");
    if (!f) { perror("Erro ao abrir CSV"); return 0; }

    char linha[4096];
    size_t qtd = 0;

    // descarta cabeçalho
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }

    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] != '\n' && linha[0] != '\r') qtd++;
    }

    fclose(f);
    return qtd;
};

int UltimoOrgaoJulgadoStreaming(const char *path, int id, int *out_oj) {
    if (out_oj) *out_oj = 0;

    FILE *f = fopen(path, "r");
    if (!f) { perror("Erro ao abrir CSV"); return 0; }

    char linha[4096];
    char flds[FIELDS][MAX_STR];

    // cabeçalho
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }

    while (fgets(linha, sizeof(linha), f)) {
        rstrip(linha);
        if (!linha[0]) continue;

        if (split_semicolon(linha, flds) < FIELDS) continue;

        int id_proc = atoi(flds[0]);
        if (id_proc == id) {
            int oj = atoi(flds[8]); // id_ultimo_oj é a coluna 8
            if (out_oj) *out_oj = oj;
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0; // não encontrou
};

int DtRecebimentoMaisAntigoStreaming(const char *path, int *out_id, char out_date[11]) {
    if (out_id) *out_id = 0;
    if (out_date) out_date[0] = '\0';

    FILE *f = fopen(path, "r");
    if (!f) { perror("Erro ao abrir CSV"); return 0; }

    char linha[4096];
    char flds[FIELDS][MAX_STR];
    int inicializado = 0;
    char menor[11] = {0};
    int  menor_id = 0;

    // cabeçalho
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }

    while (fgets(linha, sizeof(linha), f)) {
        rstrip(linha);
        if (!linha[0]) continue;

        if (split_semicolon(linha, flds) < FIELDS) continue;

        // dt_recebimento é col 9
        if (flds[9][0] == '\0') continue; // sem data

        if (!inicializado || strcmp(flds[9], menor) < 0) {
            strncpy(menor, flds[9], sizeof(menor)-1);
            menor[sizeof(menor)-1] = '\0';
            menor_id = atoi(flds[0]);
            inicializado = 1;
        }
    }

    fclose(f);

    if (!inicializado) return 0;
    if (out_id) *out_id = menor_id;
    if (out_date) {
        strncpy(out_date, menor, 11);
        out_date[10] = '\0';
    }
    return 1;
};

static size_t contar_flag_col_streaming(const char *path, int col_flag) {
    FILE *f = fopen(path, "r");
    if (!f) { perror("Erro ao abrir CSV"); return 0; }

    char linha[4096];
    char flds[FIELDS][MAX_STR];
    size_t count = 0;

    // cabeçalho
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }

    while (fgets(linha, sizeof(linha), f)) {
        rstrip(linha);
        if (!linha[0]) continue;
        if (split_semicolon(linha, flds) < FIELDS) continue;

        if (atoi(flds[col_flag]) == 1) count++;
    }

    fclose(f);
    return count;
};

// ---- wrappers públicos: uma função por flag ----
size_t ContarFlagViolenciaDomesticaStreaming(const char *path) {
    return contar_flag_col_streaming(path, 11);
};

size_t ContarFlagFeminicidioStreaming(const char *path) {
    return contar_flag_col_streaming(path, 12);
};

size_t ContarFlagAmbientalStreaming(const char *path) {
    return contar_flag_col_streaming(path, 13);
};

size_t ContarFlagQuilombolasStreaming(const char *path) {
    return contar_flag_col_streaming(path, 14);
};

size_t ContarFlagIndigenasStreaming(const char *path) {
    return contar_flag_col_streaming(path, 15);
};

size_t ContarFlagInfanciaStreaming(const char *path) {
    return contar_flag_col_streaming(path, 16);
};

size_t DiferencaDiasPorIdStreaming(const char *path, int id) {
    FILE *f = fopen(path, "r");
    if (!f) { perror("Erro ao abrir CSV"); return 0; }

    char linha[4096];
    char flds[FIELDS][MAX_STR];

    // descarta cabeçalho
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }

    while (fgets(linha, sizeof(linha), f)) {
        rstrip(linha);
        if (!linha[0]) continue;
        if (split_semicolon(linha, flds) < FIELDS) continue;

        int id_proc = atoi(flds[0]);
        if (id_proc == id) {
            const char *d1 = flds[9];   // dt_recebimento
            const char *d2 = flds[18];  // dt_resolvido
            int y1,m1,day1,y2,m2,day2;

            if (!parse_ymd(d1,&y1,&m1,&day1)) { fclose(f); return 0; }
            if (!parse_ymd(d2,&y2,&m2,&day2)) { fclose(f); return 0; }

            long a = days_from_ymd(y1,m1,day1);
            long b = days_from_ymd(y2,m2,day2);
            long diff = (a>b)?(a-b):(b-a);

            fclose(f);
            return (size_t)diff;
        }
    }

    fclose(f);
    return 0; // não encontrado
};

double CalcularMeta1AgregadaStreaming(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { perror("Erro ao abrir CSV"); return 0.0; }

    char linha[4096];
    char flds[FIELDS][MAX_STR];

    long long soma_cnm1 = 0;
    long long soma_desm1 = 0;
    long long soma_susm1 = 0;
    long long soma_julgadom1 = 0;

    // descarta cabeçalho
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0.0; }

    while (fgets(linha, sizeof(linha), f)) {
        rstrip(linha);
        if (!linha[0]) continue;
        if (split_semicolon(linha, flds) < FIELDS) continue;

        soma_cnm1      += atoi(flds[19]); // cnm1
        soma_julgadom1 += atoi(flds[24]); // julgadom1
        soma_desm1     += atoi(flds[25]); // desm1
        soma_susm1     += atoi(flds[26]); // susm1
    }

    fclose(f);

    long long denominador = soma_cnm1 + soma_desm1 - soma_susm1;
    if (denominador <= 0) return 0.0;

    return ((double)soma_julgadom1 / denominador) * 100.0;
};

int ExportarProcessosJulgadosMeta1CSV(const char *in_path, const char *out_path, size_t *out_count) {
    if (out_count) *out_count = 0;

    FILE *fin = fopen(in_path, "r");
    if (!fin) { perror("Erro ao abrir CSV de entrada"); return 0; }

    FILE *fout = fopen(out_path, "w");
    if (!fout) { perror("Erro ao criar CSV de saída"); fclose(fin); return 0; }

    char linha[4096];
    char flds[FIELDS][MAX_STR];

    /* 1) Copia o cabeçalho como está */
    if (!fgets(linha, sizeof(linha), fin)) {
        /* arquivo vazio */
        fclose(fin);
        fclose(fout);
        return 1; /* nada a exportar, mas tecnicamente sucesso */
    }
    fputs(linha, fout);  /* escreve o cabeçalho original */

    /* 2) Filtra linhas por julgadom1 > 0 (coluna 24, 0-based) */
    size_t escritos = 0;
    while (fgets(linha, sizeof(linha), fin)) {
        /* guarda linha original sem o \n? vamos normalizar adicionando \n ao final */
        rstrip(linha);
        if (!linha[0]) continue;

        if (split_semicolon(linha, flds) < FIELDS) continue;

        int julgadom1 = atoi(flds[24]);
        if (julgadom1 > 0) {
            /* reconstroi a linha para garantir final de linha consistente */
            for (int i = 0; i < FIELDS; i++) {
                fputs(flds[i], fout);
                fputc(i == FIELDS - 1 ? '\n' : ';', fout);
            }
            escritos++;
        }
    }

    if (out_count) *out_count = escritos;
    fclose(fin);
    fclose(fout);
    return 1;
};
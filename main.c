#include <stdio.h>
#include <stdlib.h>
#include "csv.h"

int main(void) {
    const char *ARQ = "TJDFT_filtrado.csv";   
    const char *OUT = "processos_julgados_meta1.csv";

    // 1) Contar total de processos (linhas, sem carregar tudo)
    size_t total = ContarProcessos(ARQ);
    printf("Total de processos: %zu\n", total);

    // 2) Último órgão julgado por ID (streaming)
    int id_busca = 323961078;
    int ultimo_oj = 0;
    if (UltimoOrgaoJulgadoStreaming(ARQ, id_busca, &ultimo_oj)) {
        printf("ID %d -> id_ultimo_oj = %d\n", id_busca, ultimo_oj);
    } else {
        printf("ID %d não encontrado.\n", id_busca);
    }

    // 3) Data de recebimento mais antiga (streaming)
    int id_antigo = 0;
    char dt_antiga[11] = {0};
    if (DtRecebimentoMaisAntigoStreaming(ARQ, &id_antigo, dt_antiga)) {
        printf("Mais antigo: id=%d, dt_recebimento=%s\n", id_antigo, dt_antiga);
    } else {
        printf("Nenhuma data de recebimento válida encontrada.\n");
    }

    // 4) Resumo de flags (streaming)
     printf("Violencia Domestica: %zu\n", ContarFlagViolenciaDomesticaStreaming(ARQ));
    printf("Feminicidio:         %zu\n", ContarFlagFeminicidioStreaming(ARQ));
    printf("Ambiental:           %zu\n", ContarFlagAmbientalStreaming(ARQ));
    printf("Quilombolas:         %zu\n", ContarFlagQuilombolasStreaming(ARQ));
    printf("Indigenas:           %zu\n", ContarFlagIndigenasStreaming(ARQ));
    printf("Infancia:            %zu\n", ContarFlagInfanciaStreaming(ARQ));


    int id = 325359695;
    size_t dias = DiferencaDiasPorIdStreaming(ARQ, id);

    if (dias > 0) {
        printf("O processo %d levou %zu dias entre recebimento e resolvido.\n", id, dias);
    } else {
        printf("ID %d não encontrado ou datas inválidas.\n", id);
    }

    double meta1_tribunal = CalcularMeta1AgregadaStreaming(ARQ);

    if (meta1_tribunal > 0.0)
        printf("Cumprimento da Meta 1 (agregado) = %.2f%%\n", meta1_tribunal);
    else
        printf("Não foi possível calcular a Meta 1 agregada (dados inválidos).\n");



        size_t n = 0;
    if (ExportarProcessosJulgadosMeta1CSV(ARQ, OUT, &n)) {
        printf("Gerado '%s' com %zu processos julgados (mérito) na Meta 1.\n", OUT, n);
    } else {
        printf("Falha ao gerar o CSV filtrado.\n");
    }


    return 0;
}

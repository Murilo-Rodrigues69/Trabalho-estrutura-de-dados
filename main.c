#include <stdio.h>
#include <stdlib.h>
#include "csv.h"

int main(void) {
    Processo *processos = NULL;

    // Ajuste o nome/caminho se necessário (por padrão: dados.csv)
    size_t qtd = LerArquivo("TJDFT_amostra.csv", &processos);
    if (!qtd) {
        printf("Nenhum processo lido.\n");
        return 0;
    }

    printf("✅ Foram lidos %zu processos.\n\n", qtd);

    // imprime os 5 primeiros para conferir
   // size_t lim = (qtd < 5) ? qtd : 5;
   // for (size_t i = 0; i < lim; i++) {
   //     printf("Processo %zu\n", i+1);
   //     printf("  ID: %d\n", processos[i].id_processo);
    //    printf("  Procedimento: %s\n", processos[i].procedimento);
    //    printf("  Ramo: %s\n", processos[i].ramo_justica);
    //    printf("  Recebido: %s\n", processos[i].dt_recebimento);
    //    printf("  Resolvido: %s\n", processos[i].dt_resolvido);
    //    printf("  Violência Doméstica: %d\n", processos[i].flag_violencia_domestica);
    //    printf("-------------------------------------\n");
   // }

    size_t quantidadeProcessos = ContarProcessos("TJDFT_amostra.csv");
    printf("Numero de processos: %i",quantidadeProcessos);

    free(processos);

    printf("\n\n");
    UltimoOrgaoJulgado(323961078,processos);
    printf("\n\n");
    dtRecebimentoMaisAntigo(processos, qtd);
    printf("\n\n");
    size_t qtdVD = ContarProcessosViolenciaDomestica(processos, qtd);
    printf("Existem %zu processos de violencia domestica.\n", qtdVD);
    printf("\n\n");
    size_t qtdFe = ContarProcessosFeminicidio(processos,qtd);
    printf("Existem %zu processos de feminicidio.\n",qtdFe);
    printf("\n\n");
    size_t qtdAm = ContarProcessosAmbientais(processos,qtd);
    printf("Existem %zu processos Ambientais.\n",qtdAm);
    printf("\n\n");
    size_t qtdQu = ContarProcessosQuilombas(processos,qtd);
    printf("Existem %zu processos Quilombolas.\n",qtdQu);
    printf("\n\n");
    size_t qtdIn = ContarProcessosIndigenas(processos,qtd);
    printf("Existem %zu processos Indigenas.\n",qtdIn);
    printf("\n\n");
    size_t qtdIi = ContarProcessosInfancia(processos,qtd);
    printf("Existem %zu processos envolvendo infancia e juventude.\n",qtdIi);

    printf("\n\n");

    int id = 323961063;
    size_t dias = DiferencaEntreDtRecibimentoDtResolvido(processos, id, qtd);
    printf("ID %d: diferenca = %zu dias\n", id, dias);

    return 0;
}



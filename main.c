#include "expressao.h"
#include <stdio.h>
#include <string.h>

int main()
{
    Expressao exp;

    printf("==== Avaliador de Expressoes Numericas ====\n\n");

    // 1. Entrada infixa
    printf("Digite a expressao infixa:\n> ");
    fgets(exp.inFixa, 512, stdin);
    exp.inFixa[strcspn(exp.inFixa, "\n")] = 0; // Remove newline

    // 1.1. Converter infixa para pós-fixa e avaliar
    strcpy(exp.posFixa, getFormaPosFixa(exp.inFixa));
    exp.Valor = getValorPosFixa(exp.posFixa);

    printf("\nExpressao em notacao pos-fixa: %s\n", exp.posFixa);
    printf("Valor da expressao: %.4f\n", exp.Valor);

    // 2. Entrada pós-fixa
    printf("\nDigite a expressao pos-fixa:\n> ");
    fgets(exp.posFixa, 512, stdin);
    exp.posFixa[strcspn(exp.posFixa, "\n")] = 0; // Remove newline

    // 2.1. Converter pós-fixa para infixa e avaliar
    strcpy(exp.inFixa, getFormaInFixa(exp.posFixa));
    exp.Valor = getValorPosFixa(exp.posFixa);

    printf("\nExpressao em notacao infixa: %s\n", exp.inFixa);
    printf("Valor da expressao: %.4f\n", exp.Valor);

    printf("\n==== Fim do programa ====\n");

    return 0;
}
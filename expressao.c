#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#define CONST_PI 3.14159265358979323846

typedef struct NumNode
{
    float numero;
    struct NumNode *anterior;
} NumNode;

typedef struct StrNode
{
    char conteudo[512];
    struct StrNode *anterior;
} StrNode;

// Pilha numérica
NumNode *adicionarNumero(NumNode **pilha, float valor)
{
    NumNode *novo = malloc(sizeof(NumNode));
    if (!novo)
        return NULL;
    novo->numero = valor;
    novo->anterior = *pilha;
    return novo;
}

NumNode *removerNumero(NumNode **pilha)
{
    NumNode *removido = *pilha;
    if (*pilha)
        *pilha = (*pilha)->anterior;
    return removido;
}

// Pilha de strings
StrNode *empilharStr(StrNode *topo, const char *texto)
{
    StrNode *novo = malloc(sizeof(StrNode));
    if (!novo)
        return NULL;
    snprintf(novo->conteudo, sizeof(novo->conteudo), "%s", texto);
    novo->anterior = topo;
    return novo;
}

StrNode *desempilharStr(StrNode *topo)
{
    if (!topo)
        return NULL;
    return topo->anterior;
}

int operadorValido(char simb)
{
    return strchr("+-*/%^", simb) != NULL;
}

int prioridadeOperador(char op)
{
    switch (op)
    {
    case '^':
        return 4;
    case '*':
    case '/':
    case '%':
        return 3;
    case '+':
    case '-':
        return 2;
    default:
        return 0;
    }
}

float grausParaRadianos(float graus)
{
    return graus * CONST_PI / 180.0f;
}

char *ajustarSeparadorDecimal(char *entrada)
{
    for (int i = 0; entrada[i]; i++)
        if (entrada[i] == ',')
            entrada[i] = '.';
    return entrada;
}

char operadorPrincipal(const char *expressao)
{
    int nivel = 0;
    for (int i = 0; expressao[i]; i++)
    {
        nivel += expressao[i] == '(';
        nivel -= expressao[i] == ')';
        if (nivel == 0 && operadorValido(expressao[i]))
            return expressao[i];
    }
    return '\0';
}

char *getFormaInFixa(char *entrada)
{
    static char resultado[512];
    StrNode *pilha = NULL;
    char temp[512];
    int i = 0;

    while (entrada[i])
    {
        while (isspace(entrada[i]))
            i++;

        int j = 0;
        while (entrada[i] && !isspace(entrada[i]))
            temp[j++] = entrada[i++];
        temp[j] = '\0';

        if (isdigit(temp[0]) || temp[0] == '.')
        {
            pilha = empilharStr(pilha, temp);
        }
        else if (!strcmp(temp, "sen") || !strcmp(temp, "cos") || !strcmp(temp, "tg") ||
                 !strcmp(temp, "log") || !strcmp(temp, "raiz"))
        {
            StrNode *arg = pilha;
            pilha = desempilharStr(pilha);
            char expressao[512];
            snprintf(expressao, sizeof(expressao), "%s(%s)", temp, arg->conteudo);
            pilha = empilharStr(pilha, expressao);
            free(arg);
        }
        else if (operadorValido(temp[0]) && strlen(temp) == 1)
        {
            char op = temp[0];
            StrNode *dir = pilha;
            pilha = desempilharStr(pilha);
            StrNode *esq = pilha;
            pilha = desempilharStr(pilha);

            char esqF[256], dirF[256];
            int pEsq = prioridadeOperador(operadorPrincipal(esq->conteudo));
            int pDir = prioridadeOperador(operadorPrincipal(dir->conteudo));
            int pAtual = prioridadeOperador(op);

            snprintf(esqF, sizeof(esqF), (pEsq < pAtual) ? "(%s)" : "%s", esq->conteudo);
            snprintf(dirF, sizeof(dirF), (pDir < pAtual || (pDir == pAtual && op != '^')) ? "(%s)" : "%s", dir->conteudo);

            char nova[512];
            snprintf(nova, sizeof(nova), "%s %c %s", esqF, op, dirF);
            pilha = empilharStr(pilha, nova);
            free(esq);
            free(dir);
        }
    }

    snprintf(resultado, sizeof(resultado), "%s", pilha ? pilha->conteudo : "");
    if (pilha)
        free(pilha);
    return resultado;
}

char *getFormaPosFixa(char *entrada)
{
    static char saida[512];
    saida[0] = '\0';
    StrNode *operadores = NULL;
    int i = 0, k = 0;

    for (; entrada[i]; i++)
    {
        if (isspace(entrada[i]))
            continue;

        if (isdigit(entrada[i]) || entrada[i] == '.' || entrada[i] == ',')
        {
            while (isdigit(entrada[i]) || entrada[i] == '.' || entrada[i] == ',')
            {
                saida[k++] = (entrada[i] == ',') ? '.' : entrada[i];
                i++;
            }
            saida[k++] = ' ';
            i--;
        }
        else if (isalpha(entrada[i]))
        {
            char buffer[64];
            int j = 0;
            while (isalpha(entrada[i]))
                buffer[j++] = entrada[i++];
            buffer[j] = '\0';
            operadores = empilharStr(operadores, buffer);
            i--;
        }
        else if (entrada[i] == '(')
        {
            operadores = empilharStr(operadores, "(");
        }
        else if (entrada[i] == ')')
        {
            while (operadores && strcmp(operadores->conteudo, "("))
            {
                k += sprintf(&saida[k], "%s ", operadores->conteudo);
                operadores = desempilharStr(operadores);
            }
            if (operadores)
                operadores = desempilharStr(operadores);
            if (operadores && isalpha(operadores->conteudo[0]))
            {
                k += sprintf(&saida[k], "%s ", operadores->conteudo);
                operadores = desempilharStr(operadores);
            }
        }
        else if (operadorValido(entrada[i]))
        {
            char atual[2] = {entrada[i], '\0'};
            while (operadores && operadorValido(operadores->conteudo[0]) &&
                   prioridadeOperador(operadores->conteudo[0]) >= prioridadeOperador(atual[0]))
            {
                k += sprintf(&saida[k], "%s ", operadores->conteudo);
                operadores = desempilharStr(operadores);
            }
            operadores = empilharStr(operadores, atual);
        }
    }

    while (operadores)
    {
        k += sprintf(&saida[k], "%s ", operadores->conteudo);
        operadores = desempilharStr(operadores);
    }

    saida[k] = '\0';
    return saida;
}

float getValorPosFixa(char *posfixa)
{
    NumNode *pilha = NULL;
    char temp[64];
    int i = 0;

    while (posfixa[i])
    {
        if (isspace(posfixa[i]))
        {
            i++;
            continue;
        }

        if (isdigit(posfixa[i]) || posfixa[i] == '.' || posfixa[i] == ',')
        {
            int j = 0;
            while (isdigit(posfixa[i]) || posfixa[i] == '.')
                temp[j++] = posfixa[i++];
            temp[j] = '\0';
            pilha = adicionarNumero(&pilha, atof(temp));
        }
        else if (isalpha(posfixa[i]))
        {
            int j = 0;
            while (isalpha(posfixa[i]))
                temp[j++] = posfixa[i++];
            temp[j] = '\0';

            NumNode *topo = removerNumero(&pilha);
            if (!topo)
                return 0.0f;

            float a = topo->numero;
            free(topo);

            float r = 0.0f;
            if (!strcmp(temp, "sen"))
                r = sinf(grausParaRadianos(a));
            else if (!strcmp(temp, "cos"))
                r = cosf(grausParaRadianos(a));
            else if (!strcmp(temp, "tg"))
                r = tanf(grausParaRadianos(a));
            else if (!strcmp(temp, "log"))
                r = log10f(a);
            else if (!strcmp(temp, "raiz"))
                r = sqrtf(a);

            pilha = adicionarNumero(&pilha, r);
        }
        else if (operadorValido(posfixa[i]))
        {
            char op = posfixa[i++];
            NumNode *b = removerNumero(&pilha);
            NumNode *a = removerNumero(&pilha);
            if (!a || !b)
                return 0.0f;

            float res = 0.0f;
            switch (op)
            {
            case '+':
                res = a->numero + b->numero;
                break;
            case '-':
                res = a->numero - b->numero;
                break;
            case '*':
                res = a->numero * b->numero;
                break;
            case '/':
                res = a->numero / b->numero;
                break;
            case '%':
                res = fmodf(a->numero, b->numero);
                break;
            case '^':
                res = powf(a->numero, b->numero);
                break;
            }

            free(a);
            free(b);
            pilha = adicionarNumero(&pilha, res);
        }
        else
        {
            i++;
        }
    }

    float final = pilha ? pilha->numero : 0.0f;
    if (pilha)
        free(pilha);
    return final;
}

float getValorInFixa(char *entrada)
{
    return getValorPosFixa(getFormaPosFixa(ajustarSeparadorDecimal(entrada)));
}

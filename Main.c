#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include "meuconio.h"
#include "Structs_FuncoesGeral.h"
#define TF 200

//----------INCLUDES--------------

typedef struct TpNoArg
{
    Campos *Campo;
} TpNoArg;

typedef struct TpNoDado
{
    char TipoDados;
    TipoValor Tipo;
} TpNoDado;

typedef union Nos
{
    TpNoDado Dado;
    TpNoArg Arg;
} Nos;

typedef struct Fila
{
    union Nos Nos;
    struct Fila *prox;
} Fila;

//----------ESTRUTURAS------------------

void enqueue(Fila **F1, Nos Dado);
void dequeue(Fila **F1, Nos *Dado);
void init(Fila **F1);
char isEmpty(Fila *F1);
void lerPalavra(char frase[TF], char aux[TF]);
char lerInstrucao(char str[TF]);
void lerColuna(char frase[TF], char aux[TF]);
char validarColunas(Fila *F1);
char lerArgs(BancoDados *bd, Fila **F1, char frase[TF], char tabela[TF]);
char lerDados(BancoDados *bd, Fila *F1, char Tab[TF], char str[TF], Fila **F2);
char validarTabela(BancoDados *bd, char frase[TF], char tabela[TF]);
void insert(BancoDados *bd, char aux[TF]);
void lerAteWhere(char frase[TF],char colval[TF]);
void converteDado(char tipoDados,char valor[TF],TipoValor *dado);
void lerSet(Fila **F1,Fila **F2, Tabela *auxTab,char frase[TF]);
void update(BandoDados *bd,char frase[TF]);
void lerWhere();
void delete();
void destruir(Fila **F1);
void selectAll();


void enqueue(Fila **F1, Nos Dado)
{
    Fila *aux = *F1;
    Fila *novo = malloc(sizeof(Fila));
    novo->Nos = Dado;
    novo->prox = NULL;

    if (*F1 == NULL)
        *F1 = novo;
    else
    {
        while (aux->prox != NULL)
            aux = aux->prox;
        aux->prox = novo;
    }
}

void dequeue(Fila **F1, Nos *Dado)
{
    Fila *aux = *F1;
    *Dado = aux->Nos;
    *F1 = aux->prox;
    free(aux);
}

void init(Fila **F1)
{
    *F1 = NULL;
}

char isEmpty(Fila *F1)
{
    return F1 == NULL;
}

void destruir(Fila **F1)
{
    if(*F1 != NULL)
    {
        destruir(&(*F1)->prox);
        free(*F1);
        *F1 = NULL;
    }
}

void lerPalavra(char frase[TF], char aux[TF])
{
    int i = 0, j = 0;
    if (frase[i] == '(')
    {
        while (frase[i + 1] != ')' && frase[i] != '\0')
        {
            aux[i] = frase[i + 1];
            i++;
        }
        aux[i] = '\0';
        i = i + 2;
        while(frase[i] == ' ')
            i++;
        while (frase[i] != '\0')
        { 
            frase[j] = frase[i];
            i++;
            j++;
        }
        frase[j] = '\0';

    }
    else
    {
        while (frase[i] != ' ' && frase[i] != '\n' && frase[i] != '\0')
        {
            aux[i] = frase[i];
            i++;
        }

        aux[i] = '\0';

        while(frase[i] == ' ')
            i++;

        while (frase[i] != '\0')
        {
            frase[j] = frase[i];
            i++;
            j++;
        }

        frase[j] = '\0';
    }
}

char lerInstrucao(char str[TF])
{
    char aux[TF];
    int i = 0;
    lerPalavra(str, aux);
    if (strcmp(aux, "INSERT") == 0)
    {
        lerPalavra(str, aux);
        if (strcmp(aux, "INTO") == 0)
        {
            return 'I';
        }
        printf("\nComando '%s' invalido!\n", aux);
        return 'N';
    }
    else if (strcmp(aux, "UPDATE") == 0)
    {
        return 'U';
    }
    else if (strcmp(aux, "DELETE") == 0)
    {
        lerPalavra(str, aux);
        if (strcmp(aux, "FROM") == 0)
        {
            return 'D';
        }
    }
    else if (strcmp(aux, "SELECT") == 0)
    {
        lerPalavra(str, aux);
        if (strcmp(aux, "*") == 0)
        {
            return '*';
        }
        while (aux[i] != '\0')
            i++;
        aux[i] = ' ';
        aux[i + 1] = '\0';
        strcat(aux, str);
        strcpy(str, aux);
        return 'S';
    }
    printf("\nComando '%s' invalido!\n", aux);
    return 'N';
}

void lerColuna(char frase[TF], char aux[TF])
{
    int i = 0, j = 0;

    while (frase[i] != ',' && frase[i] != '\0')
    {
        aux[i] = frase[i];
        i++;
    }

    aux[i] = '\0';
    if (frase[i] != '\0')
    {
        if (frase[i + 1] == ' ')
            i = i + 2;
        else if (frase[i] == ',')
            i++;
    }

    while (frase[i] != '\0')
    {
        frase[j] = frase[i];
        i++;
        j++;
    }

    frase[j] = '\0';
}

char validarColunas(Fila *F1)
{
    int flag = 0;
    while (F1 != NULL)
    {
        if (F1->Nos.Arg.Campo == NULL)
            flag = 1;
        F1 = F1->prox;
    }
    return flag == 0;
}

char lerArgs(BancoDados *bd, Fila **F1, char frase[TF], char tabela[TF])
{
    Tabela *auxTab;
    Campos *auxCampo;
    auxTab = buscarTabela(bd->pTabelas, tabela);
    Nos Dado;
    Fila *F2;
    init(&F2);
    int i = 1;
    char aux[TF], colunas[TF];
    lerPalavra(frase, aux);
    if (aux[0] != ' ' && aux[0] != '\0' && aux[0] != '\n')
    {
        strcpy(colunas, aux);
        while (colunas[0] != '\0')
        {
            lerColuna(colunas, aux);
            auxCampo = buscarCampo(auxTab->pCampos, aux);
            Dado.Arg.Campo = auxCampo;
            enqueue(&F2, Dado);
        }
        if (validarColunas(F2))
        {
            while (!isEmpty(F2))
            {
                dequeue(&F2, &Dado);
                enqueue(&*F1, Dado);
            }
            return 1;
        }
        else
            return 0;
    }
    return 0;
}

char lerDados(BancoDados *bd, Fila *F1, char Tab[TF], char str[TF], Fila **F2)
{
    int i;
    float f;
    Tabela *auxTab;
    Campos *auxCamp, *auxCampo;
    Nos Dados;
    char aux[TF],valores[TF];
    auxTab = buscarTabela(bd->pTabelas, Tab);
    auxCamp = auxTab->pCampos;
    lerPalavra(str, aux);
    if (strcmp(aux, "VALUES") == 0)
    {
        lerPalavra(str, aux);
        strcpy(valores,aux);
        if (isEmpty(F1))
        {
            while (auxCamp != NULL)
            {
                lerColuna(valores,aux);
                Dados.Dado.TipoDados = auxCamp->tipo;
                converteDado(Dados.Dado.TipoDados,aux,&Dados.Dado.Tipo);
                if (aux[0] == ' ' || aux[0] == '\0' || aux[0] == '\n')
                {
                    printf("\nQuantidade de argumentos insuficientes!\n");
                    return 0;
                }
                enqueue(&*F2, Dados);
                auxCamp = auxCamp->prox;
            }
            lerPalavra(str, aux);
            if (aux[0] != ' ' && aux[0] != '\0' && aux[0] != '\n')
            {
                printf("\nQuantidade de argumentos excedentes!\n");
                return 0;
            }
            return 1;
        }
        else
        {
            while (F1 != NULL)
            {
                auxCampo = buscarCampo(auxCamp, F1->Nos.Arg.Campo->campo);
                if (auxCampo->tipo == F1->Nos.Arg.Campo->tipo)
                {
                    lerColuna(valores,aux);
                    converteDado(Dados.Dado.TipoDados,aux,&Dados.Dado.Tipo);
                    if (aux[0] == ' ' || aux[0] == '\0' || aux[0] == '\n')
                    {
                        printf("\nQuantidade de argumentos insuficientes!\n");
                        return 0;
                    }
                    enqueue(&*F2, Dados);
                    F1 = F1->prox;
                }
                else
                {
                    printf("\nTipagem de dado incorreta para a coluna mencionada!\n");
                    return 0;
                }
            }
            lerPalavra(str, aux);
            if (aux[0] != ' ' && aux[0] != '\0' && aux[0] != '\n')
            {
                printf("\nQuantidade de argumentos excedentes!\n");
                return 0;
            }
            return 1;
        }
    }
    else
    {
        printf("\nComando '%s' invalido!\n", aux);
        return 0;
    }
}

char validarTabela(BancoDados *bd, char frase[TF], char tabela[TF])
{
    Tabela *auxTab;
    char aux[TF];
    int i = 0;
    lerPalavra(frase, aux);
    auxTab = buscarTabela(bd->pTabelas, aux);
    if (auxTab != NULL)
        while (auxTab->tabela[i] != '\0')
        {
            tabela[i] = auxTab->tabela[i];
            i++;
        }
    tabela[i] = '\0';
    return auxTab != NULL;
}

void insert(BancoDados *bd, char aux[TF])
{
    char tabela[TF];
    Fila *F1, *F2;
    Nos auxDados;
    Valor *novoValor;
    init(&F1);
    init(&F2);
    if (validarTabela(bd, aux, tabela))
    {
        if (lerArgs(bd, &F1, aux, tabela))
        {
            if (lerDados(bd, F1, tabela, aux, &F2))
            {
                    while (!isEmpty(F2))
                    {
                        novoValor = criarValor(F2->Nos.Dado.Tipo);
                        inserirValor(F1->Nos.Arg.Campo, novoValor);
                        if (!isEmpty(F1))
                            dequeue(&F1, &auxDados);
                        dequeue(&F2, &auxDados);
                    }
                    printf("\nValores inseridos com sucesso!\n");
            }
        }
        else
            printf("\nColunas Invalidas!\n");
    }
    else
        printf("Tabela Invalida!");
}

void lerAteWhere(char frase[TF],char colval[TF])
{
    char aux[TF];
    int i;
    colval[0] = '\0';
    lerPalavra(frase,aux);
    while(strcmp(aux,"WHERE") != 0 && aux[0] != '\0')
    {
        strcat(colval,aux);
        strcat(colval, " ");
        lerPalavra(frase,aux); 
    } //WHERE ja consumido
    i = strlen(colval);
    if(colval[i-1] == ' ')
        colval[i-1] = '\0';
}

void converteDado(char tipoDados,char valor[TF],TipoValor *dado)
{
    int i;
    float f;
    switch (tipoDados)
                    {
                    case 'I':
                        i = atoi(valor);
                        dado->valorI = i;
                        break;
                    case 'N':
                        f = atof(valor);
                        dado->valorF = f;
                        break;
                    case 'D':
                        strcpy(dado->valorD,valor);
                        break;
                    case 'C':
                        dado->valorC = valor[0];
                        break;
                    case 'T':
                        strcpy(dado->valorT,valor);
                        break;
                    }
}

void lerSet(Fila **F1,Fila **F2, Tabela *auxTab,char frase[TF])
{
    char auxSet[TF],aux[TF];
    Nos dado,coluna;
    lerPalavra(frase,aux); //consumir SET
    lerAteWhere(frase,auxSet);
    lerPalavra(auxSet,aux);
    while(aux[0] != '\0')
    {
        coluna.Arg.Campo = buscarCampo(auxTab->pCampos,aux);
        enqueue(F1, coluna);
        lerPalavra(auxSet,aux); //ler '='
        dado.Dado.TipoDados = coluna.Arg.Campo->tipo; 
        lerColuna(auxSet,aux);
        converteDado(dado.Dado.TipoDados,aux,&dado.Dado.Tipo);
        enqueue(F2,dado);
        lerPalavra(auxSet,aux);
    }

}

void lerWhere(Campos **condCampo, Tabela *Tab, char *modo, TipoValor *valor, TipoValor *valorIni, TipoValor *valorFin, char frase[TF])
{
    char aux[TF];

    *modo = 'N';

    if (frase[0] != '\0')
    {
        lerPalavra(frase, aux);
        *condCampo = buscarCampo(Tab->pCampos, aux);

        lerPalavra(frase, aux);
        if (strcmp(aux, "=") == 0) 
            *modo = '=';
        else if (strcmp(aux, "!=") == 0) 
            *modo = '!';
        else if (strcmp(aux, ">") == 0) 
            *modo = '>';
        else if (strcmp(aux, "<") == 0) 
            *modo = '<';
        else if (strcmp(aux, ">=") == 0) 
            *modo = 'M';
        else if (strcmp(aux, "<=") == 0) 
            *modo = 'm';
        else if (strcmp(aux, "BETWEEN") == 0) 
            *modo =   'B';

        if (*modo != 'B')
        {
            lerPalavra(frase, aux);
            converteDado((*condCampo)->tipo, aux,   valor);
        }
        else
        {
            lerPalavra(frase, aux);
            converteDado((*condCampo)->tipo, aux,   valorIni);
            lerPalavra(frase, aux);   //consome AND
            lerPalavra(frase, aux);
            converteDado((*condCampo)->tipo, aux,   valorFin);
        }
    }
}

void update(BancoDados *bd,char frase[TF])
{
    Fila *F1,*F2,*Fcol,*Fval;
    init(&F1);
    init(&F2);
    Nos col;
    Tabela *auxTab;
    Campos *campoCond, *campoPk;
    Valor *linhasPk,*valorAtual,*valorTroca;
    char tabela[TF],aux[TF],modo;
    TipoValor valorCond,valorIni,valorFin;
    int linha = 1;

    lerPalavra(frase,tabela);
    auxTab = buscarTabela(bd->pTabelas,tabela);
    
    lerSet(&F1,&F2,auxTab,frase);
    lerWhere(&campoCond,auxTab,&modo,&valorCond,&valorIni,&valorFin,frase);

    campoPk = buscarCampoPK(auxTab->pCampos);
    linhasPk = campoPk->pDados;
    while(linhasPk != NULL)
    {  
        valorAtual = buscarValorPorIndice(campoCond->pDados,linha);
        if(modo == 'N' || compararValor(valorAtual,campoCond->tipo,modo,valorCond,valorIni,valorFin))
        {
            Fcol = F1;
            Fval = F2;
            while(Fcol != NULL)
            {
                valorTroca = buscarValorPorIndice(Fcol->Nos.Arg.Campo->pDados,linha);
                atualizarValor(valorTroca,&Fval->Nos.Dado.Tipo);
                Fcol = Fcol->prox;
                Fval = Fval->prox;
            }
        }
        linhasPk = linhasPk->prox;
        linha++;
    }
    printf("\nUPDATE concluido!\n");
    destruir(&F1);
    destruir(&F2);
}


void delete(BancoDados *bd,char frase[TF])
{
    Tabela *auxTab;
    char aux[TF],modo;
    Campos *condCampo,*campoPk,*colunaPk;
    Valor *linhaPk,*valorCond,*valorAtual;
    TipoValor valor,valorIni,valorFin;
    int linha = 1;

    lerPalavra(frase,aux);
    auxTab = buscarTabela(bd->pTabelas,aux);
    lerPalavra(frase,aux); //consome WHERE
    lerWhere(&condCampo,auxTab,&modo,&valorCond,&valorIni,&valorFin,frase);
    
    campoPk = buscarCampoPk(auxTab->pCampos);
    linhaPk = campoPk->pDados;
    while(linhaPk != NULL)
    {
        valorAtual = buscarValorPorIndice(condCampo->pDados,linha);
        if(modo == 'N' || compararValor(valorAtual,condCampo->tipo,modo,valorCond,valorIni,valorFin))
        {
            colunaPk = auxTab->pCampos;
            while(colunaPk != NULL)
            {
                removerValorPorIndice(colunaPk,linha);
                colunaPk = colunaPk->prox;
            }

        }
        else
            linha++;
        linhaPk = linhaPk->prox;
    }
    printf("\nDelete concluido!\n");
}

void selectAll()
{

}


void executar(BancoDados *bd)
{
    char str[TF], instrucao, tecla = 'S';
    gets(str);
    instrucao = lerInstrucao(str);

    do
    {
        switch (instrucao)
        {
        case 'I':
            insert(bd, str);
            break;
        case 'U':
            update(bd, str);
            break;
        case 'D':
            Delete(bd, str);
            break;
        case 'S':
            Select(bd, str);
            break;
        case '*':
            SelectALL(bd, str);
            break;
        case 'N':
            break;
        }
        getch(tecla);
        gets(str);
        instrucao = lerInstrucao(str);
    } while (tecla != 27);
}


int main()
{

}
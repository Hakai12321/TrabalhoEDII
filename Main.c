#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include "Structs_FuncoesGeral.h"
#define TF 200
#define TAM_SCRIPT 17000

//----------INCLUDES--------------

typedef struct TpNoArg
{
    Campos *Campo;
    Tabela *Tab;
} TpNoArg;

typedef struct TpNoDado
{
    char TipoDados;
    struct Campos *pk,*fk;
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
void destruir(Fila **F1);

void lerPalavra(char frase[TF], char aux[TF]);
char lerInstrucao(char str[TF]);
void lerColuna(char frase[TF], char aux[TF]);
char validarColunas(Fila *F1);
char validarTabela(BancoDados *bd, char frase[TF], char tabela[TF]);
void converteDado(char tipoDados,char valor[TF],TipoValor *dado);

char lerArgs(BancoDados *bd, Fila **F1, char frase[TF], char tabela[TF]);
char lerDados(BancoDados *bd, Fila *F1, char Tab[TF], char str[TF], Fila **F2);
void lerWhere(Campos **condCampo, Tabela *Tab, char *modo, TipoValor *valor, TipoValor *valorIni, TipoValor *valorFin, char frase[TF]);
void lerAteWhere(char frase[TF],char colval[TF]);

void insert(BancoDados *bd, char aux[TF]);
void lerSet(Fila **F1,Fila **F2, Tabela *auxTab,char frase[TF]);

void update(BancoDados *bd,char frase[TF]);
void delete(BancoDados *bd,char frase[TF]);
void select(BancoDados *bd,char frase[TF]);

void imprimirValor(char campo[TF],Valor* valor, char tipo,int linha);
char contemPonto(char frase[TF]);
void exibirDados(Tabela *auxTab,Fila *F1,Fila *F2,int qtde,Campos *condCampo,char modo,TipoValor valor,TipoValor valorIni,TipoValor valorFin,char where,Campos *Fk,Campos *Pk);
void lerPonto(char frase[TF],char tab[TF],char col[TF]);
void wherePonto(BancoDados *bd,Campos **Fk,Campos **Pk,char frase[TF]);
void LerBloco(char *frase, char *aux);
void LerItemDDL(char *frase, char *aux);
void LerComandoDDL(char *frase, char *comando);
char ConverterTipoSQL(char *tipoSQL);
void ProcessarConstraintPK(Tabela *tab, char *item);
void ProcessarCreateTable(BancoDados *bd, char *comando);
void ProcessarAlterTable(BancoDados *bd, char *comando);
void ProcessarComandoDDL(BancoDados **bd, char *comando);
BancoDados* LerScript(char *nomeArquivo);


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
    int i = 0, j = 0, aspas = 0;

    while (frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t')
        i++;

    if (frase[i] == '(')
    {
        i++;
        while (frase[i] != '\0')
        {
            if (frase[i] == '\'')
                aspas = !aspas;

            if (frase[i] == ')' && aspas == 0)
                break;

            aux[j++] = frase[i];
            i++;
        }
        aux[j] = '\0';

        if (frase[i] == ')')
            i++;

        while (frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t')
            i++;

        j = 0;
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
        while (frase[i] != '\0')
        {
            if (frase[i] == '\'')
                aspas = !aspas;

            if (aspas == 0 && (frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t' || frase[i] == ';' || frase[i] == ','))
                break;

            aux[j++] = frase[i];
            i++;
        }

        aux[j] = '\0';

        while (frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t' || frase[i] == ',' || frase[i] == ';')
            i++;

        j = 0;
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
        return 'S';
    }
    printf("\nComando '%s' invalido!\n", aux);
    return 'N';
}

void lerColuna(char frase[TF], char aux[TF])
{
    int i = 0, j = 0, aspas = 0;

    while (frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t')
        i++;

    while (frase[i] != '\0')
    {
        if (frase[i] == '\'')
            aspas = !aspas;

        if (aspas == 0 && (frase[i] == ',' || frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t'))
            break;

        aux[j++] = frase[i];
        i++;
    }

    aux[j] = '\0';

    while (frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t')
        i++;

    if (frase[i] == ',')
        i++;
    else if (frase[i] == ';')
        i++;

    while (frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t')
        i++;

    j = 0;
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
    Tabela *auxTab;
    Campos *auxCamp, *auxCampo;
    Nos Dados;
    char aux[TF],valores[TF],flag = 1;
    auxTab = buscarTabela(bd->pTabelas, Tab);
    auxCamp = auxTab->pCampos;
    lerPalavra(str, aux);
    if (strcmp(aux, "VALUES") == 0)
    {
        lerPalavra(str, aux);
        strcpy(valores,aux);
        if (isEmpty(F1))
        {
            while (auxCamp != NULL && flag)
            {
                lerColuna(valores,aux);
                Dados.Dado.TipoDados = auxCamp->tipo;
                converteDado(Dados.Dado.TipoDados,aux,&Dados.Dado.Tipo);
                if (aux[0] == ' ' || aux[0] == '\0' || aux[0] == '\n')
                {
                    printf("\nQuantidade de argumentos insuficientes!\n");
                    flag = 0;
                }
                if(flag)
                {
                    enqueue(&*F2, Dados);
                    auxCamp = auxCamp->prox;
                }
            }
            lerPalavra(str, aux);
            if (aux[0] != ' ' && aux[0] != '\0' && aux[0] != '\n')
            {
                printf("\nQuantidade de argumentos excedentes!\n");
                flag = 0;
            }
        }
        else
        {
            while (F1 != NULL && flag)
            {
                auxCampo = buscarCampo(auxCamp, F1->Nos.Arg.Campo->campo);
                if (auxCampo->tipo == F1->Nos.Arg.Campo->tipo)
                {
                    lerColuna(valores,aux);
                    Dados.Dado.TipoDados = auxCampo->tipo;
                    converteDado(Dados.Dado.TipoDados,aux,&Dados.Dado.Tipo);
                    if (aux[0] == ' ' || aux[0] == '\0' || aux[0] == '\n')
                    {
                        printf("\nQuantidade de argumentos insuficientes!\n");
                        flag = 0;
                    }
                    if(flag)
                    {
                        enqueue(&*F2, Dados);
                        F1 = F1->prox;
                    }
                }
                else
                {
                    printf("\nTipagem de dado incorreta para a coluna mencionada!\n");
                    flag = 0;
                }
            }
            lerPalavra(str, aux);
            if (aux[0] != ' ' && aux[0] != '\0' && aux[0] != '\n')
            {
                printf("\nQuantidade de argumentos excedentes!\n");
                flag = 0;
            }
        }
    }
    else
    {
        printf("\nComando '%s' invalido!\n", aux);
        flag = 0;
    }

    return flag==1;
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
                    printf("\n***Valores inseridos com sucesso!***\n");
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
    int i, j = 0;
    float f;
    char valorLimpo[TF];

    while (valor[j] == ' ' || valor[j] == '\n' || valor[j] == '\r' || valor[j] == '\t')
        j++;

    if (valor[j] == '\'' && valor[strlen(valor) - 1] == '\'')
    {
        int k = j + 1;
        int l = 0;
        while (valor[k] != '\0' && valor[k] != '\'')
            valorLimpo[l++] = valor[k++];
        valorLimpo[l] = '\0';
        strcpy(valor, valorLimpo);
    }

    switch (tipoDados)
                    {
                    case 'I':
                        i = atoi(valor);
                        dado->valorI = i;
                        break;
                    case 'N':
                        f = atof(valor);
                        dado->valorN = f;
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
        lerPalavra(frase,aux);
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
            converteDado((*condCampo)->tipo, aux,valor);
        }
        else
        {
            lerPalavra(frase, aux);
            converteDado((*condCampo)->tipo, aux,valorIni);
            lerPalavra(frase, aux);   //consome AND
            lerPalavra(frase, aux);
            converteDado((*condCampo)->tipo, aux,valorFin);
        }
}

void update(BancoDados *bd,char frase[TF])
{
    Fila *F1,*F2,*Fcol,*Fval;
    init(&F1);
    init(&F2);
    Tabela *auxTab;
    Campos *campoCond, *campoPk;
    Valor *linhasPk,*valorAtual,*valorTroca;
    char tabela[TF],modo;
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
    printf("\n***UPDATE concluido!***\n");
    destruir(&F1);
    destruir(&F2);
}


void delete(BancoDados *bd,char frase[TF])
{
    Tabela *auxTab;
    char aux[TF],modo;
    Campos *condCampo,*campoPk,*colunaPk;
    Valor *valorAtual;
    TipoValor valor,valorIni,valorFin;
    int linha = 1;

    lerPalavra(frase,aux);
    auxTab = buscarTabela(bd->pTabelas,aux);
    lerPalavra(frase,aux); //consome WHERE
    lerWhere(&condCampo,auxTab,&modo,&valor,&valorIni,&valorFin,frase);
    
    campoPk = buscarCampoPK(auxTab->pCampos);
    while(buscarValorPorIndice(campoPk->pDados,linha))
    {
        valorAtual = buscarValorPorIndice(condCampo->pDados,linha);
        if(modo == 'N' || compararValor(valorAtual,condCampo->tipo,modo,valor,valorIni,valorFin))
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
    }
    printf("\n***Delete concluido!***\n");
}

char contemPonto(char frase[TF])
{
    int i = 0;

    while(frase[i] != '\0')
    {
        if(frase[i] == '.')
            return 1;

        i++;
    }

    return 0;
}


void imprimirValor(char campo[TF], Valor *valor, char tipo, int linha)
{
    if(valor == NULL || linha < 1)
    {
        printf("Erro!!\n");
        return;
    }

    while(valor != NULL && linha > 1)
    {
        valor = valor->prox;
        linha--;
    }

    if(valor == NULL)
    {
        printf("%s: null\n", campo);
        return;
    }

    switch(toupper(tipo))
    {
        case 'I':
            printf("%s: %d\n", campo, valor->valor.valorI);
            break;

        case 'N':
            printf("%s: %.2f\n", campo, valor->valor.valorN);
            break;

        case 'D':
            printf("%s: %s\n", campo, valor->valor.valorD);
            break;

        case 'C':
            printf("%s: %c\n", campo, valor->valor.valorC);
            break;

        case 'T':
            printf("%s: %s\n", campo, valor->valor.valorT);
            break;
    }
}

void exibirDados(Tabela *auxTab, Fila *F1, Fila *F2, int qtde,Campos *condCampo, char modo, TipoValor valor,TipoValor valorIni, TipoValor valorFin, char where,Campos *Fk, Campos *Pk)
{
    Valor *valorAtual,*valorPk,*valorFk;
    Campos *campoPk,*campo;
    Fila *FCol,*FTab;;

    int linha=1,linhaFk,linhaPk=1,nLin=1,i;

    if(where == 'N')
    {
        campoPk = buscarCampoPK(auxTab->pCampos);
        printf("\n----------*** %s ***----------\n", auxTab->tabela);

        valorAtual = campoPk->pDados;
        while(valorAtual != NULL)
        {
            printf("---------LINHA %d----------\n", linha);
            FCol = F1;
            while(FCol != NULL)
            {
                imprimirValor(FCol->Nos.Arg.Campo->campo,FCol->Nos.Arg.Campo->pDados,FCol->Nos.Arg.Campo->tipo,linha);
                FCol = FCol->prox;
            }
            valorAtual = valorAtual->prox;
            linha++;
        }
    }
    else if(where == 'W')
    {
        printf("\n----------*** %s ***----------\n", auxTab->tabela);
        while(buscarValorPorIndice(condCampo->pDados, linha) != NULL)
        {
            valorAtual = buscarValorPorIndice(condCampo->pDados, linha);

            if(compararValor(valorAtual,condCampo->tipo,modo,valor,valorIni,valorFin))
            {
                printf("---------LINHA %d----------\n", linha);
                FCol = F1;
                while(FCol != NULL)
                {
                    imprimirValor(FCol->Nos.Arg.Campo->campo,FCol->Nos.Arg.Campo->pDados,FCol->Nos.Arg.Campo->tipo,linha);
                    FCol = FCol->prox;
                }
            }
            linha++;
        }
    }
    else if(where == '.')
    {
        printf("\n-----***%s***-----***%s***-----\n",F2->Nos.Arg.Tab->tabela,F2->prox->Nos.Arg.Tab->tabela);
        valorPk = Pk->pDados;
        while(valorPk != NULL)
        {
            linhaFk = 1;
            valorFk = Fk->pDados;
            while(valorFk != NULL)
            {
                if(compararValor(valorPk,Pk->tipo,'=',valorFk->valor,valorIni,valorFin))
                {
                    printf("---------LINHA %d----------\n",nLin);

                    FCol = F1;
                    FTab = F2;

                    i = 0;

                    while(i < qtde)
                    {
                        campo = FTab->Nos.Arg.Tab->pCampos;
                        while(campo != NULL && campo != Pk)
                            campo = campo->prox;
                        if(campo == Pk)
                        {
                            imprimirValor(FCol->Nos.Arg.Campo->campo,FCol->Nos.Arg.Campo->pDados,FCol->Nos.Arg.Campo->tipo,linhaPk);
                        }
                        else
                        {
                            imprimirValor(FCol->Nos.Arg.Campo->campo,FCol->Nos.Arg.Campo->pDados,FCol->Nos.Arg.Campo->tipo,linhaFk);
                        }
                        FCol = FCol->prox;
                        FTab = FTab->prox;
                        i++;
                    }
                    nLin++;
                }
                valorFk = valorFk->prox;
                linhaFk++;
            }
            valorPk = valorPk->prox;
            linhaPk++;
        }
    }
    printf("+----------------------------------------+\n");
}

void lerPonto(char frase[TF], char tab[TF], char col[TF])
{
    char aux[TF];
    int i = 0;
    int j = 0;

    lerPalavra(frase, aux);

    while(aux[i] != '.' && aux[i] != ',' && aux[i] != '\0')
    {
        tab[i] = aux[i];
        i++;
    }

    tab[i] = '\0';

    if(aux[i] == '.')
    {
        i++;

        while(aux[i] != ',' && aux[i] != '\0')
        {
            col[j] = aux[i];
            i++;
            j++;
        }

        col[j] = '\0';
    }
    else
        col[0] = '\0';
}

void wherePonto(BancoDados *bd,Campos **Fk,Campos **Pk,char frase[TF])
{
    char aux[TF],tab[TF],cond[TF];
    Tabela *auxTab;
    lerPonto(frase,tab,cond);
    auxTab = buscarTabela(bd->pTabelas,tab);
    *Pk = buscarCampo(auxTab->pCampos,cond); // nome da coluna da PK
    lerPalavra(frase,aux); // consome '='
    lerPonto(frase,tab,cond);
    auxTab = buscarTabela(bd->pTabelas,tab);
    *Fk = buscarCampo(auxTab->pCampos,cond); // nome da coluna da FK

}

void select(BancoDados *bd,char frase[TF])
{
    Fila *F1,*F2;
    Campos *Fk,*Pk;
    init(&F2);
    init(&F1);
    char aux[TF],colunas[TF],copia[TF],modo,where,tab[TF],col[TF];
    colunas[0] = '\0';
    Nos coln,tabl;
    Tabela *auxTab=NULL;
    Campos *auxCampo,*condCampo;
    TipoValor valor,valorIni,valorFin;
    int qtde=0;

    strcpy(copia,frase);
    lerPalavra(copia,aux); // primeiro token depois do SELECT

    if(strcmp(aux,"*")==0)
    {
        lerPalavra(copia,aux); // FROM
        lerPalavra(copia,aux); // tabela
        auxTab = buscarTabela(bd->pTabelas,aux);
        if(auxTab == NULL)
        {
            printf("\nTabela invalida!\n");
            return;
        }
        auxCampo = auxTab->pCampos;
        while(auxCampo != NULL)
        {
            coln.Arg.Campo = auxCampo;
            enqueue(&F1,coln);
            qtde++;
            auxCampo = auxCampo->prox;
        }
        where = 'N';
        exibirDados(auxTab,F1,F2,qtde,condCampo,modo,valor,valorIni,valorFin,where,Fk,Pk);
    }
    else if(contemPonto(aux))
    {
        strcpy(colunas, aux);
        while(strcmp(tab,"FROM")!=0)
        {
            lerPonto(copia,tab,col);
            tabl.Arg.Tab = buscarTabela(bd->pTabelas,tab);
            coln.Arg.Campo = buscarCampo(tabl.Arg.Tab->pCampos,col);
            enqueue(&F1,coln);
            enqueue(&F2,tabl);
            qtde++;
            if (copia[0] != '\0')
                lerPonto(copia,tab,col);
        }
        lerPalavra(copia,aux);
        while(strcmp(aux,"WHERE")!=0)
            lerPalavra(copia,aux);
        wherePonto(bd,&Fk,&Pk,copia);
        where = '.';
        exibirDados(auxTab,F1,F2,qtde,condCampo,modo,valor,valorIni,valorFin,where,Fk,Pk);
    }
    else
    {
        strcpy(colunas,aux);
        while(strcmp(aux,"FROM")!=0)
        {
            if(aux[strlen(aux)-1] == ',')
                aux[strlen(aux)-1] = '\0';
            strcat(colunas,aux);
            strcat(colunas," ");
            lerPalavra(copia,aux);
        }
        lerPalavra(copia,aux); //ler tabela
        auxTab = buscarTabela(bd->pTabelas,aux);
        auxCampo = auxTab->pCampos;
        lerPalavra(colunas,aux);
        while(aux[0] != '\0')
        {
            coln.Arg.Campo = buscarCampo(auxCampo,aux);
            enqueue(&F1,coln);
            qtde++;
            lerPalavra(colunas,aux);
        }
        if(copia[0] != '\0')
        {
            lerPalavra(copia,aux); //ler WHERE
            if(strcmp(aux,"WHERE") == 0)
            {
                lerWhere(&condCampo,auxTab,&modo,&valor,&valorIni,&valorFin,copia);
                where = 'W';
            }
            else
                where = 'N';
        }
        else
            where = 'N';

        exibirDados(auxTab,F1,F2,qtde,condCampo,modo,valor,valorIni,valorFin,where,Fk,Pk);
    }

    destruir(&F1);
    destruir(&F2);
    printf("\n***SELECT concluido!***\n");
}

//--------------SELCT------------------------//

// ========== leitura de script  ========== //


void LerBloco(char *frase, char *aux)
{
    int i = 0, j = 0, prof;
    while (frase[i] != '(' && frase[i] != '\0') 
        i++;
    i++;
    prof = 1;
    while (prof > 0 && frase[i] != '\0')
    {
        if (frase[i] == '(')
            prof++;
        else 
            if (frase[i] == ')') 
                prof--;
        if (prof > 0) { 
            aux[j] = frase[i]; 
            j++;
        }
        i++;
    }
    aux[j] = '\0';
    int k = 0;
    while (frase[i] != '\0') { 
        frase[k] = frase[i]; 
        i++; 
        k++; 
    }
    frase[k] = '\0';
}


void LerItemDDL(char *frase, char *aux)
{
    int i = 0, j = 0, prof = 0;
    while (frase[i] == ' ') 
        i++;  

    while (frase[i] != '\0' && !(frase[i] == ',' && prof == 0))
    {
        if (frase[i] == '(') 
            prof++;
        else 
            if (frase[i] == ')') 
                prof--;
        aux[j] = frase[i];
        i++;
        j++;
    }

    while (j > 0 && aux[j-1] == ' ') 
        j--;

    aux[j] = '\0';

    if (frase[i] == ',') 
        i++;
    if (frase[i] == ' ') 
        i++;

    int k = 0;

    while (frase[i] != '\0') { 
        frase[k] = frase[i]; 
        i++; 
        k++; 
    }
    frase[k] = '\0';
}


void LerComandoDDL(char *frase, char *comando)
{
    int i = 0, j = 0, ultimoEraEspaco = 1;

    while (frase[i] != ';' && frase[i] != '\0')
    {
        char c = frase[i];
        if (c == '\n' || c == '\r' || c == '\t') c = ' ';

        if (c == ' ')
        {
            if (!ultimoEraEspaco) 
            { 
                comando[j] = ' '; 
                j++; 
                ultimoEraEspaco = 1; 
            }
        }
        else { 
                comando[j] = c; 
                j++; 
                ultimoEraEspaco = 0; 
            }
        i++;
    }
    if (j > 0 && comando[j-1] == ' ') j--;
    comando[j] = '\0';

    if (frase[i] == ';') 
        i++;
    while (frase[i] == ' ' || frase[i] == '\n' || frase[i] == '\r' || frase[i] == '\t') 
        i++;

    int k = 0;
    while (frase[i] != '\0') { 
        frase[k] = frase[i];
        i++; 
        k++; 
    }
    frase[k] = '\0';
}


char ConverterTipoSQL(char *tipoSQL)
{
    char nome[TF],c;
    int i = 0;

    while (tipoSQL[i] != '(' && tipoSQL[i] != '\0')
    {
        nome[i] = tipoSQL[i];
        i++;
    }
    nome[i] = '\0';

    if (strcmp(nome, "INTEGER") == 0) 
        c = 'I';
    if (strcmp(nome, "DATE") == 0) 
        c = 'D';
    if (strcmp(nome, "NUMERIC") == 0) 
        c = 'N';
    if (strcmp(nome, "CHARACTER") == 0)
    {
        if (tipoSQL[i] == '(' && atoi(&tipoSQL[i + 1]) == 1)
            c = 'C';    
        else
            c = 'T';                // CHARACTER(N>1) vira texto 
    }
    else
        c = 'T';
    return c;
}


void ProcessarConstraintPK(Tabela *tab, char *item)
{
    char aux[TAM_SCRIPT], colunas[TAM_SCRIPT], nomeColuna[TAM_SCRIPT];
    lerPalavra(item, aux);     // CONSTRAINT 
    lerPalavra(item, aux);     // nome da constraint 
    lerPalavra(item, aux);     // PRIMARY 
    lerPalavra(item, aux);     // KEY 
    lerPalavra(item, colunas);

    while (colunas[0] != '\0')
    {
        lerColuna(colunas, nomeColuna);
        Campos *c = buscarCampo(tab->pCampos, nomeColuna);
        if (c != NULL) 
            c->pk = 'S';
    }
}


void ProcessarCreateTable(BancoDados *bd, char *comando)
{
    char aux[TAM_SCRIPT], nomeTabela[TF], bloco[TAM_SCRIPT], blocoOriginal[TAM_SCRIPT], item[TAM_SCRIPT];

    lerPalavra(comando, aux);      // CREATE
    lerPalavra(comando, aux);      // TABLE
    lerPalavra(comando, nomeTabela);

    Tabela *tab = criarTabela(nomeTabela);

    LerBloco(comando, bloco);
    strcpy(blocoOriginal, bloco); // GUARDA COPIA

    //CRIA COLUNAS
    while (bloco[0] != '\0')
    {
        LerItemDDL(bloco, item);

        char itemCopia[TAM_SCRIPT], primeira[TAM_SCRIPT];
        strcpy(itemCopia, item);
        lerPalavra(itemCopia, primeira);

        if (strcmp(primeira, "CONSTRAINT") != 0)
        {
            char nomeColuna[TF], tipoSQL[TAM_SCRIPT];
            lerPalavra(item, nomeColuna);
            lerPalavra(item, tipoSQL);

            char tipoInterno = ConverterTipoSQL(tipoSQL);
            Campos *campo = criarCampoSemFk(nomeColuna, tipoInterno, 'N');
            inserirCampo(tab, campo);
        }
    }

    // COLOCA PK
    strcpy(bloco, blocoOriginal);
    while (bloco[0] != '\0')
    {
        LerItemDDL(bloco, item);

        char itemCopia[TAM_SCRIPT], primeira[TAM_SCRIPT];
        strcpy(itemCopia, item);
        lerPalavra(itemCopia, primeira);

        if (strcmp(primeira, "CONSTRAINT") == 0)
            ProcessarConstraintPK(tab, item);
    }

    inserirTabela(bd, tab);
}

void ProcessarAlterTable(BancoDados *bd, char *comando)
{
    char aux[TAM_SCRIPT], nomeTabela[TF], nomeColuna[TAM_SCRIPT], nomeTabelaRef[TF], nomeColunaRef[TAM_SCRIPT];

    lerPalavra(comando, aux);          //ALTER
    lerPalavra(comando, aux);          //TABLE
    lerPalavra(comando, nomeTabela);
    lerPalavra(comando, aux);          //ADD
    lerPalavra(comando, aux);          //CONSTRAINT
    lerPalavra(comando, aux);          //NOME DA CONSTRAINT
    lerPalavra(comando, aux);          //FOREIGN
    lerPalavra(comando, aux);          //KEY
    lerPalavra(comando, nomeColuna);   
    lerPalavra(comando, aux);          //REFERENCES
    lerPalavra(comando, nomeTabelaRef);
    lerPalavra(comando, nomeColunaRef); 

    Tabela *tab = buscarTabela(bd->pTabelas, nomeTabela);
    Tabela *tabRef = buscarTabela(bd->pTabelas, nomeTabelaRef);
    Campos *campo = buscarCampo(tab->pCampos, nomeColuna);
    Campos *campoRef = buscarCampo(tabRef->pCampos, nomeColunaRef);

    campo->fk = campoRef;
}

void ProcessarComandoDDL(BancoDados **bd, char *comando)
{
    char copia[TAM_SCRIPT], p1[TAM_SCRIPT], p2[TAM_SCRIPT];
    strcpy(copia, comando);
    lerPalavra(copia, p1);
    lerPalavra(copia, p2);

    if (strcmp(p1, "CREATE") == 0 && strcmp(p2, "DATABASE") == 0)
    {
        char nome[TF];
        lerPalavra(copia, nome);
        *bd = criarBancoDados(nome);
    }
    else if (strcmp(p1, "CREATE") == 0 && strcmp(p2, "TABLE") == 0)
    {
        ProcessarCreateTable(*bd, comando);
    }
    else if (strcmp(p1, "ALTER") == 0 && strcmp(p2, "TABLE") == 0)
    {
        ProcessarAlterTable(*bd, comando);
    }
}

BancoDados* LerScript(char *nomeArquivo)  // ler com fgets
{
    FILE *arq = fopen(nomeArquivo, "r");

    char conteudo[TAM_SCRIPT];
    int tam = 0, ch;
    while ((ch = fgetc(arq)) != EOF && tam < TAM_SCRIPT - 1)
    {
        conteudo[tam] = (char) ch;
        tam++;
    }
    conteudo[tam] = '\0';
    fclose(arq);

    BancoDados *bd = NULL;
    char comando[TAM_SCRIPT];

    while (conteudo[0] != '\0')
    {
        LerComandoDDL(conteudo, comando);
        if (comando[0] != '\0')
            ProcessarComandoDDL(&bd, comando);
    }

    return bd;
}

// ========== acaba leitura de script  ========== //

void executar(BancoDados *bd)
{
    char str[TF], instrucao, tecla = 'S';
    do
    {
        printf("\nLinha de Comando SQL: ");
        gets(str);
        instrucao = lerInstrucao(str);
        switch (instrucao)
        {
        case 'I':
            insert(bd, str);
            break;
        case 'U':
            update(bd, str);
            break;
        case 'D':
            delete(bd, str);
            break;
        case 'S':
            select(bd, str);
            break;
        case 'N':
            break;
        }
        printf("Tecle para continuar os comandos SQL; [Esc] para finalizar programa");
        tecla = getche();
    } while (tecla != 27);
}


int main()
{
    char arq[TF];
    printf("Arquivo: ");
    gets(arq);
    BancoDados *bd = NULL;
    bd = LerScript(arq);
    if(bd != NULL)
        printf("\n***Banco de Dados Criado!***\n");
    executar(bd);

    return 0;
}
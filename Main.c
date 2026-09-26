#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include "meuconio.h"
#include "Structs_FuncoesGeral.h"
#define TF 200
#define TAM_SCRIPT 17000

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
void update(BancoDados *bd,char frase[TF]);
void lerWhere();
void delete();
void destruir(Fila **F1);
void selectAll();
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
    printf("\nUPDATE concluido!\n");
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
    printf("\nDelete concluido!\n");
}

void selectAll()
{

}

void select()
{

}

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
            Campos *campo = criarCampo(nomeColuna, tipoInterno, 'N');
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
            delete(bd, str);
            break;
        case 'S':
            select(bd, str);
            break;
        case '*':
            selectAll(bd, str);
            break;
        case 'N':
            break;
        }
        tecla = getch();
        gets(str);
        instrucao = lerInstrucao(str);
    } while (tecla != 27);
}


int main()
{
    char arq[TF];
    printf("Arquivo: ");
    gets(arq);
    BancoDados *bd;

    bd = lerScript(arq);
    executar(bd);
    
}
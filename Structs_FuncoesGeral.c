#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

//--------------------------------------------Structs--------------------------------------------//
struct BancoDados{
    char bancoDados[50];
    Tabela *pTabelas;
}typedef BancoDados;

struct Tabela{
    char tabela[50];
    Tabela *ant,*prox;
    Campos *pCampos;
}typedef Tabela;

typedef struct Campos {
    char campo[50];
    struct Campos *prox;
    Valor *pValores;
} Campos;

typedef struct Valor {
    char valor[50];
    struct Valor *prox;
} Valor;
//--------------------------------------------Structs--------------------------------------------//


//--------------------------------------------Prototipo das Funções--------------------------------------------//
BancoDados* criarBancoDados(char nome[50]);
Tabela* criarTabela(char nome[50]);
Campos* criarCampo(char* nome, char tipo, char pk, char fk);
Valor* criarValor(char tipo, char* textoValor);   

void  inserirValor(Valor** lista, Valor* novo);        // sempre no fim, pra manter a ordem das linhas
Valor* buscarValorPorIndice(Valor* lista, int indiceLinha);
void  removerValorPorIndice(Valor** lista, int indiceLinha);
void  atualizarValor(Valor* Valor, char tipo, char* novoTextoValor);
int   compararValor(Valor* Valor, char tipo, char* valorComparar); // igualdade e BETWEEN no WHERE
void  imprimirValor(Valor* Valor, char tipo);         // formata pra tela conforme o tipo
void  liberarValores(Valor* lista);

void   inserirCampo(Campos** lista, Campos* novo);
Campos* buscarCampo(Campos* lista, char* nome);
int    getIndiceCampo(Campos* lista, char* nome);   // posição do campo — precisa pra achar o Dado correspondente na linha
Campos* buscarCampoPK(Campos* lista);                // pra validar INSERT/UPDATE contra chave primária
void   liberarCampos(Campos* lista);

void    inserirTabela(BancoDados *bd, Tabela* nova);
Tabela* buscarTabela(Tabela* tab, char nome[50]);   // usado por DDL, DML e DQL o tempo todo //recebe bd->pTabela
void    removerTabela(BancoDados *bd, char nome[50]); // opcional, mas bom ter
void    liberarTabelas(Tabela* lista);
         
void liberarBancoDados(BancoDados* bd);    // libera em cascata os Campos e Dados
//--------------------------------------------Prototipo das Funções--------------------------------------------//


//--------------------------------------------Descritor - Banco de Dados--------------------------------------------//

BancoDados* criarBancoDados(char nome[50])
{
    BancoDados *bd = (BancoDados*)malloc(sizeof(BancoDados));
    strcpy(bd->bancoDados,nome);
    bd->pTabelas = NULL;
    return bd;
}

void liberarBancoDados(BancoDados* bd){
    if (bd == NULL) 
        printf("Banco de Dados inexistente!!!");
    else{
        liberarTabelas(bd->pTabelas);
        free(bd);
    }
}
//--------------------------------------------Descritor - Banco de Dados--------------------------------------------//


//--------------------------------------------Lista Dupla - Tabela--------------------------------------------//

Tabela* criarTabela(char nome[50]){
    Tabela *tb = (Tabela*)malloc(sizeof(Tabela));
    tb->ant = tb->prox = NULL;
    tb->pCampos = NULL;
    strcpy(tb->tabela,nome);
    return tb;
}

void    inserirTabela(BancoDados *bd, Tabela *tab){
    if(bd == NULL || tab == NULL)
    {
        printf("Erro de Compilacao!!!");
    }
    else
    {
        if(bd->pTabelas == NULL)
            bd->pTabelas = tab;
        else
        {
            Tabela* atual = bd;
            while (atual->prox != NULL)
                atual = atual->prox;
            atual->prox = tab;
            tab->ant = atual;
        }
    }
}

Tabela* buscarTabela(Tabela *tab, char nome[50]){
    if(tab == NULL)
    {
        printf("Erro de Compilacao!!!");
    }
    else
    {
        while(tab->prox != NULL && (strcmp(tab->tabela,nome)!=0) )
        {
            tab = tab->prox;
        }
        if(strcmp(tab->tabela,nome)==0)
        {
            return tab;
        }
        printf("Nao existe!!!");
    }
    return NULL;
}

void  liberarTabelas(Tabela* tab){
    if (tab == NULL) 
        printf("Tabela inexistente!!!");
    else{
        Tabela *atual=tab; 
        while(atual!=NULL)
        {
            liberarCampos(atual->pCampos);
            atual = atual->prox;
            free(tab);
            tab = atual;
        }
    }
}

void    removerTabela(BancoDados *bd, char nome[50]){
    if (bd == NULL) 
        printf("Banco de Dados inexistente!!!");
    else{
        Tabela *atual=buscarTabela(bd->pTabelas,nome); 
        if(atual != NULL)
        {
            liberarCampos(atual->pCampos);
            if(atual->ant == atual->prox)
                bd->pTabelas = NULL;
            else
            {
                if(atual->ant == NULL){
                    bd->pTabelas = atual->prox;
                    atual->prox->ant = NULL;
                }
                else
                {
                    if(atual->prox == NULL)
                    {
                        atual->ant->prox = NULL;
                    }
                    else
                    {
                        atual->ant->prox = atual->prox;
                        atual->prox->ant = atual->ant;
                    }
                }
            }
            free(atual);
        }
    }
}
//--------------------------------------------Lista Dupla - Tabela--------------------------------------------//


//--------------------------------------------Lista Simples - Campos--------------------------------------------//
//--------------------------------------------Lista Simples - Campos--------------------------------------------//


//--------------------------------------------Lista Simples - Valor--------------------------------------------//
//--------------------------------------------Lista Simples - Valor--------------------------------------------//
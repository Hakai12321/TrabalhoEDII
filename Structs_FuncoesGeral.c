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
Tabela* buscarTabela(Tabela* lista, char* nome);   // usado por DDL, DML e DQL o tempo todo
void    removerTabela(Tabela** lista, char* nome); // opcional, mas bom ter
void    liberarTabelas(Tabela* lista);             // libera em cascata os Campos e Dados
//--------------------------------------------Prototipo das Funções--------------------------------------------//


//--------------------------------------------Descritor - Banco de Dados--------------------------------------------//

void liberarBancoDados(BancoDados* bd);

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

void    inserirTabela(BancoDados *bd, Tabela* nova){
    if(bd == NULL || nova == NULL)
    {
        printf("Erro de Compilacao!!!");
    }
    else
    {
        
    }
}
//--------------------------------------------Lista Dupla - Tabela--------------------------------------------//


//--------------------------------------------Lista Simples - Campos--------------------------------------------//
//--------------------------------------------Lista Simples - Campos--------------------------------------------//


//--------------------------------------------Lista Simples - Valor--------------------------------------------//
//--------------------------------------------Lista Simples - Valor--------------------------------------------//
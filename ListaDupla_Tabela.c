#include "ListaSimples_Campos.c"
struct Tabela{
    char tabela;
    Tabela *ant,*prox;
    Campos pCampos;
}typedef Tabela;
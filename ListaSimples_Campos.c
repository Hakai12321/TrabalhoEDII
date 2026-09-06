#include "ListaSimples_Valor.c"
struct Campos{
    Valor *pAtual,*pDados;
    char campo[50],tipo,PK;
    Campos *FK,*prox;
}typedef Campos;
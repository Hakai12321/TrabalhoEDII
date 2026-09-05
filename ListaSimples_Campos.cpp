#include "ListaSimples_Valor.cpp"
struct Campos{
    Valor *pAtual,*pDados;
    char campo[50],tipo,PK;
    Campos *FK,*prox;
}typedef Campos;
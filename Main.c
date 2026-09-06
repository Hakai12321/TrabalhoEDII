#include "Descritor_BancoDados.c"
#include "ListaDupla_Tabela.c"
#include "ListaSimples_Campos.c"
#include "ListaSimples_Valor.c"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

int main()
{
    char s[50];
    FILE *ptr = fopen("","r");
    while(!feof(ptr))
    {
        fscanf(ptr,"%s",s);
        //interpreta cada palavra
    }
    
}
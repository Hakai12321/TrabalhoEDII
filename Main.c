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
        fscanf(ptr,"%s %s",s);
        //interpreta cada palavra
        
    }
    
}
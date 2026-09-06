struct Valor{
    Valor *prox;
    TipoValor tipoValor;
}typedef Valor;

union TipoValor
{
    int valorI;
    float valorN;
    char valorD[10];
    char valorC;
    char valorT[20];
}typedef TipoValor;

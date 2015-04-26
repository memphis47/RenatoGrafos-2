#include <stdio.h>
#include "grafo.h"

//------------------------------------------------------------------------------
int main(void) {

  return ! destroi_grafo(escreve_grafo(stdout, le_grafo(stdin)));
}

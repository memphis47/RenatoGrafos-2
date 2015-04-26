#include <stdio.h>
#include <stdlib.h>
#include <cgraph.h>

//------------------------------------------------------------------------------
static Agedge_t **arestas;

static int direcionado,
  n_vertices,
  n_arestas,
  n_arestas_visitadas = 0;

//------------------------------------------------------------------------------
static int busca_aresta(Agedge_t *a){

  for ( int i=0; i < n_arestas_visitadas; ++i)
    
    if ( ageqedge(a, arestas[i]) )
      
      return 1;

  return 0;
}
//------------------------------------------------------------------------------
static void guarda_arestas(Agraph_t *g, Agnode_t *v) {

  for (Agedge_t *a=agfstedge(g,v); a; a=agnxtedge(g,a,v))

    if ( ! busca_aresta(a) )

      arestas[n_arestas_visitadas++] = a;
}
//------------------------------------------------------------------------------
static void guarda_arcos(Agraph_t *g, Agnode_t *v) {

  for (Agedge_t *a=agfstout(g,v); a; a=agnxtout(g,a))

    if ( ! busca_aresta(a) )

      arestas[n_arestas_visitadas++] = a;
}
//------------------------------------------------------------------------------
static void mostra_arestas(void) {

  if ( !n_arestas )
    return;
  
  char rep_aresta = direcionado ? '>' : '-';
  
  for (int i=0; i < n_arestas; ++i) {

    char *peso = agget(arestas[i], (char *)"peso");
    
    printf("    \"%s\" -%c \"%s\"",
           agnameof(agtail(arestas[i])),
           rep_aresta,
           agnameof(aghead(arestas[i]))
           );

    if ( peso && *peso )
      printf(" [peso=%s]", peso);

    printf("\n");
  }
}
//------------------------------------------------------------------------------
static void mostra_vertices(Agraph_t *g) {

  if ( !n_vertices )
    return;

  for (Agnode_t *v=agfstnode(g); v; v=agnxtnode(g,v)) {

    printf("    \"%s\"\n", agnameof(v));
    
    if ( direcionado )
      guarda_arcos(g, v);
    else
      guarda_arestas(g, v);
  }
}
//------------------------------------------------------------------------------
static Agraph_t *mostra_grafo(Agraph_t *g) {

  if ( ! (g && agisstrict(g)) )
    return NULL;

  direcionado = agisdirected(g);

  n_vertices = agnnodes(g);

  n_arestas = agnedges(g);
  
  arestas = malloc(((unsigned int)n_arestas)*sizeof(Agedge_t *));

  printf("strict %sgraph \"%s\" {\n\n",
         agisdirected(g) ? "di" : "",
         agnameof(g)
         );

  mostra_vertices(g);

  printf("\n");

  mostra_arestas();

  free(arestas);

  printf("}\n");

  return g;
}
//------------------------------------------------------------------------------
int main(void) {

  Agraph_t *g = agread(stdin, NULL);

  if ( !g )
    return 1;

  free(mostra_grafo(g));

  return agerrors();
}

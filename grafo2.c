#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "grafo.h"

typedef struct aresta
{
  long int peso; // Peso da aresta
  char *verticeLigado; // Nome do vértice que essa aresta liga
  long int index; //indice do vertice na lista de vertices do grafo
} aresta;

//------------------------------------------------------------------------------
typedef struct grafo {
  vertice *vertices; // Conjunto de vertices de um grafo
  char *nome; // Nome do grafo
  int direcionado; // Informacao sobre o tipo do grafo, 0 se não eh e 1 caso seja direcionado
  int conexo;
  int hardConexo;
  long int diametro;
  int n_vertices; // Numero de vertices do grafo
  int n_arestas;  // Numero de arestas do grafo
  long int **matrixAdj;
} *grafo;

//------------------------------------------------------------------------------
typedef struct vertice {
  char *nome; // Nome do vertice
  aresta *arestas; // Conjunto de arestas do vertice
  int n_arestas_vertice; // Numero de arestas do vertice
} *vertice;

//------------------------------------------------------------------------------
const long int infinito = LONG_MAX;


static int direcionado,
  n_vertices,
  n_arestas,
  n_arestas_visitadas = 0; // variaveis para guardar alguns dados do grafo


/**   guarda_arestas: Coleta todas as arestas de um vertice v, de um grafo g, coletando
*           informacoes como peso e o nome do vertice que ela liga e salva no grafo graf;
*
**/
int buscaVertice(char * nome,grafo graf){
  int i;
  for(i=0;i< graf->n_vertices, i++){
     if(strcmp(nome,graf->vertices[i].nome) == 0)
        return i;
  }
}

static void guarda_arestas(Agraph_t *g, Agnode_t *v,  grafo graf, int i) {
  int j=0;
  double pes=0.0;
  for (Agedge_t *a=agfstedge(g,v); a; a=agnxtedge(g,a,v)){
    char *peso = agget(a, (char *)"peso");
    if(peso!= NULL)
      pes = atof(peso);

      graf->vertices[i].arestas[j].peso=pes;
      // verifica se o tail dessa aresta nao eh o vertice atual, se não for ele usa o tail para dizer qual eh é o vertice de ligacao, se não usa o head
      if(strcmp(agnameof(agtail(a)),agnameof(v)) != 0){ 

        int tam= (int) strlen(agnameof(agtail(a)));
        graf->vertices[i].arestas[j].verticeLigado=(char *) malloc(sizeof(char)*tam);
        graf->vertices[i].arestas[j].verticeLigado=agnameof(agtail(a));
        graf->vertices[i].arestas[j].index=buscaVertice(graf->vertices[i].arestas[j].verticeLigado,graf);
      }
      else{
        int tam=(int) strlen(agnameof(agtail(a)));
        graf->vertices[i].arestas[j].verticeLigado=(char *) malloc(sizeof(char)*tam);
        graf->vertices[i].arestas[j].verticeLigado=agnameof(aghead(a));
        graf->vertices[i].arestas[j].index=buscaVertice(graf->vertices[i].arestas[j].verticeLigado,graf);
      }
      
      j++;
    }
    graf->vertices[i].n_arestas_vertice=j;
}



/**   guarda_arestas: Coleta todas os vertices de um grafo g, coletando
*           informacoes como nome do vertice e as suas arestas, e salva elas no grafo graf
*
**/
static void getVerticeArestas( grafo graf,Agraph_t *g){
  int i=0;
  for (Agnode_t *v=agfstnode(g); v; v=agnxtnode(g,v)) {

    int num_arest=agdegree(g,v,1,1);
        
    graf->vertices[i].nome=(char *) malloc(sizeof(char)*strlen(agnameof(v)));
    graf->vertices[i].nome=agnameof(v);
    graf->vertices[i].arestas=(aresta *) malloc (num_arest*sizeof (aresta)); //TODO: multiplicar pelo numero de arestas do vertice

    guarda_arestas(g, v, graf, i);
    i++;
  }
}

static void allocateMatrix(grafo graf){
  /* aloca as linhas da matriz */
  int i;
  graf->matrixAdj = (long int **) calloc (graf->n_vertices, sizeof(long int *));
  if (graf->matrixAdj == NULL) {
    printf ("** Erro: Memoria Insuficiente **");
    return (NULL);
  }
  /* aloca as colunas da matriz */
  for ( i = 0; i < graf->n_vertices; i++ ) {
    graf->matrixAdj[i] = (long int*) calloc (graf->n_vertices, sizeof(long int));
    if (graf->matrixAdj[i] == NULL) {
      printf ("** Erro: Memoria Insuficiente **");
      return (NULL);
    }
  }
}

int createMatrix(grafo graf){
  if(graf->n_vertices > 1){
    allocateMatrix(graf);
    int i,j,k;
    for(i=0;graf->n_vertices;i++){
      for(j=0;graf->n_vertices;j++){
        if(j==graf->vertices[i].arestas[j].index)
          if(graf->vertices[i].arestas[j].peso > 0)
            graf->matrixAdj[i][j]=graf->vertices[i].arestas[j].peso;
          else
            graf->matrixAdj[i][j]=1;
        else
          graf->matrixAdj[i][j]=0;
      }
    }

  }
  else{

  }

}
//------------------------------------------------------------------------------
grafo le_grafo(FILE *input) {
  Agraph_t *g = agread(input, NULL);
  if ( (g ) ){

    direcionado = agisdirected(g);

    n_vertices = agnnodes(g);

    n_arestas = agnedges(g);

    grafo graf=( grafo ) malloc (sizeof( grafo));

    int tam=(int) strlen(agnameof(g));
    graf->nome=(char *) malloc(sizeof(char)*tam);
    graf->nome=agnameof(g);
    graf->vertices=(vertice *) malloc (n_vertices*sizeof(vertice));
    graf->direcionado=direcionado;
    graf->n_vertices=n_vertices;
    graf->n_arestas=n_arestas;
    getVerticeArestas(graf,g);
    createMatrix(graf);
    return graf;
  }
  return input ? NULL : NULL;
}




//------------------------------------------------------------------------------
int destroi_grafo(grafo g) {
  int i,j;
  for(i=0;i < g->n_vertices;i++){
    free(g->vertices[i].arestas);
  }
  free(g->vertices);
  free(g);
  return g ? 0 : 1;
}
//------------------------------------------------------------------------------
grafo escreve_grafo(FILE *output, grafo g) {
  int i=0;
  int j=0;
  char dir='>';

  if(g->direcionado)
    fprintf(output,"strict digraph \"%s\" { \n",g->nome);
  else
    fprintf(output,"strict graph \"%s\" { \n",g->nome);

  for(i=0;i< g->n_vertices;i++){ //imprime todos os vertices do grafo
    fprintf(output,"\t%s\n",g->vertices[i].nome);
  }
  fprintf(output,"\n");
  for(i=0;i < g->n_vertices;i++){ //imprime todas as arestas do grafo
    for (j = 0; j < g->vertices[i].n_arestas_vertice; j++)
    {
      if(!g->direcionado)
        dir='-';
      if(g->vertices[i].arestas[j].peso > 0)
        fprintf(output,"\t%s -%c %s [peso=%f]\n", g->vertices[i].nome,
                      dir,
                      g->vertices[i].
                      arestas[j].verticeLigado, 
                      g->vertices[i].arestas[j].peso);
      else
        fprintf(output,"\t%s -%c %s \n", g->vertices[i].nome,
                      dir,
                      g->vertices[i].
                      arestas[j].verticeLigado);
    }
  }
  fprintf(output,"}\n");
  return output ? g : NULL;
}
//------------------------------------------------------------------------------
char *nome(grafo g) {

  return g ? g->nome : "";
}
//------------------------------------------------------------------------------
unsigned int n_vertices(grafo g) {

  return g ? g->n_vertices : 0;
}

//------------------------------------------------------------------------------
int direcionado(grafo g) {

  return g ? g->direcionado : 0;
}
//------------------------------------------------------------------------------
int conexo(grafo g) {
  buscaConexo(g);

  return g ? g->conexo : 0;
}

static void buscaConexo(grafo graf){
  int wayArray[= (int *) calloc(]graf->n_vertices*sizeof(int));
  int i,j,k,cont;
  cont=0;
  i=0;
  wayArray[0]=1;
  while(){
    for(j=0;j<graf->n_vertices;j++){
      if(g->matrixAdj[i][j]>=1){
        wayArray[j]=1;
      }
    }
    for(k=0;k<graf->n_vertices;k++){
      if(wayArray[k]==1){
        i=k;
        break;
      }
    }
  }
}
//------------------------------------------------------------------------------
int fortemente_conexo(grafo g)  {

  return g ? g->hardConexo : 0;
}
//------------------------------------------------------------------------------
long int  diametro(grafo g) {
  return g ? g->diametro : infinito;
}
//------------------------------------------------------------------------------

grafo distancias(grafo g) {

  return g;
}


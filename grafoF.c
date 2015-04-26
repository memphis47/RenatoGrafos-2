#include <stdlib.h>
#include <stdio.h>
#include <graphviz/cgraph.h>
#include <string.h>
#include "grafo.h"

/**
*  Struct Aresta: Struct que reprensenta a aresta de um grafo, essa aresta pode ter ou não um peso
*  				  e tem o nome do vértice que ela liga;
*				  Ex: se o vértice é "A" e a sua aresta liga com o vértice "B" então o campo 
*				  verticeLigado sera preenchido com o nome do vértice "B".
**/
typedef struct aresta
{
	double peso; // Peso da aresta
	char *verticeLigado; // Nome do vértice que essa aresta liga
} aresta;


/**
*  Struct Vertice: Struct que reprensenta o vertice de um grafo, esse vertice tem um nome, um conjunto
*				   de vertices que ele liga e quantas arestas ele possui ;
**/
typedef struct vertice
{
	char *nome; // Nome do vertice
	aresta *arestas; // Conjunto de arestas do vertice
	int n_arestas_vertice; // Numero de arestas do vertice
} vertice;


/**
*  Struct Grafo: Struct que reprensenta um Grafo, essa Grafo tem um conjunto de vertices, um nome, se ele
*				 eh ou nao direcionado e quantos vertices e arestas ele tem;
**/
struct grafo {
	vertice *vertices; // Conjunto de vertices de um grafo
	char *nome; // Nome do grafo
	int direcionado; // Informacao sobre o tipo do grafo, 0 se não eh e 1 caso seja direcionado
	int n_vertices;	// Numero de vertices do grafo
	int n_arestas;  // Numero de arestas do grafo
};





static int direcionado,
  n_vertices,
  n_arestas,
  n_arestas_visitadas = 0; // variaveis para guardar alguns dados do grafo


/**		guarda_arestas: Coleta todas as arestas de um vertice v, de um grafo g, coletando
*						informacoes como peso e o nome do vertice que ela liga e salva no grafo graf;
*
**/

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
  		}
  		else{
  			int tam=(int) strlen(agnameof(agtail(a)));
  			graf->vertices[i].arestas[j].verticeLigado=(char *) malloc(sizeof(char)*tam);
  			graf->vertices[i].arestas[j].verticeLigado=agnameof(aghead(a));
  		}
  		
  		j++;
  	}
  	graf->vertices[i].n_arestas_vertice=j;
}

/**		guarda_arestas: Coleta todas os vertices de um grafo g, coletando
*						informacoes como nome do vertice e as suas arestas, e salva elas no grafo graf
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

// lê um grafo no formato dot de input, usando as rotinas de libcgraph
// 
// desconsidera todos os atributos do grafo lido
// exceto o atributo "peso" nas arestas onde ocorra
// 
// num grafo com pesos nas arestas todas as arestas tem peso, que é um double
// 
// o peso default de uma aresta num grafo com pesos é 0.0
// 
// devolve o grafo lido ou
//         NULL em caso de erro 
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
		return graf;
	}
	return NULL;
}






//------------------------------------------------------------------------------
// desaloca toda a memória utilizada em g
// 
// devolve 1 em caso de sucesso ou
//         0 em caso de erro

int destroi_grafo(grafo g) {
	int i,j;
	for(i=0;i < g->n_vertices;i++){
		free(g->vertices[i].arestas);
	}
	free(g->vertices);
	free(g);
	return 1;
}

//------------------------------------------------------------------------------
// escreve o grafo g em output usando o formato dot, de forma que
// 
// 1. todos os vértices são escritos antes de todas as arestas/arcos 
// 2. se uma aresta tem peso, este deve ser escrito como um atributo
//
// devolve o grafo escrito ou
//         NULL em caso de erro 

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
}

void main(int argc, char *argv[]) 
{ 
	grafo grafs;
	FILE *in;
	FILE *out;
	if(argc>1){ // Caso o programa possua parametros usa eles

		if(!(in=fopen(argv[1], "r" ))) // o primeiro parametro eh o arquivo dot, caso o arquivo nao seja valido le a entrada stdin padrao
			in=stdin;
		
		if(!(out=fopen(argv[2],"w"))) // o segundo parametro eh um arquivo de saida, caso o arquivo nao seja valido usa a saida stdout padrao
			out=stdout;

	}
	else{ // caso nao tenha parametros usa a entrada stdin padrao e a saida stdout padrao
		in=stdin;
		out=stdout;
	}
	grafs=le_grafo(in);
	if(grafs!=NULL){
		escreve_grafo(out,grafs);
		if(!destroi_grafo(grafs))
			printf("Erro ao dar free() no grafo\n");
	}
	else
		printf("Aconteceu algum erro durante a leitura do grafo\n");

}
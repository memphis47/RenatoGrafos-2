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
	long int index; //indice do vertice na lista de vertices do grafo
} aresta;


/**
*  Struct Vertice: Struct que reprensenta o vertice de um grafo, esse vertice tem um nome, um conjunto
*				   de vertices que ele liga e quantas arestas ele possui ;
**/
struct vertice
{
	char *nome; // Nome do vertice
	aresta *arestas; // Conjunto de arestas do vertice
	int n_arestas_vertice; // Numero de arestas do vertice
};


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
	long int **matrixAdj;
	long int **pathSize;
	int conexo;
	int fortConexo;
	int diametro;
};





 // variaveis para guardar alguns dados do grafo


int buscaVertice(char * nome,grafo graf){
  int i;
  for(i=0;i< graf->n_vertices;i++){
     if(strcmp(nome,graf->vertices[i]->nome) == 0)
        return i;
  }
  return -1;
}
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

  		graf->vertices[i]->arestas[j].peso=pes;
  		// verifica se o tail dessa aresta nao eh o vertice atual, se não for ele usa o tail para dizer qual eh é o vertice de ligacao, se não usa o head
  		
	  		if(strcmp(agnameof(agtail(a)),agnameof(v)) != 0){ 
	  			if(!graf->direcionado){
		  			int tam= (int) strlen(agnameof(agtail(a)));
		  			graf->vertices[i]->arestas[j].verticeLigado=(char *) malloc(sizeof(char)*tam);
		  			graf->vertices[i]->arestas[j].verticeLigado=agnameof(agtail(a));
		  			j++;
	  			}
	  		}
	  		else{
	  			int tam=(int) strlen(agnameof(aghead(a)));
	  			graf->vertices[i]->arestas[j].verticeLigado=(char *) malloc(sizeof(char)*tam);
	  			graf->vertices[i]->arestas[j].verticeLigado=agnameof(aghead(a));
	  			j++;
	  			
	  		}
  		
  	}
  	graf->vertices[i]->n_arestas_vertice=j;
}

/**		guarda_arestas: Coleta todas os vertices de um grafo g, coletando
*						informacoes como nome do vertice e as suas arestas, e salva elas no grafo graf
*
**/
static void getVerticeArestas( grafo graf,Agraph_t *g){
	int j,i=0;
	for (Agnode_t *v=agfstnode(g); v; v=agnxtnode(g,v)) {

		int num_arest=agdegree(g,v,1,1);
        
		graf->vertices[i]->nome=(char *) malloc(sizeof(char)*strlen(agnameof(v)));
		graf->vertices[i]->nome=agnameof(v);
		graf->vertices[i]->arestas=(aresta *) malloc (num_arest*sizeof (aresta)); //TODO: multiplicar pelo numero de arestas do vertice

		guarda_arestas(g, v, graf, i);
		i++;
	}
	for(i=0; i < graf->n_vertices; i++){
		for(j=0;j < graf->vertices[i]->n_arestas_vertice; j++)
			graf->vertices[i]->arestas[j].index=buscaVertice(graf->vertices[i]->arestas[j].verticeLigado,graf);
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
static void allocateMatrix(grafo graf){
  /* aloca as linhas da matriz */
	int i;
    graf->matrixAdj = (long int **) calloc (graf->n_vertices, sizeof(long int *));
    if (graf->matrixAdj == NULL) {
      printf ("** Erro: Memoria Insuficiente **");
      return;
    }
    /* aloca as colunas da matriz */
    for ( i = 0; i < graf->n_vertices; i++ ) {
      graf->matrixAdj[i] = (long int*) calloc (graf->n_vertices, sizeof(long int));
      if (graf->matrixAdj[i] == NULL) {
        printf ("** Erro: Memoria Insuficiente **");
        return;
      }
    }
}

static void allocatePathMatrix(grafo g){
  /* aloca as linhas da matriz */
	int i;
    g->pathSize = (long int **) calloc (g->n_vertices, sizeof(long int *));
    if (g->pathSize == NULL) {
      printf ("** Erro: Memoria Insuficiente **");
      return;
    }
    /* aloca as colunas da matriz */
    for ( i = 0; i < g->n_vertices; i++ ) {
      g->pathSize[i] = (long int*) calloc (g->n_vertices, sizeof(long int));
      if (g->pathSize[i] == NULL) {
        printf ("** Erro: Memoria Insuficiente **");
        return;
      }
    }
}

static void findSmallPath(grafo g){
	int count;
	for(int i=0;i < g->n_vertices;i++){
		for (int j= 0; j < g->n_vertices; j++) // acha os caminhos
		{
			if(g->pathSize[i][j]>=1){
				for (int k= 0; k < g->n_vertices; ++k)
				{
					if(g->pathSize[j][k]>=1){
						count=g->pathSize[j][k]+g->pathSize[i][j];
						if(i!=k){
							if(g->pathSize[i][k]>count || g->pathSize[i][k]==0 && i!=k){
									g->pathSize[i][k]=count;
							}
						}
					}
				}
			}
		}
	}
}

long int findLongestPath(grafo g){
	long int big=0;
	for(int i=0;i < g->n_vertices;i++){
		for (int j= 0; j < g->n_vertices; j++) // pega os caminhos 1 do grafo
		{
			if(g->pathSize[i][j]>big){
				big=g->pathSize[i][j];
			}
		}
	}
	return big;
}

long int diametro(grafo g){
	long int pathSize;
	findSmallPath(g);
	printf("\n     ");
	for(int i=0;i  < g->n_vertices;i++){
		printf("%s||", g->vertices[i]->nome);
	}
	printf("\n");
	for(int i=0;i < g->n_vertices;i++){
	  printf("%s  ||", g->vertices[i]->nome);
	  for(int j=0;j < g->n_vertices;j++){
	  	printf("%ld||", g->pathSize[i][j]);
	  }
	  printf("\n");
	}
	pathSize=findLongestPath(g);
	return pathSize;
}

static void createMatrix(grafo graf){
	if(graf->n_vertices > 1){
		allocateMatrix(graf);
		allocatePathMatrix(graf);
		int idx;
		int i,j,k;
		for ( i = 0; i < graf->n_vertices; ++i)
		{
			for ( k = 0; k < graf->vertices[i]->n_arestas_vertice; ++k)
			{
				idx=graf->vertices[i]->arestas[k].index;
				graf->matrixAdj[i][idx]=1;
				graf->pathSize[i][idx]=1;
			}
		}
		

	}

}


int continua(int *vetCon, int max){
	for(int i=0; i < max; ++i){
		if(vetCon[i]==1)
			return i;
	}
	return -1;   
}



int conexo(grafo graf) {
	int *vetCon= (int *) calloc (graf->n_vertices,sizeof(int));
	int canGo=1;
	int row=0;
	int i,sum;
	long int mul=0;
	while(canGo!=-1){
		vetCon[row]=2;
		for(i=0;i < graf->n_vertices; ++i){
				sum+=graf->matrixAdj[row][i];
				if(graf->matrixAdj[row][i]>0 && vetCon[i]!=2)
					vetCon[i]=1;	
		}
		while(vetCon[row]!=1 && row < graf->n_vertices)
				row++;
		if(row >= graf->n_vertices){
			canGo=continua(vetCon,graf->n_vertices);
			row=canGo;
		}
	}
	
	for (i = 0; i < graf->n_vertices; ++i){
		mul=mul+vetCon[i];
	}
	if(mul==2*graf->n_vertices)
		return 1;
	return 0;
}


grafo le_grafo(FILE *input) {
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
	Agraph_t *g = agread(in, NULL);
	if ( (g ) ){

		grafo graf=(grafo ) malloc (sizeof( grafo));

		int tam=(int) strlen(agnameof(g));
		graf->nome=(char *) malloc(sizeof(char)*tam);
		graf->nome=agnameof(g);
		graf->n_vertices = agnnodes(g);
		graf->n_arestas = agnedges(g);
		graf->direcionado = agisdirected(g);
		graf->vertices=(vertice *) malloc (graf->n_vertices*sizeof(vertice));
		for (int i = 0; i < graf->n_vertices; ++i)
		{
			graf->vertices[i]=(vertice) malloc (sizeof(vertice));
		}

		getVerticeArestas(graf,g);
		createMatrix(graf);
		graf->conexo=conexo(graf);
		graf->diametro=diametro(graf);
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
		free(g->vertices[i]->arestas);
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
		fprintf(output,"\t%s\n",g->vertices[i]->nome);
	}
	fprintf(output,"\n");
	for(i=0;i < g->n_vertices;i++){ //imprime todas as arestas do grafo
		for (j = 0; j < g->vertices[i]->n_arestas_vertice; j++)
		{
			if(!g->direcionado)
				dir='-';
			if(g->vertices[i]->arestas[j].peso > 0)
				fprintf(output,"\t%s -%c %s [peso=%f]\n", g->vertices[i]->nome,
											dir,
											g->vertices[i]->
											arestas[j].verticeLigado, 
											g->vertices[i]->arestas[j].peso);
			else
				fprintf(output,"\t%s -%c %s \n", g->vertices[i]->nome,
											dir,
											g->vertices[i]->
											arestas[j].verticeLigado);
		}
	}
	fprintf(output,"}\n");
	return g;
}


grafo distancias(grafo g){
	grafo newGrapf=(grafo ) malloc (sizeof( grafo));
	int arestasGraph=0;
	int tam=(int) strlen(g->nome);
	newGrapf->nome=(char *) malloc(sizeof(char)*tam);
	strcpy(newGrapf->nome,g->nome);
	newGrapf->vertices=(vertice *) malloc (g->n_vertices*sizeof(vertice));
	for (int i = 0; i < g->n_vertices; ++i)
	{
		newGrapf->vertices[i]=(vertice) malloc (sizeof(vertice));
		newGrapf->vertices[i]->nome=(char *) malloc(sizeof(char)*strlen(g->vertices[i]->nome));
		strcpy(newGrapf->vertices[i]->nome,g->vertices[i]->nome);
	}
	for (int i = 0; i < g->n_vertices; ++i)
	{
		int arestasVertice=0;
		int index=0;
		for(int k=0;k < g->n_vertices; ++k){
			if(g->pathSize[i][k]>=1)
				arestasVertice++;
		}
		newGrapf->vertices[i]->arestas=(aresta *) malloc (arestasVertice*sizeof (aresta)); 
		for (int j = 0; j < g->n_vertices; ++j)
		{
			if(g->pathSize[i][j]>=1){
				//newGrapf->vertices[i]->arestas=(aresta *) realloc(newGrapf->vertices[i]->arestas,sizeof(aresta));
				newGrapf->vertices[i]->arestas[index].peso=g->pathSize[i][j];

				int tam= (int) strlen(newGrapf->vertices[index]->nome);
				newGrapf->vertices[i]->arestas[index].verticeLigado=(char *) malloc(sizeof(char)*tam);
				strcpy(newGrapf->vertices[i]->arestas[index].verticeLigado,newGrapf->vertices[j]->nome);
				newGrapf->vertices[i]->arestas[index].index=j;
				arestasGraph++;
				index++;
			}
		}
		newGrapf->vertices[i]->n_arestas_vertice=arestasVertice;
	}
	newGrapf->n_vertices=g->n_vertices;
	newGrapf->n_arestas=arestasGraph;

}



// int main(int argc, char *argv[]) 
// { 
// 	grafo grafs;
// 	FILE *in;
// 	FILE *out;
// 	if(argc>1){ // Caso o programa possua parametros usa eles

// 		if(!(in=fopen(argv[1], "r" ))) // o primeiro parametro eh o arquivo dot, caso o arquivo nao seja valido le a entrada stdin padrao
// 			in=stdin;
		
// 		if(!(out=fopen(argv[2],"w"))) // o segundo parametro eh um arquivo de saida, caso o arquivo nao seja valido usa a saida stdout padrao
// 			out=stdout;

// 	}
// 	else{ // caso nao tenha parametros usa a entrada stdin padrao e a saida stdout padrao
// 		in=stdin;
// 		out=stdout;
// 	}
// 	grafs=le_grafo(in);
// 	if(grafs!=NULL){
// 		escreve_grafo(out,grafs);
// 		grafo g=distancias(grafs);
// 		printf("----------------\n");
// 		escreve_grafo(out,g);
// 		if(!destroi_grafo(grafs))
// 			printf("Erro ao dar free() no grafo\n");
// 	}
// 	else
// 		printf("Aconteceu algum erro durante a leitura do grafo\n");

// }
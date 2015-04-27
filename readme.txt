Trabalho para a disciplina CI065 / CI755 - Algoritmos e Teoria dos Grafos
Professor Renato Carmo

Aluno: Rafael Rocha de Carvalho 
GRR: 20104490
Identificador: rafaelc

Introdução:
	Esse Trabalho tem como objetivo implementar o calculo de distancia e diametro de um grafo usando sua matriz de adj.

Modo de execução:
	executar com o makefile
Bugs:

	Bug 1: 
		Bug: Erro ao dar free no grafo g;
		Gravidade: Médio;
		Função Responsável: destroi_grafo(); 
		Descrição:  
			Nos arquivos .dot que foram disponibilizados pelo professor não acontece nenhum erro, 
			porêm com arquivos .dot que eu criei e com o arquivo petersen.dot o arquivo apresenta o seguinte erro:

				*** Error in `./nome_do_executavel': free(): invalid next size (fast): 0x0000000001448180 ***
				Abortado

			Esse erro não afeta o armazenamento do grafo na struct, só interrompendo o programa durante a execução da função detroi_grafo.

	Bug 2: 
		Bug: Erro ao calcular distancia no paises.dot;
		Gravidade: alta;
		Função Responsável: distancia(); 
		Descrição:  
			Nos arquivos .dot que foram disponibilizados pelo professor não acontece nenhum erro,
			porêm com o arquivo paises.dot ocorre o seguinte erro na hora de salvar o nome do verticeLigado:

 			malloc.c:2372: sysmalloc: Assertion `(old_top == (((mbinptr) (((char *) &((av)->bins[((1) - 1) * 2])) - __builtin_offsetof (struct malloc_chunk, fd)))) && old_size == 0) || ((unsigned long) (old_size) >= (unsigned long)((((__builtin_offsetof (struct malloc_chunk, fd_nextsize))+((2 *(sizeof(size_t))) - 1)) & ~((2 *(sizeof(size_t))) - 1))) && ((old_top)->size & 0x1) && ((unsigned long) old_end & pagemask) == 0)' failed.

			Abortado

			Acontece no pais uruguay com index=34;


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct word word;
typedef struct hash hash;
typedef struct lista lista;

//definicao de escopo
int hashing(char *palavra, int tam);
hash* criarHash(int tam);
word* criarNo(char *palavra);
void inserirHash(hash *tabela, char *palavra, int posicao);
int deletarHash(hash *tabela, char *palavra);
void imprimir(hash *tabela);
int buscar(hash *tabela, char *palavra);
int fatorDeCarga(hash *tabela);
int verificarPalavra(char *palavra);
hash* rehash(hash *tabela);
char* minusculo(char *palavra);

//definicoes de escopo da lista de sugestoes ortograficas
lista* letraErrada(hash *tabela, char *palavra, lista *list);
lista* letraTrocada(hash *tabela, char *palavra, lista *list);
lista* letraMenos(hash *tabela, char *palavra, lista *list);
lista* letraMais(hash *tabela, char *palavra, lista *list);
lista* listAddWord(lista *listaWord, char *palavra);
lista* listAddOrdenado(lista *list, char *palavra);
void listImprime(lista *list);
int listVazia(lista* list);
void listLibera(lista *list);
int listBusca(lista *list, char *palavra);

//definicoes de estruturas
struct word{
	char palavra[100];
	word *proximo;
};

struct hash{
	word *head;
	int contElem; //quantidade de elementos na hash
	int tamanho; //tamanho da tabela hash
};

struct lista{
	char palavra[102];
	lista *proximo;
};


int main(){
	hash *tabelaHash = criarHash(50); //começa com 50 elementos
	char entrada[100] = "";
	char auxEntrada[100] = " ";
	int posicao;
	
	//MENU PRINCIPAL
	while (1) {
		strcpy(entrada, "");
		fgets(entrada, 100, stdin); //Le entrada
        
        if(entrada[0]==10){ //Se a entrada for ENTER
			printf("fail\n");
		}else{
			entrada[strlen(entrada)-1] = '\0'; //Coloca o \0 no final da palavra
					           
			//SAIR DO PROGRAMA
			if(entrada[0]=='*'){
				free(tabelaHash);
				exit(0);
			}
			//COLOCAR PALAVRA
			else if(entrada[0]=='+'){
				if (verificarPalavra(auxEntrada)==1){
					if(buscar(tabelaHash, auxEntrada)==0){
						posicao = hashing(auxEntrada, tabelaHash->tamanho);
						inserirHash(tabelaHash, auxEntrada, posicao);
						printf("ok %s\n", auxEntrada);
						if(fatorDeCarga(tabelaHash)>1){
							tabelaHash = rehash(tabelaHash);
						}
					}else{
						printf("fail %s\n", auxEntrada);
					}
				}else{
					printf("fail %s\n", auxEntrada);
				}
			}
			//REMOVER PALAVRA
			else if(strcmp(entrada, "-") == 0){
				if (verificarPalavra(auxEntrada)==1){
					posicao = hashing(auxEntrada, tabelaHash->tamanho);
					if((deletarHash(tabelaHash, auxEntrada)==1)){
						printf("ok %s\n", auxEntrada);
					}else{
						printf("fail %s\n", auxEntrada);
					}
				}else{
					printf("fail %s\n", auxEntrada);
				}	
			//BUSCAR PALAVRA
			}else{
				if (verificarPalavra(entrada)==1){
					strcpy(entrada, minusculo(entrada));
					if(buscar(tabelaHash, entrada)==1){
						printf("ok %s\n", entrada);
					}else{
						lista *teste = NULL;
						lista *tempList = NULL;
						
						//1. VERIFICA SE POSSUI A MAIS
						tempList = letraMais(tabelaHash, entrada, teste);
						if (tempList != NULL){
							teste = tempList;
						}
						
						//2. VERIFICA SE POSSUI LETRA A MENOS
						tempList = letraMenos(tabelaHash, entrada, teste);
						if (tempList != NULL){
							teste = tempList;
						}
						
						//4. VERIFICA SE POSSUI LETRA ERRADAS
						tempList = letraErrada(tabelaHash, entrada, teste);
						if (tempList != NULL){
							teste = tempList;
						}
						
						//3. VERIFICA SE POSSUI LETRAS TROCADAS
						tempList = letraTrocada(tabelaHash, entrada, teste);
						if (tempList != NULL){
							teste = tempList;
						}
							
						if(listVazia(teste)==1){
							printf("not found\n");
						}else{
							listImprime(teste);
							listLibera(tempList);
						}
						
						listLibera(teste);
						listLibera(tempList);	
					}
				}else{
					printf("fail %s\n", entrada);
				}
				strcpy(auxEntrada, entrada);
			}
			strcpy(entrada, "");
		}
	}
	return 0;
}

//funcao hash
int hashing(char *palavra, int tam){
	int i, h = 0;
	for (i = 1; palavra[i] != '\0'; i++){
		h = (31 * h + palavra[i]);
	}
	return abs(h) % tam;
}

//cria tabela hash
hash* criarHash(int tam){
	hash* novaHash = (hash *) malloc (sizeof(hash)*tam);
	novaHash->contElem=0;
	novaHash->tamanho=tam;
	return novaHash;
}

//cria nova palavra
word* criarNo(char *palavra){
	word *novaPalavra;
	novaPalavra = (word *) malloc(sizeof(word));
	strcpy(novaPalavra->palavra, palavra);
	novaPalavra->proximo=NULL;
	return novaPalavra;
}

//insere palavra na hash
void inserirHash(hash *tabela, char *palavra, int posicao){
	hash *aux = tabela;
	word *novaPalavra = criarNo(palavra);
	novaPalavra->proximo = aux[posicao].head;
	aux[posicao].head = novaPalavra;
	aux->contElem++;
}

//deleta palavra da hash
int deletarHash(hash *tabela, char *palavra){
	hash *auxTable = tabela;
	word *aux, *temp;
	int posicaoHash = hashing(palavra, auxTable->tamanho), flag=0;
	aux = auxTable[posicaoHash].head;
	
	if(aux == NULL){
		return 0; 
	}else{
		temp = aux;
		while(aux != NULL){
			if (strcmp(aux->palavra, palavra)==0){
				flag=1;
				if(aux == auxTable[posicaoHash].head){
					auxTable[posicaoHash].head = aux->proximo;
				}else{
					temp->proximo=aux->proximo;
				}
				tabela->contElem--;
				free(aux);
				break;
			}
			temp = aux;
			aux = aux ->proximo;
		}
		if(flag==1){
			return 1; //printf("Palavra deletada com sucesso\n");
		}else{
			return 0; //printf("Palavra nao esta presente\n");
		}
	}	
}

//imprimir tabela hash
void imprimir(hash *tabela){
	hash *temp = tabela;
	word *aux;
	for(int i=0; i < (temp->tamanho); i++){
		aux = temp[i].head;
		while(aux != NULL){
			printf("Chave: %d\tPalavra: %s\n", i, aux->palavra);
			aux = aux -> proximo;
		}
	}
}

//busca palavra na hash
int buscar(hash *tabela, char *palavra){
	hash *temp = tabela;
	int posicaoHash = hashing(palavra, temp->tamanho);
	
	word *aux = temp[posicaoHash].head;
	
	if (!aux){
		return 0;
	}else{
		while(aux!=NULL){
			if (strcmp(aux->palavra, palavra)==0){
				return 1;
			}
			aux = aux->proximo;
		}
	}
	return 0;
}

//calcula fator de carga
int fatorDeCarga(hash *tabela){
	return (tabela->contElem) / (tabela->tamanho);
}

//funcao rehash
hash* rehash(hash *tabela){
	//printf("rehash\n");
	hash *temp = tabela;
	word *aux;
	hash *novaHash = (hash *) malloc (sizeof(hash)*(2*temp->tamanho));
	int posicao = 0; 
	
	novaHash->contElem=0;
	novaHash->tamanho=2*(temp->tamanho); //dobra o tamanho da tabela hash
	
	for(int i=0; i < (temp->tamanho); i++){ //copia os elementos pra nova hash
		aux = temp[i].head;
		while(aux != NULL){
			posicao = hashing(aux->palavra, novaHash->tamanho);
			inserirHash(novaHash, aux->palavra, posicao);
			aux = aux -> proximo;
		}
	}	
	free(temp);
	free(aux);
	free(tabela);
	return novaHash;
}

//Libera lista de palavras sugeridas
void liberaHash(hash *table){
	hash *p = table;
	while(p->head!=NULL){
		word *t = p->head->proximo;
		free(p);
		p->head = t;
	}
	free(table);
}

//verifica se a palavra de entrada eh valida
int verificarPalavra(char *palavra){ //verifica se a palavra eh valida
	int hifen=0;
	if(palavra[0]==45){
		return 0;
	}else{
		for(int i=0; palavra[i]!='\0'; i++){			
			if((palavra[i]>-1)&&((palavra[i]<45) || //tem um caracter invalido
			((palavra[i]>45)&&(palavra[i]<65)) ||
			((palavra[i]>90)&&(palavra[i]<97)) ||
			((palavra[i]>122)&&(palavra[i]<128)) ||
			((palavra[i]>131)&&(palavra[i]<135)) ||
			((palavra[i]>135)&&(palavra[i]<149)) ||
			((palavra[i]>151)&&(palavra[i]<160)) || 
			((palavra[i]>163)&&(palavra[i]<181)))){
				return 0;
			}
			
			if(palavra[i]==45){ 
				hifen++;
			}
		}
	}
	if(hifen>1){ //se a palavra tiver mais de um hifen da erro
		return 0;
	}
	return 1;
}

//converte a palavra de entrada para minusculo
char* minusculo(char *palavra){
	for(int i=0; palavra[i]!='\0'; i++){
		if((palavra[i]>64)&&(palavra[i]<91)){
			palavra[i] += 32;
		}else if(palavra[i]==128){
			palavra[i] += 7;
		}
	}
	return palavra;
}

/**
 * LISTA DE PALAVRAS SUGERIDAS
 * **/
//Adiciona na lista as palavras sugeridas de forma ordenada
lista* listAddOrdenado(lista *list, char *palavra){
	lista *novo;
	lista *a = NULL;
	lista *p = list;
	
	while(p != NULL && (strcmp(p->palavra, palavra)<0)){
		a = p;
		p = p->proximo;
	}
	novo = (lista *) malloc(sizeof(lista));
	strcpy(novo->palavra, palavra);
	
	if(a == NULL){
		novo->proximo = list;
		list = novo;
	}else{
		novo->proximo = a->proximo;
		a->proximo = novo;
	}
	return list;
}

//Imprime lista com as palavras sugeridas
void listImprime(lista *list){
	lista *p;
	for(p = list; p!=NULL; p=p->proximo){
		printf("%s\n", p->palavra);
	}
}

//Verifica se a lista de palavras sugeridas esta vazia
int listVazia(lista* list){
	return (list == NULL);
}

//Libera lista de palavras sugeridas
void listLibera(lista *list){
	lista *p = list;
	while(p!=NULL){
		lista *t = p->proximo;
		free(p);
		p = t;
	}
}

//Busca palavra na lista de palavras sugeridas
int listBusca(lista *list, char *palavra){
	lista *p;
	for(p = list; p != NULL; p=p->proximo){
		if(strcmp(p->palavra, palavra)==0){
			return 1;
		}
	}
	return 0;
}

//Palavra contem letra errada
lista* letraErrada(hash *tabela, char *palavra, lista *list){ //verifica se a palavra é válida
	hash *table = tabela;
	int flag=0;
	char auxPalavra[100];
	
	strcpy(auxPalavra, palavra);
	for(int i=0; palavra[i]!='\0'; i++){
		for(int j=97; j<123; j++){
			auxPalavra[i]=j;
			if(listBusca(list, auxPalavra)==0){ //Se a palavra sugestao nao esta na lista
				if (buscar(table, auxPalavra)==1){ //Se tem a palavra na hash
					flag=1;
					list = listAddOrdenado(list, auxPalavra);
				}
			}	
		}
		strcpy(auxPalavra, palavra);
	}
	
	if(flag==1)
		return list;
	else
		return NULL;
}

//Palavra contem letra trocada
lista* letraTrocada(hash *tabela, char *palavra, lista *list){
	hash *table = tabela;
	int flag=0;
	char auxPalavra[100];
	char temp;
	strcpy(auxPalavra, palavra);
	
	for(int i=0; i < strlen(auxPalavra)-1; i++){
		temp = auxPalavra[i];
		auxPalavra[i] = auxPalavra[i+1];
		auxPalavra[i+1] = temp;
		
		if (buscar(table, auxPalavra)==1){ //Se tem a palavra na hash
			if(listBusca(list, auxPalavra)==0){ //Se a palavra sugestao nao esta na lista
				flag=1;
				list = listAddOrdenado(list, auxPalavra);
			}
		}
		strcpy(auxPalavra, palavra);
	} 
	
	if(flag==1)
		return list;
	else
		return NULL;
}

//Palavra contem letra menos
lista* letraMenos(hash *tabela, char *palavra, lista *list){
	hash *table = tabela;
	int flag=0;
	for(int i=0; i <= strlen(palavra); i++){
		//ANTES DO CARACTER A SER ACRESCENTADO
		char temp[102] = "\0";
		for(int j=0; j<i; j++){
			temp[j]=palavra[j];
		}
		
		//DEPOIS DO CARACTER A SER ACRESCENTADO	
		char temp1[102] = "\0";
		int k = 0;
		for(int j=i; j<strlen(palavra)+1; j++){
			temp1[k]=palavra[j];
			k++;
		}
			
		//CONCATENACAO COM CADA CARACTER
		for(int j=97; j<123; j++){
			char temp2[127] = "\0";
			char aux[2]="\0";	
			aux[0] = j;
			strcpy(temp2, temp);
			strcat(temp2, aux);
			strcat(temp2, temp1);
			
			if(listBusca(list, temp2)==0){ //Se a palavra sugestao nao esta na lista
				if (buscar(table, temp2)==1){ //Se tem a palavra na hash
					flag=1;
					list = listAddOrdenado(list, temp2);
				}
			}
		}
	}
	if(flag==1)
		return list;
	else
		return NULL;
}

//Palavra contem letra a mais
lista* letraMais(hash *tabela, char *palavra, lista *list){
	hash *table = tabela;
	int flag=0;
	for(int i=0; i <= strlen(palavra); i++){
		//ANTES DO CARACTER REMOVIDO A SER TESTADO
		char temp[102] = "\0";
		for(int j=0; j<i; j++){
			temp[j]=palavra[j];
		}
		
		//DEPOIS DO CARACTER REMOVIDO A SER TESTADO	
		char temp1[102] = "\0";
		int k = 0;
		for(int j=i+1; j<strlen(palavra)+1; j++){
			temp1[k]=palavra[j];
			k++;
		}
			
		char temp2[102] = "\0";
		strcpy(temp2, temp);
		strcat(temp2, temp1);
		
		if(listBusca(list, temp2)==0){ //Se a palavra sugestao nao esta na lista
			if (buscar(table, temp2)==1){ //Se tem a palavra na hash
				flag=1;
				list = listAddOrdenado(list, temp2);
			}
		}
	}
	
	if(flag==1)
		return list;
	else
		return NULL;
}

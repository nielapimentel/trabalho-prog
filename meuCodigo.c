#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int vida;
  int posX;
  int pista;
  char forma[2][3];
} tGalinha;

typedef struct {
  char direcao;
  int velocidade;
  int posX;
  int pista;
  int endereco;
  char forma[2][3];
} tCarro;

typedef struct {
  int alturaMax;
  int morteMax;
  int morteMin;
  int movTotais;
  int movOpostos;
} tEstatistica;

void VerificaQtdArgum (int argc);
void LeArquivos(char argv[], int tam, char diretorio[]);
void VerificaArgum ( int comprimento);


int main(int argc, char * argv[]){
  printf ("iniciando o jogo\n");
  
  VerificaArgum (argc);
  
  //printf ("argc: %d\n", argc);
  //printf("Argumento: %s\n", argv[1]);
  
  char diretorio[1100]; // verificar erro de mais de 1000 carac

  LeArquivos(argv[1], 1100, diretorio);
  
  //printf ("diretorio: %s\n", diretorio);
  
  
  
  //carregando e lendo arquivo de configuracao incial
  FILE *fp;
  char buffer[1000];
  char arquivoDeTexto[] = "/config_inicial.txt";
  char diretorio_config_inicial[1100];
  
  strcpy(diretorio_config_inicial, diretorio);
  
  strcat(diretorio_config_inicial, arquivoDeTexto); //caminho+config_inicial.txt
  printf("\n%s\n", diretorio_config_inicial);
  
  fp = fopen(diretorio_config_inicial, "r");
  if (fp == NULL) {
    perror("Erro ao abrir o arquivo configuracao inicial");
    exit(1);
  }
  
  while (fgets(buffer, sizeof(buffer), fp) != NULL) { //le o arquivo ate ficar NULL
    printf("%s", buffer); // Imprime a linha
  }
  printf ("\n");
  fclose(fp);

  //separando as vriaveis que vamos usar no futuro


  
  //carregando e lendo arquivo de personagem
  
  FILE *person;
  char arquivoDePersonagem[] = "/personagens.txt";
  char diretorio_personagens[1100];
  
  strcpy(diretorio_personagens, diretorio);
  
  strcat(diretorio_personagens, arquivoDePersonagem);
  printf ("\n%s\n", diretorio_personagens);
  
  person = fopen(diretorio_personagens, "r");
  if (person == NULL) {
    perror("Erro ao abrir o arquivo personagens");
    exit(1);
  }
  
  while (fgets(buffer, sizeof(buffer), person) != NULL) {
    printf("%s", buffer); // Imprime a linha
  }
  printf ("\n");
  fclose(person);

  //FILE *inicializacao;
  //char arquivoDeInicializacao[] = 
  
  
  printf ("finalizando o jogo\n");
}

void VerificaQtdArgum (int argc){
  if (argc==1) {
    printf ("ERRO: Informe o diretorio com os arquivos de configuracao.\n");
    exit(1); // falta o diretorio
  }
}
void LeArquivos(char argv[], int tam, char diretorio[]){
  int comprimento = strlen(argv);
  VerificaArgum(comprimento);
  strcpy(diretorio, argv);

  //printf("comprimento: %d\n", comprimento);
  //printf("diretorio: %s\n", diretorio);
}

void VerificaArgum(int comprimento){
  if (comprimento > 1000){
    printf("ERRO: Informe um caminho com no maximo 1000 caracteres.\n");
    exit(1);
  }
}
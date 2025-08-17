#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int animacao;
  int vida;
  int posX;
  int pista;
  char forma[2][3];
} tGalinha;

typedef struct {
  //char direcao;
  //int velocidade;
  int posX;
  //int pista;
  //int endereco[11];
  //char forma[2][3];
} tCarro;

typedef struct {
  //int largura;
  //int qtdPist;
  tCarro carros[10];
  int qtdCarros;
  //tGalinha galinha;
  char direcao;
  int velocidade;
} tPista;

typedef struct {
  int sucesso;
  char digitada; // w, s ou espaco
} tJogada;

typedef struct {
  tPista pista[12];
  int qtdPista;
  int largura;
  tJogada jogada;
  int pontuacao;
  int iteracao;
  int animacao;
  char formaCarro[2][3];
  //tCarro carro[12][10];
  //int numCarro[10];
  tGalinha galinha;
} tJogo;

typedef struct {
  int alturaMax;
  int morteMax;
  int morteMin;
  int movTotais;
  int movOpostos;
} tEstatistica;



//tJogo LeArquivos(char argv[], int tam, char diretorio[]);

//funcoes da main
tJogo InicializarJogo(int argc, char * argv[]); //feito
tJogo ReailizarJogo (tJogo jogo, char * argv[]);
void GerarResumo (tJogo jogo);
//void GerarEstatisticas (tJogo jogo);
//void GerarRanking (tJogo jogo);
//void GerarMapaDeCalor (tJogo jogo);


int main(int argc, char * argv[]){
  tJogo jogao; //coloca como global depois
  
  jogao = InicializarJogo(argc, argv); 
  jogao = ReailizarJogo(jogao, argv);
  
  //GerarResumo(jogao);
  //GerarEstatisticas (jogao);
  //GerarRanking (jogao);
  //GerarMapaDeCalor (jogao);
  
  return 0;
}

// funcoes de Inicializar Jogo
void VerificaQtdArgum (int argc); //verifica se informou o diretorio
void VerificaArgum (char * argv[], int tamMax); //verifica se o diretorio te no max 1000
tJogo LeArquivos(char * argv[]);
void ImprimeMapaArq (FILE * arq, tJogo jogo);
void GerarArqIni (tJogo jogo, char * argv[]); // inicializacao.txt
void ImprimeTopoArq( FILE * arq, tJogo jogo);
void ImprimePistaArq (FILE * arq, tJogo jogo, tPista pista);
void ImprimeUltimaPistaArq (FILE * arq, tJogo jogo);
void ImprimeDivisaoArq (FILE * arq, tJogo jogo);

tJogo InicializarJogo(int argc, char * argv[]){

  VerificaQtdArgum(argc);
  VerificaArgum (argv, 1000); //0-999 carac +1 pro \0= 1000
  
  tJogo jogo;
  
  jogo = LeArquivos (argv);
  GerarArqIni (jogo, argv);

  //debugLeArquivos na raca
  // int i=0, j=0;
  // printf("animacao:%d\n", jogo.animacao);
  // printf("largura:%d qtdPista:%d\n", jogo.largura, jogo.qtdPista);
  //  for (i=0; i<jogo.qtdPista-1; i++){
  //   printf ("qtdCarros da pista: %d\n", jogo.pista[i].qtdCarros);
  //   if (jogo.pista[i].qtdCarros!=0){
  //     printf ("direcao: %c ", jogo.pista[i].direcao);
  //     printf ("velocidade: %d\n", jogo.pista[i].velocidade);
  //     for (j=0; j<jogo.pista[i].qtdCarros; j++) {
  //       printf ("carro%d: %d ", j, jogo.pista[i].carros[j].posX);
  //     }
  //     printf ("\n");
  //   }
  // }
  // printf("%d %d\n",jogo.galinha.posX, jogo.galinha.vida);
  // printf ("GALINHA:\n");
  // for (i=0; i<2; i++){
  //   for (j=0; j<3; j++){
  //     printf("%c", jogo.galinha.forma[i][j]);
  //   }
  //   printf ("\n");
  // }
  // printf ("\n");
  // printf ("CARRO:\n");
  // for (i=0; i<2; i++){
  //   for (j=0; j<3; j++){
  //     printf("%c", jogo.formaCarro[i][j]);
  //   }
  //   printf ("\n");
  // }

  return jogo;
}

void VerificaQtdArgum (int argc){
  if (argc==1) {
    printf ("ERRO: Informe o diretorio com os arquivos de configuracao.\n");
    exit(1); // falta o diretorio
  }
}

void VerificaArgum(char * argv[], int tamMax){
  int comprimento;
  char diretorio[1001];

  comprimento= strlen (argv[1]);
  if (comprimento > tamMax){ 
    printf("ERRO: Informe um caminho com no maximo 1000 caracteres.\n");
    exit(1);
  }
}

tJogo LeArquivos(char * argv[]){
  char diretorio_config[1021]; //ja com espaco pro \0
  char diretorio_person[1018]; //ja com espaco pro \0
  char diretorio[1001];

  strcpy(diretorio, argv[1]);
  
  strcpy (diretorio_config, diretorio);
  strcpy (diretorio_person, diretorio);
  
  strcat (diretorio_config, "/config_inicial.txt"); 
  strcat (diretorio_person, "/personagens.txt");

  tJogo jogo;

  //le config_inicial
  FILE *config = fopen (diretorio_config, "r");
  if (config==NULL) {
    printf ("Erro ao abrir arquivo config_inicial.txt.\nCaminho informado: %s", diretorio);
    exit(1);
  }

  char lixo;//consome \n

  fscanf(config, "%d%*[^\n]", &jogo.animacao);
  fscanf (config, "%c", &lixo); 
  fscanf(config, "%d %d%*[^\n]", &jogo.largura, &jogo.qtdPista);
  fscanf (config, "%c", &lixo);
  //printf ("\n lixo:%c.\n", lixo);
  //fscanf (config, "%c", &lixo);
  //printf ("\n lixo:%c.\n", lixo);

  int i, j=0;
  char carac;

  for (i=0; i<jogo.qtdPista; i++){
    fscanf(config, "%c", &carac);
    if (carac=='\n'){
      jogo.pista[i].qtdCarros=0;
    } else if (carac=='E' || carac=='D') {
      jogo.pista[i].direcao= carac;
      fscanf(config, "%d %d", &jogo.pista[i].velocidade, &jogo.pista[i].qtdCarros); 
      
      for (j=0; j<jogo.pista[i].qtdCarros; j++){
        fscanf(config, "%d", &jogo.pista[i].carros[j].posX);
        //jogo.carro[i][j].endereco[j]=j+1; //carro 1 vai ser o um ao inves do 0
      }
      fscanf(config, "%c", &lixo);
    } else if (carac=='G') { 
      fscanf(config, "%d %d", &jogo.galinha.posX, &jogo.galinha.vida);
    } else {
      printf ("Erro ao ler config_inicial.txt"); //nao eh pra isso acontecer nunca
    }
  }
  fclose(config);

  //le personagens (ate agora sem animacao bonus)
  FILE *person = fopen(diretorio_person, "r");
  if (person==NULL) {
    printf ("Erro ao abrir arquivo pesonagens.txt.\nCaminho informado: %s", diretorio);
    exit(1);
  }

  for(i=0; i<2; i++){
    for (j=0; j<3; j++){
      fscanf (person, "%c", &jogo.galinha.forma[i][j]);
    }
    fscanf(person, "%c", &lixo); //consome o \n
  }

  for(i=0; i<2; i++){
    for (j=0; j<3; j++){
      fscanf (person, "%c", &jogo.formaCarro[i][j]);
    }
    fscanf(person, "%c", &lixo); //consome o \n
  }

  fclose(person);

  return jogo;
}

void GerarArqIni (tJogo jogo, char * argv[]){

  strcat (argv[1],"saida/inicializacao.txt");
  //printf ("%s", argv[1]);

  FILE *inicia = fopen(argv[1], "w");
  if (inicia==NULL) {
    printf ("Erro: pasta 'saida' nao existe!"); //no documento disse que ela ja esta criada, so pra garantir
    exit(1);
  }
  int i,j;

  // printf("DEBUG: qtdPista=%d\n", jogo.qtdPista);
  // printf("DEBUG: posX=%d\n", jogo.galinha.posX);

  ImprimeMapaArq (inicia, jogo);
  fprintf (inicia, "A posicao central da galinha iniciara em (%d %d).", jogo.galinha.posX, jogo.qtdPista*3-2);
  fclose (inicia);
}

void ImprimeMapaArq (FILE * arq, tJogo jogo){
  int i,j,k;

  ImprimeTopoArq (arq, jogo);
  for (i=0; i<jogo.qtdPista-1; i++) { //nao imprimo a pista da galinha
    ImprimePistaArq(arq, jogo, jogo.pista[i]);
  }

  ImprimeUltimaPistaArq (arq, jogo);
}

void ImprimeUltimaPistaArq (FILE * arq, tJogo jogo){
  int i,j;
  char linha1[101]={0};
  char linha2[101]={0};  

  for (i=0; i<jogo.largura; i++){
    linha1[i]=' ';
    linha2[i]=' ';
  }
  linha1[i]='\0';
  linha2[i]='\0';  
  
  j= jogo.galinha.posX -1;
  linha1[j-1]= jogo.galinha.forma[0][0];
  linha1[j]= jogo.galinha.forma[0][1];
  linha1[j+1]= jogo.galinha.forma[0][2];
  
  linha2[j-1]= jogo.galinha.forma[1][0];
  linha2[j]= jogo.galinha.forma[1][1];
  linha2[j+1]= jogo.galinha.forma[1][2];

  fprintf(arq, "|%s|\n", linha1);
  fprintf(arq, "|%s|\n", linha2);
  ImprimeTopoArq(arq, jogo);
}

void ImprimeTopoArq( FILE * arq, tJogo jogo) {
  int i,j;

  fprintf(arq, "|");
  for (i=0; i<jogo.largura; i++){
    fprintf (arq, "-");
  }
  fprintf(arq, "|\n");
}

void ImprimePistaArq (FILE * arq, tJogo jogo, tPista pista) {
  int i,j;
  char linha1[101]={0};
  char linha2[101]={0};

  //deixando minhas duas linhas em branco
  for (i=0; i<jogo.largura; i++){
    linha1[i]=' ';
    linha2[i]=' ';
  }
  linha1[i]='\0';
  linha2[i]='\0';

  //alocando carros na pista
  for (i=0; i<pista.qtdCarros; i++){
    j= pista.carros[i].posX -1;
    linha1[j-1]= jogo.formaCarro[0][0];
    linha1[j]= jogo.formaCarro[0][1];
    linha1[j+1]= jogo.formaCarro[0][2];

    linha2[j-1]= jogo.formaCarro[1][0];
    linha2[j]= jogo.formaCarro[1][1];
    linha2[j+1]= jogo.formaCarro[1][2];
  }

  fprintf(arq, "|%s|\n", linha1);
  fprintf(arq, "|%s|\n", linha2);
  ImprimeDivisaoArq(arq, jogo);

}

void ImprimeDivisaoArq (FILE * arq, tJogo jogo){
  int i,j;

  fprintf(arq, "|");
  for (i=0; i<jogo.largura/3; i++){
    fprintf (arq, "-- ");
  }

  if (jogo.largura%3==1){
    fprintf (arq, "-");
  } else if (jogo.largura%3==2){
    fprintf (arq, "--");
  }

  fprintf (arq, "|\n");
}




//funcoes do RealizarJogo
int GalinhaGanhou (tJogo jogo); // retorna 1 se ganhou
int GalinhaDerrotada (tJogo jogo); //perdeu todas as vidas - retora 1
//tJogada LeJogada ();
tJogo MoveGalinha (tJogo jogo, tJogada jogada); // adiciona 1 ponto se for w
tJogo MovimentaCarros (tJogo jogo); 
int VerificaColisao (tJogo jogo); //passar pela matriz de carros e ver se algum x e y bate com o da galinha

void ImprimeJogo (tJogo jogo);
void ImprimeMapa (tJogo jogo);
void ImprimeTopo (tJogo jogo);
void ImprimePista (tJogo jogo, tPista pista, int galinha);
void ImprimeDivisao(tJogo jogo);
void ImprimeUltimaPista (tJogo jogo);

tJogo ReailizarJogo (tJogo jogo, char * argv[]){

  //O jogo terminará quando a galinha cruzar todas as pistas, consequentemente
  //chegando no topo do mapa, ou quando perder todas as vidas.
  jogo.iteracao=0;
  jogo.pontuacao=0;

  jogo.galinha.pista=jogo.qtdPista; //galinha na ultima pista

  ImprimeJogo(jogo);

  while (1) {
    if (GalinhaGanhou(jogo)){
      jogo.pontuacao= jogo.pontuacao+10;
      ImprimeJogo(jogo);
      printf ("Parabens! Voce atravessou todas as pistas e venceu!");
      break;
    } else if (GalinhaDerrotada(jogo)) {
      ImprimeJogo(jogo);
      printf ("Voce perdeu todas as vidas! Fim de jogo.");
      break;
    } else { //galinha NAO derrotada NEM vitoriosa
      ImprimeJogo(jogo);
      scanf ("%c", &jogo.jogada.digitada);
      jogo= MoveGalinha (jogo, jogo.jogada); // adiciona 1 ponto se for pra frente
      jogo= MovimentaCarros (jogo);
      if (VerificaColisao(jogo)) { // 1==colidiu
        jogo.galinha.vida--;
        jogo.pontuacao=0;
        jogo.galinha.pista=0; //nao sei como vai funcionar ainda mais tem q voltar pra posicao inicial
      } else{
        if (jogo.jogada.digitada=='w'){
          jogo.pontuacao++;
        }
      }
      jogo.iteracao++;
    }
  }
  
  return jogo;
}

int GalinhaGanhou (tJogo jogo){
  int i;
  if (jogo.galinha.pista==0) {
    return 1; //ganhou
  }
  return 0;
}
int GalinhaDerrotada (tJogo jogo){
  if (jogo.galinha.vida==0){
    return 1;
  } return 0;
}

tJogo MoveGalinha (tJogo jogo, tJogada jogada){
  if (jogada.digitada=='w'){
    jogo.galinha.pista--; //sobe uma pista
    jogo.pontuacao++; //ganha 1 ponto se foi pra frente
  } else if (jogada.digitada=='s'){
    if (jogo.galinha.pista<jogo.qtdPista) { //nao esta na pista mais inferior
      jogo.galinha.pista++; //desce uma pista
    }
  }
  return jogo;
}


void ImprimeJogo (tJogo jogo){
  printf ("Pontos: %d | Vidas: %d | Iteracoes: %d\n", jogo.pontuacao, jogo.galinha.vida, jogo.iteracao);
  ImprimeMapa (jogo);
}

void ImprimeMapa (tJogo jogo){
  int i,j,k;

  ImprimeTopo (jogo);
  for (i=0; i<jogo.qtdPista-1; i++) { //nao imprimo a pista da galinha
    if (i==jogo.galinha.pista) {
      ImprimePista(jogo, jogo.pista[i], 1); //tem galinha nessa pista
    } else {
      ImprimePista(jogo, jogo.pista[i], 0);
    }
  }

  ImprimeUltimaPista (jogo);
}

void ImprimeTopo (tJogo jogo){
  int i;
  printf ("|");
  for (i=0; i<jogo.largura; i++){
    printf ("-");
  }
  printf ("|\n");
}

void ImprimePista (tJogo jogo, tPista pista, int galinha){
  int i, j;
  char linha1[101]= {0};
  char linha2[101]= {0};

  //deixando minhas duas linhas em branco
  for (i=0; i<jogo.largura; i++){
    linha1[i]=' ';
    linha2[i]=' ';
  }
  linha1[i]='\0';
  linha2[i]='\0';

  //alocando carros na pista
  for (i=0; i<pista.qtdCarros; i++){
    j= pista.carros[i].posX -1;
    linha1[j-1]= jogo.formaCarro[0][0];
    linha1[j]= jogo.formaCarro[0][1];
    linha1[j+1]= jogo.formaCarro[0][2];

    linha2[j-1]= jogo.formaCarro[1][0];
    linha2[j]= jogo.formaCarro[1][1];
    linha2[j+1]= jogo.formaCarro[1][2];
  }
    
  printf ("|%s|\n", linha1);
  printf ("|%s|\n", linha2);
  ImprimeDivisao (jogo);
}

void ImprimeDivisao(tJogo jogo){
  int i;
  printf ("|");
  for (i=0; i<jogo.largura/3; i++){
    printf ("-- ");
  }
  if (jogo.largura%3 == 1){
    printf ("-");
  } else if (jogo.largura%3== 2){
    printf ("--");
  }
  printf ("|\n");
}

void ImprimeUltimaPista (tJogo jogo){
  int i,j;
  char linha1[101]={0};
  char linha2[101]={0};  

  for (i=0; i<jogo.largura; i++){
    linha1[i]=' ';
    linha2[i]=' ';
  }
  linha1[i]='\0';
  linha2[i]='\0';  
  
  j= jogo.galinha.posX -1;
  linha1[j-1]= jogo.galinha.forma[0][0];
  linha1[j]= jogo.galinha.forma[0][1];
  linha1[j+1]= jogo.galinha.forma[0][2];
  
  linha2[j-1]= jogo.galinha.forma[1][0];
  linha2[j]= jogo.galinha.forma[1][1];
  linha2[j+1]= jogo.galinha.forma[1][2];

  printf("|%s|\n", linha1);
  printf("|%s|\n", linha2);
  ImprimeTopo(jogo);
}

// void GerarResumo (tJogo jogo){
  
// }






// int main(int argc, char * argv[]){
//   printf ("iniciando o jogo\n");
  
//   VerificaQtArgum (argc); // verifica se passou o caminho
  
//   //printf ("argc: %d\n", argc);
//   //printf("Argumento: %s\n", argv[1]);
  
//   char diretorio[1001]; // verificar erro de mais de 1000 carac
  
//   //printf ("diretorio: %s\n", diretorio);
  
  
  
//   //carregando e lendo arquivo de configuracao incial
//   // FILE *conf;
//   // char buffer[1000];
//   // char arquivoDeTexto[] = "/config_inicial.txt"; // ja com o \0 - 20 espacos
//   // char diretorio_config_inicial[1020]; //max de 1000+20
  
//   // strcpy(diretorio_config_inicial, diretorio);
  
//   // strcat(diretorio_config_inicial, arquivoDeTexto); //caminho+config_inicial.txt
//   // //printf("\n%s\n", diretorio_config_inicial);
  
//   // conf = fopen(diretorio_config_inicial, "r");
//   // if (conf == NULL) {
//   //   printf ("Erro ao abrir o arquivo configuracao inicial");
//   //   exit(1);
//   // }
  
//   // while (fgets(buffer, sizeof(buffer), conf) != NULL) { //le o arquivo ate ficar NULL
//   //   printf("%s", buffer); // Imprime a linha
//   // }
//   // printf ("\n");
//   // fclose(conf);

//   //separando as vriaveis que vamos usar no futuro


  
//   //carregando e lendo arquivo de personagem
  
//   FILE *person;
//   char arquivoDePersonagem[] = "/personagens.txt"; // ja coloca o \0 no final - 16 espacos
//   char diretorio_personagens[1016]; // contando o \0
  
//   strcpy(diretorio_personagens, diretorio);
  
//   strcat(diretorio_personagens, arquivoDePersonagem);
//   printf ("\n%s\n", diretorio_personagens);
  
//   person = fopen(diretorio_personagens, "r");
//   if (person == NULL) {
//     printf ("Erro ao abrir o arquivo personagens");
//     exit(1);
//   }
  
//   while (fgets(buffer, sizeof(buffer), person) != NULL) {
//     printf("%s", buffer); // Imprime a linha
//   }
//   printf ("\n");
//   fclose(person);

//   //FILE *inicializacao;
//   //char arquivoDeInicializacao[] = 
  
  
//   printf ("finalizando o jogo\n");
// }



// // tJogo LeArquivos(char argv[], int tam, char diretorio[]){
// //   tJogo jogo;
// //   int comprimento = strlen(argv);
// //   VerificaArgum(comprimento, tam); // verifica se passou de 1000 carac
// //   strcpy(diretorio, argv);

// //   //printf("comprimento: %d\n", comprimento);
// //   //printf("diretorio: %s\n", diretorio);

// //   FILE *conf;
// //   char arquivoDeTexto[] = "/config_inicial.txt"; // ja com o \0 - 20 espacos
// //   char diretorio_config_inicial[1020]; //max de 1000+20
  
// //   strcpy(diretorio_config_inicial, diretorio); //copia 2 pro 1
// //   strcat(diretorio_config_inicial, arquivoDeTexto); //caminho+config_inicial.txt
// //   //printf("\n%s\n", diretorio_config_inicial);
  
// //   conf = fopen(diretorio_config_inicial, "r");
// //   if (conf == NULL) {
// //     printf ("Erro ao abrir o arquivo configuracao inicial");
// //     exit(1);
// //   }
  
// //   while (1) {
// //       fscanf (conf, "%d", &jogo.animacao);
// //   }

// //   printf ("\n");
// //   fclose(conf);

// //   return jogo;
// // }



// void CriaPista (tJogo jogo){
//   int i, j;
//   for (i=0; i<)
// }

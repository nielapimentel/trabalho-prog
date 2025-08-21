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
  int posX;
} tCarro;

typedef struct {
  tCarro carros[10];
  int qtdCarros;
  int direcao;
  int velocidade;
  int countHeatMap;
} tPista;

typedef struct {
  char digitada; // w, s ou espaco
} tJogada;

typedef struct {
  //dados necessarios pro heatmap
  int iteracao;
  int pista;
  int carro;
  int x; //pos central
  int y; //pos superior
} tMorte;

typedef struct {
  //dados necessrios pro arquivo estatistica.txt
  int alturaMax;
  int alturaAtual;
  int morteMax;
  int morteMin;
  int movTotais;
  int movOpostos;
} tEstatistica;

typedef struct {
  //dados da config inicial
  int animacao;
  int qtdPista;
  int largura;

  //dados dos personagens
  tGalinha galinha;
  char formaCarro[4][2][3];

  tPista pista[12];
  tJogada jogada;
  int formaAtual; //utilizado pra animacao
  int pontuacao;
  int iteracao;

  int colisao;
  tEstatistica estatistica;

  tMorte morte[20]; //max de vidas - max de mortes
  int qtdMortes;
  int HeatMapMatriz[35][100];
} tJogo;

//funcoes da main
tJogo InicializarJogo(int argc, char * argv[]); //le os arquivos e produz a inicializacao.txt
tJogo ReailizarJogo (tJogo jogo, char * argv[]); // realiza todo o jogo e gera a saida do programa no terminal
void GerarResumo (tJogo jogo, char * argv[]); 
void GerarEstatisticas (tJogo jogo, char * argv[]); 
void GerarRanking (tJogo jogo, char * argv[]);
void GerarMapaDeCalor (tJogo jogo, char * argv[]);


tJogo jogao; 
int main(int argc, char * argv[]){
  
  jogao = InicializarJogo(argc, argv); 
  jogao = ReailizarJogo(jogao, argv);
  
  GerarResumo(jogao, argv);
  GerarEstatisticas (jogao, argv);
  GerarRanking (jogao, argv);
  GerarMapaDeCalor (jogao, argv);
  
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
  VerificaArgum (argv, 1000); 
  
  tJogo jogo;
  
  jogo = LeArquivos (argv);
  GerarArqIni (jogo, argv);

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
  char diretorio_config[1021]; 
  char diretorio_person[1018]; 
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

  int i, j, k;
  char carac;

  //le configracao inicial
  for (i=0; i<jogo.qtdPista; i++){
    fscanf(config, "%c", &carac);
    if (carac=='\n'){
      jogo.pista[i].qtdCarros=0;
    } else if (carac=='E' || carac=='D') {
      jogo.pista[i].direcao= carac;
      fscanf(config, "%d %d", &jogo.pista[i].velocidade, &jogo.pista[i].qtdCarros); 
      
      for (j=0; j<jogo.pista[i].qtdCarros; j++){
        fscanf(config, "%d", &jogo.pista[i].carros[j].posX);
      }
      fscanf(config, "%c", &lixo);
    } else if (carac=='G') { 
      fscanf(config, "%d %d", &jogo.galinha.posX, &jogo.galinha.vida);
      jogo.pista[i].qtdCarros=0;
    } else {
      printf ("Erro ao ler config_inicial.txt"); //nao eh pra isso acontecer nunca
    }
  }
  fclose(config);

  //le personagens = todos os carros independentemente se tem ou nao animacao
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

  for (k=0; k<4; k++){
    for(i=0; i<2; i++){
      for (j=0; j<3; j++){
        fscanf (person, "%c", &jogo.formaCarro[k][i][j]);
      }
      fscanf(person, "%c", &lixo); //consome o \n
    }
  }

  fclose(person);

  return jogo;
}
void GerarArqIni (tJogo jogo, char * argv[]){

  char diretorio[1000];
  strcpy (diretorio, argv[1]);

  strcat (diretorio,"/saida/inicializacao.txt");

  FILE *inicia = fopen(diretorio, "w");
  if (inicia==NULL) {
    printf ("Erro: pasta 'saida' nao existe!"); //no documento disse que ela ja esta criada, so pra garantir
    exit(1);
  }
  int i,j;


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

  //linhas em branco
  for (i=0; i<jogo.largura; i++){
    linha1[i]=' ';
    linha2[i]=' ';
  }
  linha1[i]='\0';
  linha2[i]='\0';  
  
  //alocando a galinha
  j= jogo.galinha.posX-1; //substituindo o indice indo de 1 a 4 por exemplo, pra de 0 a 3
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

  //deixei as duas linhas em branco
  for (i=0; i<jogo.largura; i++){
    linha1[i]=' ';
    linha2[i]=' ';
  }
  linha1[i]='\0';
  linha2[i]='\0';

  //alocando carros na pista
  for (i=0; i<pista.qtdCarros; i++){
    j= pista.carros[i].posX -1;
    linha1[(j-1+jogo.largura)%jogo.largura]= jogo.formaCarro[0][0][0];
    linha1[(j+jogo.largura)%jogo.largura]= jogo.formaCarro[0][0][1];
    linha1[(j+1+jogo.largura)%jogo.largura]= jogo.formaCarro[0][0][2];

    linha2[(j-1+jogo.largura)%jogo.largura]= jogo.formaCarro[0][1][0];
    linha2[(j+jogo.largura)%jogo.largura]= jogo.formaCarro[0][1][1];
    linha2[(j+1+jogo.largura)%jogo.largura]= jogo.formaCarro[0][1][2];
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
char LeJogada ();
tJogo MoveGalinha (tJogo jogo, tJogada jogada); 
tJogo MovimentaCarros (tJogo jogo); 
int VerificaColisao (tJogo jogo); 
void ImprimeJogo (tJogo jogo);
void ImprimeMapa (tJogo jogo);
void ImprimeTopo (tJogo jogo);
void ImprimePista (tJogo jogo, tPista pista, int galinha);
void ImprimeDivisao(tJogo jogo);
void ImprimeUltimaPista (tJogo jogo);
tJogo AtualizaFormaCarro (tJogo jogo); //animacao

tJogo ReailizarJogo (tJogo jogo, char * argv[]){

  //inicializando
  jogo.iteracao=0;
  jogo.pontuacao=0;
  jogo.qtdMortes=0;

  jogo.galinha.pista=jogo.qtdPista-1; //galinha na ultima pista
  char lixo; //consome \n

  //inicializando estatisticas
  jogo.estatistica.alturaAtual=2;
  jogo.estatistica.alturaMax=0;
  jogo.estatistica.morteMax=0;
  jogo.estatistica.morteMin=0;
  jogo.estatistica.movOpostos=0;
  jogo.estatistica.movTotais=0;

  //inicializando a pista da galinha que ja comeca com 1
  jogo.pista[jogo.qtdPista-1].countHeatMap=1; 

  //inicializando a forma do carro que vai ser imprimida 
  jogo.formaAtual= 0;

  while (1) {

    if (GalinhaGanhou(jogo)){
      ImprimeJogo(jogo);
      printf ("Parabens! Voce atravessou todas as pistas e venceu!\n");
      break;
    } else if (GalinhaDerrotada(jogo)) {
      ImprimeJogo(jogo);
      printf ("Voce perdeu todas as vidas! Fim de jogo.\n");
      break;
    } else { //galinha NAO derrotada NEM vitoriosa
      ImprimeJogo(jogo);

      jogo.jogada.digitada= LeJogada();
      jogo= MoveGalinha (jogo, jogo.jogada); 

      jogo.pista[jogo.galinha.pista].countHeatMap++; //contabilizando heatmap apos cada jogada

      jogo= AtualizaFormaCarro (jogo); //troca a forma do carro se tiver animacao
      jogo= MovimentaCarros (jogo);
      jogo.colisao=VerificaColisao(jogo); //jogo.colisao me retorna o indice do carro, se o indice do carro for 0, me retorn -1

      if (jogo.colisao) { // se != de 0 entao colidiu

        //parte da animacao, diminuicao da velocidade da pista se colidir
        if (jogo.pista[jogo.galinha.pista].velocidade>1 && jogo.animacao){
          jogo.pista[jogo.galinha.pista].velocidade--;
        }
        
        //contabilizando heatmap
        jogo.pista[jogo.galinha.pista].countHeatMap=-21; //galinha tem no maximo vinte vidas, entao nunca consegue se tornar positivo
        jogo.pista[jogo.qtdPista-1].countHeatMap++; // voltou pro inicio
        
        //difinindo as primeiras alturas de morte max e min
        if (jogo.qtdMortes==0){
          jogo.estatistica.morteMax= jogo.estatistica.alturaAtual;
          jogo.estatistica.morteMin=  jogo.estatistica.alturaAtual;
        }

        //verificando quais as maiores e menores alturas de MORTE sao
        if (jogo.estatistica.morteMax<jogo.estatistica.alturaAtual){
          jogo.estatistica.morteMax= jogo.estatistica.alturaAtual;
        }
        if (jogo.estatistica.morteMin>jogo.estatistica.alturaAtual){
          jogo.estatistica.morteMin=  jogo.estatistica.alturaAtual;
        }
        
        //inicializando a altura da galinha pra estatistica
        jogo.estatistica.alturaAtual=2;
        
        //atualizando indices do carro para comecar do 1 e nao do 0
        if (jogo.colisao==-1) {
          jogo.morte[jogo.qtdMortes].carro= 1;
        } else {
          jogo.morte[jogo.qtdMortes].carro= jogo.colisao+1;
        }

        //alocando dados pro tmorte;
        jogo.morte[jogo.qtdMortes].iteracao= jogo.iteracao+1; // qtdMortes==0
        jogo.morte[jogo.qtdMortes].x= jogo.galinha.posX;
        
        //consequencias da colisao
        jogo.galinha.vida--;
        jogo.pontuacao=0;
        jogo.galinha.pista=jogo.qtdPista-1; 
        
        jogo.qtdMortes++;
        
      } else { //nao colidiu
        if (jogo.jogada.digitada=='w'){
          jogo.pontuacao++;
          if (jogo.galinha.pista==0) {
            jogo.pontuacao= jogo.pontuacao+10; //ganhou
          }
        }
      }

      //verificando a maior altura que a galinha chegou;
      if (jogo.estatistica.alturaAtual>jogo.estatistica.alturaMax){
        jogo.estatistica.alturaMax= jogo.estatistica.alturaAtual;
      }
    }
    jogo.iteracao++;
  }
  
  return jogo;
}

//atualiza a forma do carro que vai ser impresso, dependendo da animacao ou nao
tJogo AtualizaFormaCarro (tJogo jogo){
  if (jogo.animacao==1){ //com animacao
    jogo.formaAtual= (jogo.formaAtual+1)%4;
  } else { //sem animacao
    jogo.formaAtual=0; 
  }
  return jogo;
}

int GalinhaGanhou (tJogo jogo){
  int i;
  if (jogo.galinha.pista==0) {
    return 1; //ganhou
  } return 0;
}
int GalinhaDerrotada (tJogo jogo){
  if (jogo.galinha.vida==0){
    return 1;
  } return 0;
}
tJogo MoveGalinha (tJogo jogo, tJogada jogada){
  if (jogada.digitada=='w'){

    jogo.galinha.pista--; //sobe uma pista

    jogo.morte[jogo.qtdMortes].pista= jogo.galinha.pista+1; //dados utilizados no ranking
    
    jogo.estatistica.movTotais++;
    jogo.estatistica.alturaAtual += 3;

  } else if (jogada.digitada=='s'){

    jogo.estatistica.movTotais++;
    jogo.estatistica.movOpostos++;

    if (jogo.galinha.pista<jogo.qtdPista-1) { //nao esta na pista mais inferior
      jogo.galinha.pista++; //desce uma pista

      jogo.morte[jogo.qtdMortes].pista= jogo.galinha.pista+1;//dados do ranking

      jogo.estatistica.alturaAtual -= 3;
    } else {
      jogo.morte[jogo.qtdMortes].pista= jogo.galinha.pista;// dados do ranking
    }
  }
  jogo.morte[jogo.qtdMortes].y= jogo.morte[jogo.qtdMortes].pista*3 -2;
  return jogo;
}
tJogo MovimentaCarros (tJogo jogo){
  int i,j, aux=0;
  for (i=0; i<jogo.qtdPista; i++){
    for (j=0; j<jogo.pista[i].qtdCarros; j++) {
      if (jogo.pista[i].direcao=='D'){
        jogo.pista[i].carros[j].posX = jogo.pista[i].carros[j].posX + jogo.pista[i].velocidade;
      } else if (jogo.pista[i].direcao=='E') {
        jogo.pista[i].carros[j].posX= jogo.pista[i].carros[j].posX - jogo.pista[i].velocidade;
      }
      //verificando se nao ulrapassou a borda, e consertando se sim
      jogo.pista[i].carros[j].posX= (jogo.pista[i].carros[j].posX+jogo.largura)%jogo.largura;
    }
  }
  return jogo;
}
char LeJogada (){
  int i;
  char carac;
  
  do {
    scanf("%c", &carac);
  } while (carac!=' ' && carac!='w' && carac!='s');

  return carac;
}
int VerificaColisao (tJogo jogo){
  int i,j, k;

  k= jogo.galinha.pista;// inicializando k como a pista que tem a galinha
  for (i=0; i<jogo.pista[k].qtdCarros; i++){
    for (j=-1; j<2; j++){ //matriz[2][3]= 3 comparacoes pois se encostar na linha de cima encosta na de baixo
      if (jogo.pista[k].carros[i].posX+j==jogo.galinha.posX-1 || jogo.pista[k].carros[i].posX+j==jogo.galinha.posX || jogo.pista[k].carros[i].posX+j==jogo.galinha.posX+1){
        if (i==0){
          return -1; //retornando o indice 0 do carro com outro numero
        }
        return i; //retornando o indice do carro
      }
    }
  }
  return 0;
}
void ImprimeJogo (tJogo jogo){
  printf ("Pontos: %d | Vidas: %d | Iteracoes: %d\n", jogo.pontuacao, jogo.galinha.vida, jogo.iteracao);
  ImprimeMapa (jogo);
}
void ImprimeMapa (tJogo jogo){
  int i,j,k;

  ImprimeTopo (jogo);
  for (i=0; i<jogo.qtdPista-1; i++) { 
    if (i==jogo.galinha.pista) {
      ImprimePista(jogo, jogo.pista[i], 1); //tem galinha nessa pista
    } else {
      ImprimePista(jogo, jogo.pista[i], 0); //sem galinha
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
  int i, j, k;
  char linha1[101]= {0};
  char linha2[101]= {0};

  //deixando minhas duas linhas em branco
  for (i=0; i<jogo.largura; i++){
    linha1[i]=' ';
    linha2[i]=' ';
  }
  linha1[i]='\0';
  linha2[i]='\0';

  //alocando pistas sem carros com galinha
  if (pista.qtdCarros==0 && (galinha)){ 

    //as subtracoes concertam o indice e indicam a posicao que vai ser substituida
      linha1[jogo.galinha.posX-2]= jogo.galinha.forma[0][0];
      linha1[jogo.galinha.posX-1]= jogo.galinha.forma[0][1];
      linha1[jogo.galinha.posX]= jogo.galinha.forma[0][2];

      linha2[jogo.galinha.posX-2]= jogo.galinha.forma[1][0];
      linha2[jogo.galinha.posX-1]= jogo.galinha.forma[1][1];
      linha2[jogo.galinha.posX]= jogo.galinha.forma[1][2];
  }

  //alocando carros na pista
  for (i=0; i<pista.qtdCarros; i++){
    j= pista.carros[i].posX -1;

    linha1[(j-1+jogo.largura)%jogo.largura]= jogo.formaCarro[jogo.formaAtual][0][0];
    linha1[(j+jogo.largura)%jogo.largura]= jogo.formaCarro[jogo.formaAtual][0][1];
    linha1[(j+1+jogo.largura)%jogo.largura]= jogo.formaCarro[jogo.formaAtual][0][2];

    linha2[(j-1+jogo.largura)%jogo.largura]= jogo.formaCarro[jogo.formaAtual][1][0];
    linha2[(j+jogo.largura)%jogo.largura]= jogo.formaCarro[jogo.formaAtual][1][1];
    linha2[(j+1+jogo.largura)%jogo.largura]= jogo.formaCarro[jogo.formaAtual][1][2];

    if (galinha){//tem galinha na pista
      linha1[jogo.galinha.posX-2]= jogo.galinha.forma[0][0];
      linha1[jogo.galinha.posX-1]= jogo.galinha.forma[0][1];
      linha1[jogo.galinha.posX]= jogo.galinha.forma[0][2];

      linha2[jogo.galinha.posX-2]= jogo.galinha.forma[1][0];
      linha2[jogo.galinha.posX-1]= jogo.galinha.forma[1][1];
      linha2[jogo.galinha.posX]= jogo.galinha.forma[1][2];
    }
  }

  if (i==0 && jogo.galinha.pista==i){  //ultima pista
    if (galinha){//tem galinha na pista
      linha1[jogo.galinha.posX-2]= jogo.galinha.forma[0][0];
      linha1[jogo.galinha.posX-1]= jogo.galinha.forma[0][1];
      linha1[jogo.galinha.posX]= jogo.galinha.forma[0][2];

      linha2[jogo.galinha.posX-2]= jogo.galinha.forma[1][0];
      linha2[jogo.galinha.posX-1]= jogo.galinha.forma[1][1];
      linha2[jogo.galinha.posX]= jogo.galinha.forma[1][2];
    }
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

  //linhas em branco
  for (i=0; i<jogo.largura; i++){
    linha1[i]=' ';
    linha2[i]=' ';
  }
  linha1[i]='\0';
  linha2[i]='\0';  

  if (jogo.galinha.pista==jogo.qtdPista-1) { //galinha ganhou
    j= jogo.galinha.posX -1; //consertando indice
    linha1[j-1]= jogo.galinha.forma[0][0];
    linha1[j]= jogo.galinha.forma[0][1];
    linha1[j+1]= jogo.galinha.forma[0][2];
    
    linha2[j-1]= jogo.galinha.forma[1][0];
    linha2[j]= jogo.galinha.forma[1][1];
    linha2[j+1]= jogo.galinha.forma[1][2];
  }

  printf("|%s|\n", linha1);
  printf("|%s|\n", linha2);
  ImprimeTopo(jogo);
}

//funcoes de resumo.txt
void ImprimeResumo (FILE * arq, tJogo jogo);

void GerarResumo (tJogo jogo, char * argv[]){
  char diretorio[1000];
  strcpy (diretorio, argv[1]);

  strcat(diretorio, "/saida/resumo.txt");

  FILE * arq= fopen (diretorio, "w");
  if (arq == NULL) {
        printf("Erro: pasta de saida nao existe!\n"); //sempre deveria existir
        exit (1);
    }

  ImprimeResumo (arq, jogo);
  fclose(arq);
}

void ImprimeResumo (FILE * arq, tJogo jogo){
  int i;

  for (i=0; i<jogo.qtdMortes; i++){
    fprintf(arq, "[%d] Na pista %d o carro %d atropelou a galinha na posicao (%d,%d).\n", 
      jogo.morte[i].iteracao, jogo.morte[i].pista, jogo.morte[i].carro, jogo.morte[i].x, jogo.morte[i].y);
  }
  fprintf(arq, "[%d] Fim de jogo", jogo.iteracao);
}

//funcao de estatistica.txt
void ImprimeEstatisticas (FILE * arq, tJogo jogo);

void GerarEstatisticas (tJogo jogo, char * argv[]){
  char diretorio[1000];
  strcpy (diretorio, argv[1]);
  strcat (diretorio, "/saida/estatistica.txt");

  FILE * arq = fopen(diretorio, "w");
  if (arq==NULL){
    printf("Erro: pasta de saida nao existe!\n"); //sempre deveria existir
    exit (1); 
  }

  ImprimeEstatisticas(arq, jogo);
  fclose(arq);
}

void ImprimeEstatisticas (FILE * arq, tJogo jogo){
  fprintf (arq, "Numero total de movimentos: %d\n", jogo.estatistica.movTotais);
  fprintf (arq, "Altura maxima que a galinha chegou: %d\n", jogo.estatistica.alturaMax);
  fprintf (arq, "Altura maxima que a galinha foi atropelada: %d\n", jogo.estatistica.morteMax);
  fprintf (arq, "Altura minima que a galinha foi atropelada: %d\n", jogo.estatistica.morteMin);
  fprintf (arq, "Numero de movimentos na direcao oposta: %d\n", jogo.estatistica.movOpostos);  
}

//funcao de ranking.txt
void ImprimeRanking(FILE * arq, tJogo jogo);

void GerarRanking (tJogo jogo, char * argv[]){
  char diretorio[1000];
  strcpy (diretorio, argv[1]);
  strcat (diretorio, "/saida/ranking.txt");

  FILE * arq = fopen(diretorio, "w");
  if (arq==NULL){
    printf("Erro: pasta de saida nao existe!\n"); //sempre deveria existir
    exit (1); 
  }

  ImprimeRanking(arq, jogo);
  fclose(arq);
}

void ImprimeRanking(FILE * arq, tJogo jogo){
  fprintf (arq, "id_pista,id_carro,iteracao\n");

  tMorte morteCrescente[20] = {0};
  int i, j, menorMorte;
  tMorte aux;

  for (i=0; i<jogo.qtdMortes; i++){
    morteCrescente[i]= jogo.morte[i];
  }
  
  for (i=0; i<jogo.qtdMortes-1; i++){
    menorMorte= i;
    for (j=i+1; j<jogo.qtdMortes; j++) {
      if (morteCrescente[j].pista<morteCrescente[menorMorte].pista) {
        menorMorte=j;
      } else if (morteCrescente[j].pista==morteCrescente[menorMorte].pista) {
        if (morteCrescente[j].carro<morteCrescente[menorMorte].carro){
          menorMorte= j;
        } else if (morteCrescente[j].carro==morteCrescente[menorMorte].carro ) {
          if (morteCrescente[j].iteracao>morteCrescente[menorMorte].iteracao){
            menorMorte= j;
          }
        }
      }
    }
    
    if (menorMorte!=i){
      aux= morteCrescente[i];
      morteCrescente[i]=morteCrescente[menorMorte];
      morteCrescente[menorMorte]=aux;
    }
  }
  
  
  for (i=0; i<jogo.qtdMortes; i++){
    fprintf(arq, "%d,%d,%d\n", morteCrescente[i].pista, morteCrescente[i].carro, morteCrescente[i].iteracao);
  }
}

//funcao de heatmap.txt
void ImprimeHeat(FILE * arq, tJogo jogo);

void GerarMapaDeCalor (tJogo jogo, char * argv[]){
  char diretorio[1000];
  strcpy (diretorio, argv[1]);
  strcat (diretorio, "/saida/heatmap.txt");

  FILE * arq = fopen(diretorio, "w");
  if (arq==NULL){
    printf("Erro: pasta de saida nao existe!\n"); //sempre deveria existir
    exit (1); 
  }

  ImprimeHeat(arq, jogo);
  fclose(arq);
}


void ImprimeHeat(FILE * arq, tJogo jogo){
  int i, j, k;
  int comprimento= jogo.qtdPista*3-1;

  //preenchendo matriz com 0
  for (i=0; i<comprimento; i++){
    for (j=0; j<jogo.largura; j++){
      jogo.HeatMapMatriz[i][j]=0;
    }
  }

  k=jogo.qtdPista-1;
  for (i=0; i<comprimento; i=i+3){ //2 linha + 1 divisoria
    if (jogo.pista[k].countHeatMap<0) { //com colisao
      for (j = 0; j < jogo.largura; j++) { //duas linhas com -1
        jogo.HeatMapMatriz[i][j]=-1;
        jogo.HeatMapMatriz[i+1][j]=-1;
      }
    } else { //sem colisao
      jogo.HeatMapMatriz[i][jogo.galinha.posX-2]= jogo.pista[k].countHeatMap;
      jogo.HeatMapMatriz[i][jogo.galinha.posX-1]= jogo.pista[k].countHeatMap;
      jogo.HeatMapMatriz[i][jogo.galinha.posX]= jogo.pista[k].countHeatMap;
  
      jogo.HeatMapMatriz[i+1][jogo.galinha.posX-2]= jogo.pista[k].countHeatMap;
      jogo.HeatMapMatriz[i+1][jogo.galinha.posX-1]= jogo.pista[k].countHeatMap;
      jogo.HeatMapMatriz[i+1][jogo.galinha.posX]= jogo.pista[k].countHeatMap;
    }
    k--;
  }

  for (i=comprimento-1; i>=0; i--){
    for (j=0; j<jogo.largura; j++){
      if (jogo.HeatMapMatriz[i][j] == -1) {
        fprintf(arq, " * ");   // substitui -1 por *
      } else {
        fprintf(arq,"%2d ", jogo.HeatMapMatriz[i][j]);
      }
    }
    fprintf (arq, "\n");
  }

}
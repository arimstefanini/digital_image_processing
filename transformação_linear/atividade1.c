/*+-------------------------------------------------------------------------+
  | PROCESSAMENTO DE IMAGEM -                                               |
  | -----------------------                                                 |
  | Programa que calcula o NEGATIVO de uma imagem em niveis de cinza.       |
  |                                                                         |
  | Por Luiz Eduardo da Silva.                                              |
  +-------------------------------------------------------------------------+*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE  1
#define FALSE 0
#define CREATOR "# CREATOR: Luiz Eduardo V:1.00 R:MAR/2015\n"

typedef int * imagem;

/*+-------------------------------------------------------------------------+
  | Rotinas para ALOCAR e DESALOCAR dinamicamente espaco de memoria para um |
  | vetor monodimensional que armazenara' a imagem.                         |
  | PARAMETROS:                                                             |
  | I  = Endereco do vetor (ponteiro de inteiros).                          |
  | nl = Numero de linhas.                                                  |
  | nc = Numero de colunas.                                                 |
  +-------------------------------------------------------------------------+*/
int aloca_memo(imagem *I, int nl, int nc) {
    *I = (int *) malloc(nl * nc * sizeof (int));
    if (*I) return TRUE;
    else return FALSE;
}

int desaloca_memo(imagem *I) {
    free(*I);
}

/*+-------------------------------------------------------------------------+
  | Apresenta informacoes sobre um arquivo de imagem.                       |
  | Parametros:                                                             |
  |   nome = nome fisico do arquivo de imagem.                              |
  |   nl   = numero de linhas da imagem.                                    |
  |   nc   = numero de colunas da imagem.                                   |
  |   mn   = maximo nivel de cinza da imagem.                               |
  +-------------------------------------------------------------------------+*/
void info_imagem(char *nome, int nl, int nc, int mn) {
    printf("\nINFORMACOES SOBRE A IMAGEM:");
    printf("\n--------------------------\n");
    printf("Nome do arquivo.............: %s \n", nome);
    printf("Numero de linhas............: %d \n", nl);
    printf("Numero de colunas...........: %d \n", nc);
    printf("Maximo nivel-de-cinza/cores.: %d \n\n", mn);
}

/*+-------------------------------------------------------------------------+
  | Rotina que faz a leitura de uma imagem em formato .PGM ASCII e armazena |
  | num vetor monodimensional. Um exemplo de imagem .PGM ASCII gravada neste|
  | formato:                                                                |
  |                                                                         |
  | P2                                                                      |
  | # CREATOR: XV Version 3.10a  Rev: 12/29/94                              |
  | 124 122                                                                 |
  | 255                                                                     |
  | 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255     |
  | 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255     |
  | (...)                                                                   |
  |                                                                         |
  | Lin 1: contem P2, o que identifica este arquivo como PGM ASCII.         |
  | Lin 2: contem um comentario qualquer iniciado com #.                    |
  | Lin 3: numero de colunas e numero de linhas da imagem.                  |
  | Lin 4: maximo nivel de cinza na imagem (255 normalmente).               |
  | Lin 5 em diante: valores de cinza da imagem.                            |
  |                                                                         |
  | PARAMETROS:                                                             |
  | nome = nome do arquivo (entra).                                         |
  | I    = ponteiro para o vetor imagem (retorna).                          |
  | nl   = numero de linhas da imagem (retorna).                            |
  | nc   = numero de colunas da imagem (retorna).                           |
  | mn   = maximo nivel de cinza (retorna).                                 |
  +-------------------------------------------------------------------------+*/
int le_imagem_pgm(char *nome, imagem *I, int *nl, int *nc, int *mn) {
    int i, j, k, LIMX, LIMY, MAX_NIVEL;
    char c, LINHA[100];
    FILE *arq;
    if ((arq = fopen(nome, "r")) == NULL) {
        printf("Erro na ABERTURA do arquivo <%s>\n\n", nome);
        return FALSE;
    }
    /*-- PGM = "P2" -----------*/
    fgets(LINHA, 80, arq);
    if (!strstr(LINHA, "P2")) {
        printf("Erro no FORMATO do arquivo <%s>\n\n", nome);
        return FALSE;
    }
    /*-- Dimensoes da imagem --*/
    fgets(LINHA, 80, arq);
    do {
        fgets(LINHA, 80, arq);
    } while (strchr(LINHA, '#'));
    sscanf(LINHA, "%d %d", &LIMX, &LIMY);
    fscanf(arq, "%d", &MAX_NIVEL);

    if (LIMX == 0 || LIMY == 0 || MAX_NIVEL == 0) {
        printf("Erro nas DIMENSOES do arquivo <%s>\n\n", nome);
        return FALSE;
    }

    if (aloca_memo(I, LIMY, LIMX)) {
        for (i = 0; i < LIMY; i++) {
            for (j = 0; j < LIMX; j++) {
                fscanf(arq, "%d", &k);
                if (k > MAX_NIVEL) {
                    printf("Erro nos DADOS do arquivo <%s>\n", nome);
                    printf("Valor lido: %d   Max Nivel: %d\n\n", k, MAX_NIVEL);
                    return FALSE;
                }
                *(*(I) + i * LIMX + j) = k;
            }
        }
        fclose(arq);
    } else {
        printf("Erro na ALOCACAO DE MEMORIA para o arquivo <%s>\n\n", nome);
        printf("Rotina: le_imagem_pgm\n\n");
        fclose(arq);
        return FALSE;
    }
    *nc = LIMX;
    *nl = LIMY;
    *mn = MAX_NIVEL;
    return TRUE;
}

/*+-------------------------------------------------------------------------+
  | Rotina que grava o arquivo da imagem em formato PGM ASCII.              |
  | PARAMETROS:                                                             |
  | I    = ponteiro para o vetor imagem (entra).                            |
  | nome = nome do arquivo (entra).                                         |
  | nl   = numero de linhas (entra).                                        |
  | nc   = numero de colunas (entra).                                       |
  | mn   = maximo nivel de cinza (entra).                                   |
  +-------------------------------------------------------------------------+*/
void grava_imagem_pgm(imagem I, char *nome, int nl, int nc, int mn) {
    int i, j, x, k, valores_por_linha;
    FILE *arq;
    if ((arq = fopen(nome, "wt")) == NULL) {
        printf("Erro na CRIACAO do arquivo <%s>\n\n", nome);
    } else {
        fputs("P2\n", arq);
        fputs(CREATOR, arq);
        fprintf(arq, "%d  %d\n", nc, nl);
        fprintf(arq, "%d\n", mn);
        valores_por_linha = 16;
        for (i = 0, k = 0; i < nl; i++)
            for (j = 0; j < nc; j++) {
                x = *(I + i * nc + j);
                fprintf(arq, "%3d ", x);
                k++;
                if (k > valores_por_linha) {
                    fprintf(arq, "\n");
                    k = 0;
                }
            }
    }
    fclose(arq);
}

void ajuste(imagem I, imagem O, int nl, int nc, int mn, int param[5] ) {
    int i, j;
    
    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++) {
            
            int valor = I[i * nc + j];
                     
            //dados o ponto de (A-B) intervalo inical
            if(valor >= 0 && valor < param[0]){
                double a = (double) (0.0-param[1])/(0.0-param[0]);
                double b = (double) 0.0 - a*0.0;
                O[i * nc + j] = (double) a * valor +b;
                

            //dado o ponto (B-C) intervalo central o qual deve ser sofrer a transformacao
            } else if(valor >= param[0] && valor <= param[2]){
                double a = (double) (param[3] -param[1])/(param[2]-param[0]);
                double b = (double) param[3]  - a*param[2];
                O[i * nc + j] = (double)  a * valor +b;
                

            //dado o ponto (C-D) maximo nivel de cinza
            } else if(valor > param[2] && valor <= 255){
                double a = (double) (255-param[3] )/(255-param[2]);
                double b = (double) 255 - a* 255;
                O[i * nc + j] = (double) a * valor +b;
                
            }
            
        }     
}


void limiariza(imagem I, imagem O, int nl, int nc, int mn, int param[5]) {
    int i, j;
   
    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++) {
            if(I[i * nc + j] <= param[4])
                O[i * nc + j] = 0;
            else
                O[i * nc + j] = mn;
        }
}

void msg(char *s) {
    printf("\nNEGATIVO negatiquer");
    printf("\n-------------------------------");
    printf("\nUSO.:  %s  <IMG.PGM>", s);
    printf("\nONDE:\n");
    printf("    <IMG.PGM>  Arquivo da imagem em formato PGM\n\n");
}

/*+------------------------------------------------------+
  |        P R O G R A M A    P R I N C I P A L          |
  +------------------------------------------------------+*/
main(int argc, char *argv[]) {
    int OK, nc, nl, mn, tipo_img;
    
    int param[5];
    param[0] = atoi(argv[2]);
    param[1] = atoi(argv[3]);
    param[2] = atoi(argv[4]);
    param[3] = atoi(argv[5]);
    param[4]  = atoi(argv[6]);
    
    char nome[100];
    char comando[100];
    imagem In, Out;
    if (argc < 2)
        msg(argv[0]);
    else {
        OK = le_imagem_pgm(argv[1], &In, &nl, &nc, &mn);
        if (OK) {
            printf("\nTransformacao DE IMAGEM");
            info_imagem(argv[1], nl, nc, mn);
            aloca_memo(&Out, nl, nc);
            ajuste(In, Out, nl, nc, mn, param);
            strcpy(nome, argv[1]);
            strcat(nome, "-contraste.pgm");
            
            grava_imagem_pgm(Out, nome, nl, nc, mn);
            strcpy(comando, ".\\i_view32 ");
            strcat(comando, nome);
            strcat(comando, " /hide=7");
            system ("./i_view32 contraste.pgm /hide=7");
            system(comando);

            limiariza(Out, Out, nl, nc, mn, param);
            strcpy(nome, argv[1]);
            strcat(nome, "-limiariza.pgm");

            grava_imagem_pgm(Out, nome, nl, nc, mn);
            strcpy(comando, ".\\i_view32 ");
            strcat(comando, nome);
            strcat(comando, " /hide=7");
            system ("./i_view32 limiariza.pgm /hide=7");
            system(comando);
            
            desaloca_memo(&In);
            desaloca_memo(&Out);
        }
    }
    return 0;
}


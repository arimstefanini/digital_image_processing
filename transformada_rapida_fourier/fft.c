/*+-------------------------------------------------------------------------+
  | PROCESSAMENTO DE IMAGEM -                                               |
  | -----------------------                                                 |
  | Programa que calcula a transformada Rapida de FOURIER                   |
  |                                                                         |
  | Por Luiz Eduardo da Silva.                                              |
  +-------------------------------------------------------------------------+*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define TRUE 1
#define FALSE 0
#define CREATOR "# CREATOR: Luiz Eduardo\n"

typedef int *imagem;

/*-------------------------------------------------------------------------
 * Rotinas para ALOCAR e DESALOCAR dinamicamente espaco de memoria para um 
 *  vetor monodimensional que armazenara' a imagem.
 * PARAMETROS:
 *   I  = Endereco do vetor (ponteiro de inteiros).
 *   nl = Numero de linhas.
 *   nc = Numero de colunas.
 *-------------------------------------------------------------------------*/
int aloca_memo(imagem *Img, int nl, int nc)
{
    *Img = (int *)malloc(nl * nc * sizeof(int));
    if (*Img)
        return TRUE;
    else
        return FALSE;
}

int desaloca_memo(imagem *Img)
{
    free(*Img);
}

/*-------------------------------------------------------------------------
 * Apresenta informacoes sobre um arquivo de imagem.
 * Parametros:
 *   nome = nome fisico do arquivo de imagem.
 *   nl   = numero de linhas da imagem.
 *   nc   = numero de colunas da imagem.
 *   mn   = maximo nivel de cinza da imagem.
 *-------------------------------------------------------------------------*/
void info_imagem(char *nome, int nl, int nc, int mn)
{
    printf("\nINFORMACOES SOBRE A IMAGEM:");
    printf("\n--------------------------\n");
    printf("Nome do arquivo.............: %s \n", nome);
    printf("Numero de linhas............: %d \n", nl);
    printf("Numero de colunas...........: %d \n", nc);
    printf("Maximo nivel-de-cinza/cores.: %d \n\n", mn);
}

/*-------------------------------------------------------------------------
 * Rotina que faz a leitura de uma imagem em formato .PGM ASCII e armazena 
 * num vetor monodimensional. Um exemplo de imagem .PGM ASCII gravada neste
 * formato:
 *
 * P2
 * # CREATOR: XV Version 3.10a
 * 124 122
 * 255
 * 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255
 * 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255
 * (...)
 *
 * Lin 1: contem P2, o que identifica este arquivo como PGM ASCII.
 * Lin 2: contem um comentario qualquer iniciado com #.
 * Lin 3: numero de colunas e numero de linhas da imagem.
 * Lin 4: maximo nivel de cinza na imagem (255 normalmente).
 * Lin 5 em diante: valores de cinza da imagem.
 *
 * PARAMETROS:
 *   nome = nome do arquivo (entra).
 *   I    = ponteiro para o vetor imagem (retorna).
 *   nl   = numero de linhas da imagem (retorna).
 *   nc   = numero de colunas da imagem (retorna).
 *   mn   = maximo nivel de cinza (retorna).
 *-------------------------------------------------------------------------*/
int le_imagem_pgm(char *nome, imagem *Img, int *nl, int *nc, int *mn)
{
    int i, j, k, LIMX, LIMY, MAX_NIVEL;
    char c, LINHA[100];
    FILE *arq;
    if ((arq = fopen(nome, "r")) == NULL)
    {
        printf("Erro na ABERTURA do arquivo <%s>\n\n", nome);
        return FALSE;
    }
    /*-- PGM = "P2" -----------*/
    fgets(LINHA, 80, arq);
    if (!strstr(LINHA, "P2"))
    {
        printf("Erro no FORMATO do arquivo <%s>\n\n", nome);
        return FALSE;
    }
    /*-- Dimensoes da imagem --*/
    fgets(LINHA, 80, arq);
    do
    {
        fgets(LINHA, 80, arq);
    } while (strchr(LINHA, '#'));
    sscanf(LINHA, "%d %d", &LIMX, &LIMY);
    fscanf(arq, "%d", &MAX_NIVEL);

    if (LIMX == 0 || LIMY == 0 || MAX_NIVEL == 0)
    {
        printf("Erro nas DIMENSOES do arquivo <%s>\n\n", nome);
        return FALSE;
    }

    if (aloca_memo(Img, LIMY, LIMX))
    {
        for (i = 0; i < LIMY; i++)
        {
            for (j = 0; j < LIMX; j++)
            {
                fscanf(arq, "%d", &k);
                if (k > MAX_NIVEL)
                {
                    printf("Erro nos DADOS do arquivo <%s>\n", nome);
                    printf("Valor lido: %d   Max Nivel: %d\n\n", k, MAX_NIVEL);
                    return FALSE;
                }
                *(*(Img) + i * LIMX + j) = k;
            }
        }
        fclose(arq);
    }
    else
    {
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

/*-------------------------------------------------------------------------
 * Rotina que grava o arquivo da imagem em formato PGM ASCII.
 * PARAMETROS:
 *   I    = ponteiro para o vetor imagem (entra).
 *   nome = nome do arquivo (entra).
 *   nl   = numero de linhas (entra).
 *   nc   = numero de colunas (entra).
 *   mn   = maximo nivel de cinza (entra).
 *-------------------------------------------------------------------------*/
void grava_imagem_pgm(imagem Img, char *nome, int nl, int nc, int mn)
{
    int i, j, x, k, valores_por_linha;
    FILE *arq;
    if ((arq = fopen(nome, "wt")) == NULL)
    {
        printf("Erro na CRIACAO do arquivo <%s>\n\n", nome);
    }
    else
    {
        fputs("P2\n", arq);
        fputs(CREATOR, arq);
        fprintf(arq, "%d  %d\n", nc, nl);
        fprintf(arq, "%d\n", mn);
        valores_por_linha = 16;
        for (i = 0, k = 0; i < nl; i++)
            for (j = 0; j < nc; j++)
            {
                x = *(Img + i * nc + j);
                fprintf(arq, "%3d ", x);
                k++;
                if (k > valores_por_linha)
                {
                    fprintf(arq, "\n");
                    k = 0;
                }
            }
    }
    fclose(arq);
}

/*-----------------------------------------------------------------
 * Esta função calcula FFT com numeros complexo
 * x e y são arrays de 2^m pontos de numeros reais e imaginarios, 
 * respectivamente:
 *   dir =  1 para calcular a transformação direta
 *   dir = -1 para calcular a transformação reversa
 * Cálculo:
 *   F_k = F_k^{par} + W^k F_k^{impar}
 *         onde: W^k = e^{2iPI/N}
 *   F_k^{ipippii...ipiip} = fn (termo trivial, calculado sem k)
 *   Se i (ímpar) = 0 e p (par) = 1, 
 *      Então ipippii...ipiip = 0101100...01001 
 *   Os bits reversos 10010...0011010, definem o termo trivial fn
 *----------------------------------------------------------------*/
int FFT(int dir, int m, double *x, double *y)
{
    long nn, i, i1, j, k, i2, l, l1, l2;
    double c1, c2, tx, ty, t1, t2, u1, u2, z;

    /* Calcula o número de pontos */
    nn = 1;
    for (i = 0; i < m; i++)
        nn *= 2;

    /* Reordena os pontos de acordo com os bits reversos */
    i2 = nn >> 1;
    j = 0;
    for (i = 0; i < nn - 1; i++)
    {
        if (i < j)
        {
            tx = x[i];
            ty = y[i];
            x[i] = x[j];
            y[i] = y[j];
            x[j] = tx;
            y[j] = ty;
        }
        k = i2;
        while (k <= j)
        {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    /* Computa o FFT */
    c1 = -1.0;
    c2 = 0.0;
    l2 = 1;
    for (l = 0; l < m; l++)
    {
        l1 = l2;
        l2 <<= 1;
        u1 = 1.0;
        u2 = 0.0;
        for (j = 0; j < l1; j++)
        {
            for (i = j; i < nn; i += l2)
            {
                i1 = i + l1;
                t1 = u1 * x[i1] - u2 * y[i1];
                t2 = u1 * y[i1] + u2 * x[i1];
                x[i1] = x[i] - t1;
                y[i1] = y[i] - t2;
                x[i] += t1;
                y[i] += t2;
            }
            z = u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        if (dir == 1)
            c2 = -c2;
        c1 = sqrt((1.0 + c1) / 2.0);
    }

    /* Escala para a transformação direta */
    if (dir == 1)
        for (i = 0; i < nn; i++)
        {
            x[i] /= (double)nn;
            y[i] /= (double)nn;
        }

    return (TRUE);
}

/*-----------------------------------------------------
 *  Calcula a potencia de 2 mais próximo de um número
 *  twopm = 2**m <= n
 *  Retorna TRUE se 2**m == n
 *-----------------------------------------------------*/
int Powerof2(int n, int *m, int *twopm)
{
    if (n <= 1)
    {
        *m = 0;
        *twopm = 1;
        return (FALSE);
    }

    *m = 1;
    *twopm = 2;
    do
    {
        (*m)++;
        (*twopm) *= 2;
    } while (2 * (*twopm) <= n);

    if (*twopm != n)
        return (FALSE);
    else
        return (TRUE);
}

void erro(char *msg)
{
    printf("\n\nERRO: %s\n", msg);
    exit(10);
}

/*-------------------------------------------------------------------------
 * Executa um FFT 2D numa matriz de números complexos
 * A direção dir, 1 para direta, -1 para reversa
 * O tamanho da matriz (nl = numero de linhas, nc = numero de colunas)
 * Aborta com erro se não tem memória ou as dimensões não são potência de 2.
 *-------------------------------------------------------------------------*/
void FFT2D(double _Complex *c, int nl, int nc, int dir)
{
    int i, j;
    int m, twopm;
    double *real, *imag;

    /* Transforma as linhas */
    real = (double *)malloc(nl * sizeof(double));
    imag = (double *)malloc(nl * sizeof(double));
    if (real == NULL || imag == NULL)
        erro("alocação de memória - FFT2D");
    if (!Powerof2(nl, &m, &twopm) || twopm != nl)
        erro("Falhou no cálculo das linhas - FFT2D");
    for (j = 0; j < nc; j++)
    {
        for (i = 0; i < nl; i++)
        {
            real[i] = creal(c[i * nc + j]);
            imag[i] = cimag(c[i * nc + j]);
        }
        FFT(dir, m, real, imag);
        for (i = 0; i < nl; i++)
        {
            c[i * nc + j] = real[i] + imag[i] * I;
        }
    }
    free(real);
    free(imag);

    /* Transforma as colunas */
    real = (double *)malloc(nc * sizeof(double));
    imag = (double *)malloc(nc * sizeof(double));
    if (real == NULL || imag == NULL)
        erro("alocação de memória - FFT2D");
    if (!Powerof2(nc, &m, &twopm) || twopm != nc)
        erro("Falhou no cálculo das colunas - FFT2D");
    for (i = 0; i < nl; i++)
    {
        for (j = 0; j < nc; j++)
        {
            real[j] = creal(c[i * nc + j]);
            imag[j] = cimag(c[i * nc + j]);
        }
        FFT(dir, m, real, imag);
        for (j = 0; j < nc; j++)
        {
            c[i * nc + j] = real[j] + imag[j] * I;
        }
    }
    free(real);
    free(imag);

    return (TRUE);
}

double magnitude(double real, double imag)
{
    return sqrt(real * real + imag * imag);
}

void trocaQuadrante(double _Complex *img, int nl, int nc)
{
    int i, j;
    double _Complex *swapImg;
    swapImg = (double _Complex *)malloc(nl * nc * sizeof(double _Complex));

    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++)
            swapImg[i * nc + j] = img[i * nc + j];

    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++)
            img[((i + nl / 2) % nl) * nc + (j + nc / 2) % nc] = swapImg[i * nc + j];
}

void filtro(double _Complex *img, int nl, int nc)
{

    //extrai a raiz de nl e de nl e pega a media entro os dois
    double x = sqrt(nl);
    double y = sqrt(nc);
    double media_x_y = (x+y)/2;

    int i, j;
    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++)
        {  
            //calculando o centro da imagem
            double calc_j = (j-(nc/2))*(j-(nl/2));
            double calc_i = (i-(nc/2))*(i-(nl/2));
            double centro = sqrt(calc_j  + calc_i);

            // deixando o espectro apenas no centro da imagem
            if( centro > media_x_y){
                img[i+ nc*j]=0;
            }

        }
}

/*-------------------------------------------------------------------------
 * Implementação da transformada rápida de fourier
 * PARÂMETROS:
 *   In = imagem de entrada
 *   Out= imagem de saída
 *   nl = número de linhas.
 *   nc = número de colunas.
 *   mn = máximo nível de cinza
 *-------------------------------------------------------------------------*/
void fastFourier(imagem In, imagem Out, int nl, int nc, int mn)
{
    int i, j;
    double _Complex *imgFourier;
    double *FFTMag = (double *)malloc(nl * nc * sizeof(double));

    printf("Iniciou FFT\n");

    imgFourier = (double _Complex *)malloc(nl * nc * sizeof(double _Complex));
    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++)
            imgFourier[i * nc + j] = In[i * nc + j];

    FFT2D(imgFourier, nl, nc, 1);
    trocaQuadrante(imgFourier, nl, nc);
    filtro(imgFourier, nl, nc);
    FFT2D(imgFourier, nl, nc, -1);

    float max = -1.E12;
    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++)
        {
            double real = creal(imgFourier[i * nc + j]);
            double imag = cimag(imgFourier[i * nc + j]);
            float val = magnitude(real, imag);
            if (val > max)
                max = val;
            FFTMag[i * nc + j] = val;
        }

    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++)
            Out[i * nc + j] = (int)(255.0 * FFTMag[i * nc + j] / max);
            //Out[i * nc + j] = (int)(255.0 * pow(FFTMag[i * nc + j],0.125) / pow(max,0.125));
            

    printf("Terminou FFT\n");
}

void msg(char *s)
{
    printf("\nFast FOURIER de uma imagem qualquer");
    printf("\n-------------------------------");
    printf("\nUSO.:  %s  <IMG.PGM>", s);
    printf("\nONDE:\n");
    printf("    <IMG.PGM>  Arquivo da imagem em formato PGM\n\n");
}

/*------------------------------------------------------
 *        P R O G R A M A    P R I N C I P A L
 *------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int OK, nc, nl, mn;
    char nome[100];
    char comando[100];
    imagem In, Out;
    if (argc < 2)
        msg(argv[0]);
    else
    {
        OK = le_imagem_pgm(argv[1], &In, &nl, &nc, &mn);
        if (OK)
        {
            printf("\nTransformada Rápida de Fourier");
            info_imagem(argv[1], nl, nc, mn);
            aloca_memo(&Out, nl, nc);

            fastFourier(In, Out, nl, nc, mn);

            strcpy(nome, argv[1]);
            strcat(nome, "-fft.pgm");
            grava_imagem_pgm(Out, nome, nl, nc, mn);
            
            strcpy(comando, ".\\i_view32 ");
            strcat(comando, nome);
            strcat(comando, " /hide=7");
            system ("./i_view32 fft.pgm /hide=7");
            system(comando);
            
            desaloca_memo(&In);
            desaloca_memo(&Out);
        }
    }
    return 0;
}


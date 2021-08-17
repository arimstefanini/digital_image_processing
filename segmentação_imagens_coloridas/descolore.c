#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*------------------------------------------------
 * Fila de prioridade
 *------------------------------------------------*/
typedef struct no *ptno;
typedef struct no
{
    int i, j;
    ptno next;
} no;

/*-----------------------------------------------
 *  init   *Q      new     init         *Q
 *  [a:]->[b:]-+   [c:]  ->[a:]->[b:]->[c:]-+
 *   ^---------+             ^--------------+
 *----------------------------------------------*/
void insQ(ptno *Q, int i, int j)
{
    ptno new = malloc(sizeof(no));
    new->i = i;
    new->j = j;
    if (!(*Q))
        new->next = new;
    else
    {
        new->next = (*Q)->next;
        (*Q)->next = new;
    }
    *Q = new;
}

/*-----------------------------------------------
 *  init         *Q                    *Q
 *  [a:]->[b:]->[c:]-+    [a:]  [b:]->[c:]-+
 *    ^--------------+            ^--------+
 *----------------------------------------------*/
void remQ(ptno *Q, int *i, int *j)
{
    ptno init = (*Q)->next;
    *i = init->i;
    *j = init->j;
    if (*Q == init)
        *Q = NULL;
    else
        (*Q)->next = init->next;
    free(init);
}

int isEmpty(ptno *Q)
{
    return *Q == NULL;
}

void initQPrior(ptno *QPrior, int mn)
{
    int i;
    for (i = 0; i < mn; i++)
        QPrior[i] = NULL;
}

void insert(ptno *QPrior, int i, int j, int p)
{
    insQ(QPrior + p, i, j);
}

int pop(ptno *QPrior, int *i, int *j, int *maxPrior, int mn)
{
    while (*maxPrior < mn && isEmpty(QPrior + *maxPrior))
        (*maxPrior)++;
    if (*maxPrior == mn)
        return 1;
    remQ(QPrior + *maxPrior, i, j);
    return 0;
}

/*------------------------------------------------
 * Imagem PGM
 *------------------------------------------------*/
#define CREATOR "# Imagem criada por Ariela Marçal Stefanini\n"

typedef int *image;

image img_alloc(int nl, int nc)
{
    return (image)malloc(nl * nc * sizeof(int));
}

int img_free(image Img)
{
    free(Img);
}

void img_info(char *nome, int nl, int nc, int mn)
{
    printf("\nInformacoes:");
    printf("\n--------------------------\n");
    printf("Nome do arquivo.............: %s \n", nome);
    printf("Numero de linhas............: %d \n", nl);
    printf("Numero de colunas...........: %d \n", nc);
    printf("Maximo nivel-de-cinza/cores.: %d \n\n", mn);
}

void ERROR(char *s)
{
    printf("ERRO: %s\n", s);
    exit(10);
}

image read_pgm(char *nome, int *nl, int *nc, int *mn)
{
    int i, j, k;
    char c, LINHA[100];
    image Img;
    FILE *arq;
    if ((arq = fopen(nome, "r")) == NULL)
        ERROR("Abertura do arquivo PGM");
    /*-- PGM = "P2" -----------*/
    fgets(LINHA, 80, arq);
    if (!strstr(LINHA, "P3"))
        ERROR("Formato do arquivo PGM");
    /*-- le as linhas de comentario --*/
    do
    {
        fgets(LINHA, 80, arq);
    } while (LINHA[0] == '#');
    /*-- le as dimensoes da imagem --*/
    sscanf(LINHA, "%d %d", nc, nl);
    fscanf(arq, "%d", mn);

    if (*nl == 0 || *nc == 0 || *mn == 0)
        ERROR("Dimensões da imagem PGM");
    Img = img_alloc((*nl * 3), *nc);
    if (!Img)
        ERROR("Alocação de memória para imagem");
    for (i = 0; i < *nl * (*nc) * 3; i++)
    {
        fscanf(arq, "%d", &k);
        if (k > *mn)
            ERROR("valores dos pixels na imagem");
        Img[i] = k;
    }
    fclose(arq);
    return Img;
}

void write_ppm(image Img, char *nome, int nl, int nc, int mn)
{
    int x, k, valores_por_linha = 21;
    FILE *arq;
    if ((arq = fopen(nome, "wt")) == NULL)
        ERROR("Criação do arquivo");
    fputs("P3\n", arq);
    fputs(CREATOR, arq);
    fprintf(arq, "%d  %d\n", nc, nl);
    fprintf(arq, "%d\n", mn);
    for (int i = 0, k = 0; i < nl * 3; i++)
        for (int j = 0; j < nc; j++)
        {
            x = Img[i * nc + j];
            fprintf(arq, "%3d ", x);
            k++;
            if (k > valores_por_linha)
            {
                fprintf(arq, "\n");
                k = 0;
            }
        }
    fclose(arq);
}


void write_pgm(image Img, char *nome, int nl, int nc, int mn)
{
    int i, j, x, k, valores_por_linha;
    FILE *arq;
    if ((arq = fopen(nome, "wt")) == NULL)
        ERROR("Criação do arquivo PGM");
    fputs("P2\n", arq);
    fputs(CREATOR, arq);
    fprintf(arq, "%d  %d\n", nc, nl);
    fprintf(arq, "%d\n", mn);
    valores_por_linha = 20;
    for (i = 0, k = 0; i < nl; i++)
        for (j = 0; j < nc; j++)
        {
            x = Img[i * nc + j];
            fprintf(arq, "%3d ", x);
            k++;
            if (k > valores_por_linha)
            {
                fprintf(arq, "\n");
                k = 0;
            }
        }
    fclose(arq);
}

void converte_img(int nl, int nc, image Img, image Out)
{
    int soma_media = 0.0;
    for (int i = 0, j = 0; i < nl * nc; i++)
    {
        soma_media = Img[j] + Img[j + 1] + Img[j + 2];
        Out[i] = soma_media / 3;
        j = j + 3;
    }
}

/*------------------------------------------------
 * Calcula o gradient da imagem PGM
 *------------------------------------------------*/
void gradient(image In, image Out, int nl, int nc, int mn, int raio)
{
    int i, j, y, x, max, min;
    for (i = 0; i < nl; i++)
        for (j = 0; j < nc; j++)
        {
            max = -1;
            min = mn + 1;
            for (y = -raio; y <= raio; y++)     // -1 0 1
                for (x = -raio; x <= raio; x++) // -1 0 1
                {
                    int pi = i + y;
                    int pj = j + x;
                    if (pi >= 0 && pi < nl && pj >= 0 && pj < nc)
                    {
                        if (abs(x) + abs(y) <= raio && In[pi * nc + pj] > max)
                            max = In[pi * nc + pj];
                        if (abs(x) + abs(y) <= raio && In[pi * nc + pj] < min)
                            min = In[pi * nc + pj];
                    }
                }
            Out[i * nc + j] = max - min;
        }
}

/*------------------------------------------------
 * Calcula o Watershed da imagem
 *------------------------------------------------*/
void watershed(image In, image Out, int nl, int nc, int mn, int y, int x)
{
    int i, j, k, maxPrior = 0, stop = 0;
    ptno qPrior[mn];
    image mark = img_alloc(nl, nc);
    enum
    {
        NONE,
        QUEUE,
        WSHED,
        MARK1,
        MARK2,
    };
    struct
    {
        int i, j;
    } n4[4] = {0, 1, 1, 0, 0, -1, -1, 0};

    initQPrior(qPrior, mn);
    // Inicialização dos marcadores
    //-----------------------------
    for (i = 0; i < nl * nc; i++)
        mark[i] = NONE;
    // MARK1 = (y, x) - centro da região

    int raio = 10;
    for (i = -raio; i <= raio; i++)
        for (j = -raio; j <= raio; j++)
        {
            int pi = i + y;
            int pj = j + x;
            if (abs(i) + abs(j) <= raio)
                mark[pi * nc + pj] = MARK1;
        }

    //mark[y * nc + x] = MARK1;

    // MARK2 = borda da imagem
    for (i = 0; i < nl; i++)
    {
        mark[i * nc] = MARK2;
        mark[i * nc + nc - 1] = MARK2;
    }
    for (j = 0; j < nc; j++)
    {
        mark[j] = MARK2;
        mark[(nl - 1) * nc + j] = MARK2;
    }
    // Inicialização
    //--------------
    // Todos os vizinhos dos marcadores são colocados na fila
    // A prioridade é o nível de cinza do pixel
    for (i = 1; i < nl - 1; i++)
        for (j = 1; j < nc - 1; j++)
            if (mark[i * nc + j] == NONE)
            {
                int isAdj = 0;
                for (k = 0; k < 4; k++)
                {
                    int pi = i + n4[k].i;
                    int pj = j + n4[k].j;
                    int m = mark[pi * nc + pj];
                    if (m == MARK1 || m == MARK2)
                        isAdj = 1;
                }
                if (isAdj)
                {
                    mark[i * nc + j] = QUEUE;
                    insert(&qPrior, i, j, In[i * nc + j]);
                }
            }
    // Crescimento dos Marcadores
    //---------------------------
    // Enquanto a fila de prioridade não está vazia faça
    // 1. retira um pixel da fila
    // 2. Se o pixel eh vizinho de UMA marca, recebe essa marca.
    //    Seus vizinhos sem rotulo são colocados na fila
    // 3. Se o pixel eh vizinho de DUAS marcas, ele eh WATERSHED
    while (!stop)
    {
        int m = NONE;
        int isWshed = 0;
        stop = pop(qPrior, &i, &j, &maxPrior, mn);
        if (!stop)
        {
            for (k = 0; k < 4; k++)
            {
                int pi = i + n4[k].i;
                int pj = j + n4[k].j;
                if (pi >= 0 && pi < nl && pj >= 0 && pj < nc)
                {
                    int mAdj = mark[pi * nc + pj];
                    if (mAdj == MARK1 || mAdj == MARK2)
                    {
                        if (m == NONE)
                            m = mAdj;
                        else if (m != mAdj)
                            isWshed = 1;
                    }
                }
            }
            if (isWshed)
                mark[i * nc + j] = WSHED;
            else
            {
                mark[i * nc + j] = m;
                for (k = 0; k < 4; k++)
                {
                    int pi = i + n4[k].i;
                    int pj = j + n4[k].j;
                    if (pi >= 0 && pi < nl && pj >= 0 && pj < nc)
                        if (mark[pi * nc + pj] == NONE)
                        {
                            int prior, px;
                            mark[pi * nc + pj] = QUEUE;
                            px = In[pi * nc + pj];
                            prior = (px < maxPrior) ? maxPrior : px;
                            insert(&qPrior, pi, pj, prior);
                        }
                }
            }
        }
    }
    
    //divisao por metado do mn
    for (i = 0; i < nl * nc; i++)
        if (mark[i] == MARK1) {
            Out[i] = 112;
        }
        else 
            Out[i] = (mark[i] == WSHED) ? 255 : 0;

    img_free(mark);
}

void cor_original(int nl, int nc, image Img, image Out){
    for (int i = 0, j = 0; i < nl * nc; i++){
        if (Out[i] != 0){
            Img[i * 3] = Out[i];
            Img[i * 3 + 1] = Out[i];
            Img[i * 3 + 2] = Out[i];
        }
    }
}

void pega_centro( int nl, int nc, image img, image Out, int mn){
    for (int i = 0, j = 0; i < nl * nc; i++){
        if (Out[i] == 112){
            Out[i] = img[i];
        }
        else if (Out[i] == mn){
            Out[i] = img[i];
        }
    }
}



void msg(char *s)  
{
    printf("\nWatershed");
    printf("\n---------");
    printf("\nUSO.:  %s  <IMG.PGM>", s);
    printf("\nONDE:\n");
    printf("    <IMG.PGM>  Arquivo da imagem em formato PGM ASCII\n\n");
    exit(10);
}

int main(int argc, char *argv[])
{
    int OK, nc, nl, mn;
    char nome[100];
    char comando[100];
    image In, Out, Grd, Pgm;
    if (argc < 2)
        msg(argv[0]);

    In = read_pgm(argv[1], &nl, &nc, &mn);
    printf("\nWatershed");
    img_info(argv[1], nl, nc, mn);
    Out = img_alloc(nl, nc);
    Pgm = img_alloc(nl, nc);
    converte_img(nl, nc, In, Pgm);
    Grd = img_alloc(nl, nc);

    gradient(Pgm, Grd, nl, nc, mn, 1);
    watershed(Grd, Out, nl, nc, mn, 400, 112);

    pega_centro(nl, nc, Pgm,  Out, mn);
    cor_original(nl, nc,In, Out);
    sprintf(nome, "%s-%s", argv[1], "watershed.pgm");
    write_ppm(In, nome, nl, nc, mn);
    
    strcpy(comando, ".\\i_view32 ");
    strcat(comando, nome);
    strcat(comando, " /hide=7");
    system ("./i_view32 ariela1.ppm-watershed.pgm /hide=7");
    
    img_free(In);
    img_free(Grd);
    img_free(Out);
    return 0;
}

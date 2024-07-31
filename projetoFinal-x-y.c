#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <time.h>

#define ORG 'X'
#define VAZ '.'
#define TAM 101

typedef struct{
    char nomeJogo[TAM];     //Nome do padr�o
    int ciclosVida;         //N�mero de ciclos
    int dim1, dim2;         //Dimens�o da tabela
    int posicaoX, posicaoY; //Posi��o do padr�o inicial
    int objetoCabe;         //Flag que diz se � poss�vel colocar o objeto na tabela ('1' = SIM; '0' = N�O)
    char nomeInvasao[TAM];  //Nome do padr�o de invas�o
    int chanceInvasao;      //Chance de ocorrer uma invas�o definida pelo usu�rio
    int invasao;            //Flag que ir� dizer se haver� invas�o ('1' = SIM; '0' = N�O)
    int invasaoX, invasaoY; //Posi��o do padr�o que vai invadir
    char **m;
}Tab;

char** alocaMatriz(int nL, int nC);                     //Fun��o que aloca uma matriz
void imprimeMatriz(char **mat, int nL, int nC);         //Fun��o que imprime uma matriz na tela
void copiaMatriz(char **m, char **aux, int nL, int nC); //Fun��o que copia a matriz 'm' para 'aux'
void desalocaMatriz(char **m, int nL);                  //Fun��o que desaloca uma matriz
void limpaMatriz(char **m, int nL, int nC);             //Fun��o que "zera" uma matriz

void inicPadrao(Tab *tabela);        //Fun��o que inicia o padr�o desejado no menu
void inicInvasao(Tab *tabela);       //Fun��o que inicia o padr�o para a invas�o
void populacaoInvasora(Tab *tabela); //Fun��o que ir� definir aleatoriamente se haver� uma invas�o

int contaVizinha(char **m, int L, int C, int nL, int nC); //Fun��o que conta as vizinhas de uma c�lula

void jogaJogoVida(Tab *tabela);
void menuInicJogo(Tab *tabela);

int main()
{
    Tab tabela;

    int encerraPrograma = 0;

    srand(time(NULL));

    while(encerraPrograma == 0)
    {
        tabela.dim1 = 0;
        tabela.posicaoX = 0;
        tabela.dim2 = 0;
        tabela.posicaoY = 0;

        tabela.invasao = 0;
        tabela.objetoCabe = 1;
        while(tabela.dim1 == 0 || tabela.dim2 == 0)
        {
            printf("Digite o numero de linhas e de colunas, respectivamente: ");
            scanf("%d %d", &tabela.dim1, &tabela.dim2);

            if(tabela.dim1 <= 0 || tabela.dim2 <= 0)
            {
                system("cls");
                printf("Valor invalido para o tamanho da tabela... Digite novamente...\n\n");
                tabela.dim1 = 0;
                tabela.dim2 = 0;
                continue;
            }

            printf("Escolha em que linha e coluna, respectivamente, deseja posicionar o objeto: ");
            scanf("%d %d", &tabela.posicaoX, &tabela.posicaoY);

            if(tabela.posicaoX <= 0 || tabela.posicaoY <= 0 || tabela.posicaoX > tabela.dim1 || tabela.posicaoY > tabela.dim2)
            {
                system("cls");
                printf("Valor invalido para a posicao do objeto... Digite novamente...\n\n");
                tabela.dim1 = 0;
                tabela.posicaoX = 0;
                tabela.dim2 = 0;
                tabela.posicaoY = 0;
                continue;
            }

            tabela.posicaoX--;
            tabela.posicaoY--;
        }
        tabela.m = alocaMatriz(tabela.dim1, tabela.dim2);

        menuInicJogo(&tabela);

        if(tabela.objetoCabe == 0)
        {
            system("cls");
            printf("O objeto nao cabe nessa posicao... Insira os valores novamente...\n\n");
            desalocaMatriz(tabela.m, tabela.dim1);
            continue;
        }

        jogaJogoVida(&tabela);

        desalocaMatriz(tabela.m, tabela.dim1);

        printf("\nDeseja encerrar o programa? (1 para SIM e 0 para NAO): ");
        scanf("%d", &encerraPrograma);

        system("cls");
    }

    printf("Programa encerrado...\n");
    return 0;
}

char** alocaMatriz(int nL, int nC)
{
    char **mat;
    int i;

    mat = (char **) malloc (nL * sizeof(char *));
    if(mat == NULL)
    {
      printf("Erro na alocacao da matriz...");
      exit(1);
    }

    for(i = 0; i < nL; i++)
    {
        mat[i] = (char *) malloc (nC * sizeof(char));
        if(mat[i] == NULL)
        {
            printf("Erro na alocacao da matriz...");
            exit(1);
        }
    }

    return mat;
}

void imprimeMatriz(char **mat, int nL, int nC)
{
    int i, j;

    for(i = 0; i < nL; i++)
    {
        for(j = 0; j < nC; j++)
        {
            printf("%c ", mat[i][j]);
        }
        printf("\n");
    }
}

void copiaMatriz(char **m, char **aux, int nL, int nC)
{
    int i, j;

    for(i = 0; i < nL; i++)
    {
        for(j = 0; j < nC; j++)
        {
            aux[i][j] = m[i][j];
        }
    }
}

void desalocaMatriz(char **m, int nL)
{
    int i;

    for(i = 0; i < nL; i++)
    {
        free(m[i]);
    }

    free(m);
}

void limpaMatriz(char **m, int nL, int nC)
{
    int i, j;

    for(i = 0; i < nL; i++)
    {
        for(j = 0; j < nC; j++)
        {
            m[i][j] = VAZ;
        }
    }
}

void inicPadrao(Tab *tabela)
{
    FILE *arquivo;
    char linha[TAM], *token, nomeArquivo[TAM];
    int i, j;

    sprintf(nomeArquivo, "%s.csv", tabela->nomeJogo);
    arquivo = fopen(nomeArquivo, "r");
    if(arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo...");
        exit (1);
    }

    limpaMatriz(tabela->m, tabela->dim1, tabela->dim2);

    i = 0;
    while(!feof(arquivo)) //La�o que ir� se repetir at� chegar no fim do arquivo
    {
        fgets(linha, TAM, arquivo); //Ler uma linha do arquivo e guardar em 'linha'
        token = strtok(linha, ","); //Guarda na vari�vel token o primeiro n�mero que estiver em 'linha'
        while(token != NULL)
        {
            j = atoi(token); //Transforma o n�mero em inteiro e guarda na vari�vel j

            if(tabela->posicaoX+i >= tabela->dim1 || tabela->posicaoY+j-1 >= tabela->dim2)
            {
                tabela->objetoCabe = 0;
                break;
            }
            tabela->m[tabela->posicaoX+i][tabela->posicaoY+j-1] = ORG;
            token = strtok(NULL, ","); //Recebe o pr�ximo n�mero ap�s a ','
        }
        if(tabela->objetoCabe == 0)
            break;
        i++; //Passa para a pr�xima linha
    }
    fclose(arquivo);
}

void inicInvasao(Tab *tabela)
{
    FILE *arquivo;
    char linha[TAM], *token, nomeArquivo[TAM];
    int i, j;
    int invasaoCabe = 0, flag;

    sprintf(nomeArquivo, "%s.csv", tabela->nomeJogo);

    while(invasaoCabe == 0) //La�o para descobrir se a invas�o cabe na tabela
    {
        flag = 1;
        tabela->invasaoX = rand() % (tabela->dim1); //Escolhe uma linha aleat�ria que ser� colocada a invas�o
        tabela->invasaoY = rand() % (tabela->dim2); //Escolhe uma coluna aleat�ria que ser� colocada a invas�o

        arquivo = fopen(nomeArquivo, "r");
        if(arquivo == NULL)
        {
            printf("Erro ao abrir o arquivo da invasao...");
            exit (1);
        }

        i = 0;
        while(!feof(arquivo)) //La�o que ir� se repetir at� chegar no fim do arquivo
        {
            fgets(linha, TAM, arquivo); //Ler uma linha do arquivo e guardar em 'linha'
            token = strtok(linha, ","); //Guarda na vari�vel token o primeiro n�mero que estiver em 'linha'
            while(token != NULL)
            {
                j = atoi(token); //Transforma o n�mero em inteiro e guarda na vari�vel j
                if(tabela->invasaoX+i >= tabela->dim1 || tabela->invasaoY+j-1 >= tabela->dim2)
                {
                    flag = 0;
                    break;
                }
                token = strtok(NULL, ","); //Recebe o pr�ximo n�mero ap�s a ','
            }
            if(flag == 0)
                break;
            i++; //Passa para a pr�xima linha
        }
        fclose(arquivo);

        if(flag == 0) //Caso o padr�o da invas�o n�o cabe na tabela, � gerado uma nova posi��o
            continue;

        invasaoCabe = 1; //Caso chegue at� aqui, � porque a invas�o cabe na tabela
    }

    //A partir daqui, � colocado a invas�o na tabela
    arquivo = fopen(nomeArquivo, "r");
    if(arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo da invasao...");
        exit (1);
    }

    i = 0;
    while(!feof(arquivo)) //La�o que ir� se repetir at� chegar no fim do arquivo
    {
        fgets(linha, TAM, arquivo); //Ler uma linha do arquivo e guardar em 'linha'
        token = strtok(linha, ","); //Guarda na vari�vel token o primeiro n�mero que estiver em 'linha'
        while(token != NULL)
        {
            j = atoi(token); //Transforma o n�mero em inteiro e guarda na vari�vel j
            tabela->m[tabela->invasaoX+i][tabela->invasaoY+j-1] = ORG;
            token = strtok(NULL, ","); //Recebe o pr�ximo n�mero ap�s a ','
        }
        i++; //Passa para a pr�xima linha
    }
    fclose(arquivo);
}

void populacaoInvasora(Tab *tabela)
{
    int vaiInvadir;

    vaiInvadir = rand() % 10;
    if(vaiInvadir < tabela->chanceInvasao) //O usu�rio define a chance de invas�o
    {
        tabela->invasao = rand() % 6 + 1; //Escolhe aleatoriamente qual padr�o de invas�o ser� utilizado

        switch(tabela->invasao)
        {
            case 1:   strcpy(tabela->nomeJogo, "Bloco"); inicInvasao(tabela); strcpy(tabela->nomeInvasao, "Bloco"); break;
            case 2:   strcpy(tabela->nomeJogo, "VidaEterna2"); inicInvasao(tabela); strcpy(tabela->nomeInvasao, "VidaEterna2"); break;
            case 3:   strcpy(tabela->nomeJogo, "Blinker"); inicInvasao(tabela); strcpy(tabela->nomeInvasao, "Blinker"); break;
            case 4:   strcpy(tabela->nomeJogo, "Sapo"); inicInvasao(tabela); strcpy(tabela->nomeInvasao, "Sapo"); break;
            case 5:   strcpy(tabela->nomeJogo, "Glider"); inicInvasao(tabela); strcpy(tabela->nomeInvasao, "Glider"); break;
            case 6:   strcpy(tabela->nomeJogo, "LWSS"); inicInvasao(tabela); strcpy(tabela->nomeInvasao, "LWSS"); break;
        }
    }
}

int contaVizinha(char **m, int L, int C, int nL, int nC)
{
    int i, j, contagem=0;

    for(i = L-1; i <= L+1; i++)
    {
        for(j = C-1; j <= C+1; j++)
        {
            if((i==L && j==C) || (i<0 || j<0) || (i>=nL || j>=nC)) //Caso seja a mesma posi��o, ou o valor do 'i' ou do 'j' seja negativo ou maior que a tabela, d� um continue
                continue;

            if(m[i][j]==ORG)
            {
                contagem++; //Adiciona +1 na contagem de vizinhas vivas
            }
        }
    }

    return contagem;
}

void jogaJogoVida(Tab *tabela)
{
    char **aux;
    int viz, k;
    int i, j;

    aux = alocaMatriz(tabela->dim1, tabela->dim2);

    for(k = 0; k < tabela->ciclosVida+1; k++) //Repete o tanto de ciclos que o usu�rio definiu
    {
        populacaoInvasora(tabela);
        copiaMatriz(tabela->m, aux, tabela->dim1, tabela->dim2); //Copia a matriz 'm' para 'aux'
        system("cls");                                           //Limpa a tela
        imprimeMatriz(aux, tabela->dim1, tabela->dim2);          //Imprime na tela a matriz 'aux'

        if(tabela->invasao != 0) //Se houve uma invas�o, imprime qual invas�o est� ocorrendo e sua posi��o
        {
            printf("\n\nINVASAO IMINENTE: %s (%d, %d)", tabela->nomeInvasao, tabela->invasaoX+1, tabela->invasaoY+1);
            tabela->invasao = 0;
        }
        Sleep(1000);

        for(i = 0; i < tabela->dim1; i++)
        {
            for(j = 0; j < tabela->dim2; j++)
            {
                viz = contaVizinha(aux, i, j, tabela->dim1, tabela->dim2);
                if(aux[i][j]==ORG && (viz==2 || viz==3)) //Se a c�lula estiver viva e houver 2 ou 3 vizinhos vivos, ela vive
                    tabela->m[i][j] = ORG;
                else if(aux[i][j]==VAZ && (viz==3))      //Se a c�lula estiver morta e houver exatamente 3 vizinhos vivos, ela vive
                    tabela->m[i][j] = ORG;
                else                                     //Qualquer outra condi��o, deixar� a c�lula morta
                    tabela->m[i][j] = VAZ;
            }
        }
    }

    desalocaMatriz(aux, tabela->dim1);
}

void menuInicJogo(Tab *tabela)
{
    int opcao, opcaoValida = 0;

    while(opcaoValida == 0)
    {
        printf("(1)Bloco\n(2)Vida Eterna 2\n(3)Blinker\n(4)Sapo\n(5)Glider\n(6)LWSS\n(7)Especial\nEntre com a opcao: ");
        scanf("%d", &opcao);
        if(opcao < 1 || opcao > 7)
        {
            system("cls");
            printf("Opcao invalida, digite outro valor...\n\n");
            continue;
        }
        opcaoValida = 1;
    }
    switch(opcao)
    {
        case 1:   strcpy(tabela->nomeJogo, "Bloco"); inicPadrao(tabela); break;
        case 2:   strcpy(tabela->nomeJogo, "VidaEterna2"); inicPadrao(tabela); break;
        case 3:   strcpy(tabela->nomeJogo, "Blinker"); inicPadrao(tabela);  break;
        case 4:   strcpy(tabela->nomeJogo, "Sapo"); inicPadrao(tabela); break;
        case 5:   strcpy(tabela->nomeJogo, "Glider"); inicPadrao(tabela); break;
        case 6:   strcpy(tabela->nomeJogo, "LWSS"); inicPadrao(tabela); break;
        case 7:   strcpy(tabela->nomeJogo, "Especial"); inicPadrao(tabela); break;
    }

    if(tabela->objetoCabe != 0)
    {
        printf("\nO jogo escolhido foi %s!\n", tabela->nomeJogo);
        opcaoValida = 0;
        while(opcaoValida == 0)
        {
            printf("Digite a quantidade de ciclos que o jogo vai rodar: ");
            scanf("%d", &tabela->ciclosVida);
            if(tabela->ciclosVida < 0)
            {
                system("cls");
                printf("Valor invalido para o numero de ciclos, digite outro valor...\n\n");
                continue;
            }
            opcaoValida = 1;
        }
        opcaoValida = 0;
        while(opcaoValida == 0)
        {
            printf("\nDigite a taxa de chance de ocorrer uma invasao (Minimo = 0 (0%) e maximo = 10 (100%): ");
            scanf("%d", &tabela->chanceInvasao);
            if(tabela->chanceInvasao < 0 || tabela->chanceInvasao > 10)
            {
                system("cls");
                printf("Valor invalido para a chance de invasao, digite outro valor...\n\n");
                continue;
            }
            opcaoValida = 1;
        }
        system("cls");

        imprimeMatriz(tabela->m, tabela->dim1, tabela->dim2);

        printf("Se inicializacao correta digite qualquer tecla para iniciar o jogo..."); while(getchar()!='\n'); getchar();
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <math.h>

/*********************************CONSTANTES***********************************/
/* Alterar aqui, caso seja necessario */

// ENZO
#define ARQ_INSERE "C:\\Users\\steam\\Desktop\\Listas_e_Atividades\\4a_Semestre\\ED2\\Sistema-Biblioteca-V3\\arquivos\\insere.bin"
#define ARQ_AB "C:\\Users\\steam\\Desktop\\Listas_e_Atividades\\4a_Semestre\\ED2\\Sistema-Biblioteca-V3\\arquivos\\ab.bin"
#define ARQ_BUSCA "C:\\Users\\steam\\Desktop\\Listas_e_Atividades\\4a_Semestre\\ED2\\Sistema-Biblioteca-V3\\arquivos\\busca.bin"
#define ARQ_DADOS "C:\\Users\\steam\\Desktop\\Listas_e_Atividades\\4a_Semestre\\ED2\\Sistema-Biblioteca-V3\\arquivos\\dados.bin"

/*
// DOUGRINHAS GAMEPLAYS
#define ARQ_INSERE "C:\\Users\\Douglas Nicida\\Desktop\\projeto-ed2-biblioteca-arquivos-v3-main\\arquivos\\insere.bin"
#define ARQ_AB "C:\\Users\\Douglas Nicida\\Desktop\\projeto-ed2-biblioteca-arquivos-v3-main\\arquivos\\ab.bin"
#define ARQ_BUSCA "C:\\Users\\Douglas Nicida\\Desktop\\projeto-ed2-biblioteca-arquivos-v3-main\\arquivos\\busca.bin"
#define ARQ_DADOS "C:\\Users\\Douglas Nicida\\Desktop\\projeto-ed2-biblioteca-arquivos-v3-main\\arquivos\\dados.bin"
*/

#define KEY_SIZE 14
#define MAX_KEYS 3
#define NO_KEY '@'
#define NO 0
#define YES 1
#define NIL (-1)

/*********************************ESTRUTURAS***********************************/
typedef struct SLivro
{
    char isbn[KEY_SIZE];
    char titulo[50];
    char autor[50];
    char ano[5];
} Livro;

typedef struct SABPagina
{
    int ctChaves; // conta a qtd de chaves q tem na pagina no momento
    char chave[MAX_KEYS][KEY_SIZE];
    int posChave[MAX_KEYS];
    int filho[MAX_KEYS + 1]; // salva a páginas do filhos, onde cada página pode possuir no máximo 4 filhos
} ABPagina;

// #define TAM_PAGINA (sizeof(int) + ((sizeof(char) * KEY_SIZE) * (sizeof(char) * MAX_KEYS)) + (sizeof(int) * (MAX_KEYS + 1)))
#define TAM_PAGINA sizeof(ABPagina)

/********************************PROTOTIPOS************************************/

/* Sub-rotinas para manipular arvore B*/
int obterRaiz(FILE *arqAB);
int criarArvoreB(FILE *arqAB, char isbn[KEY_SIZE], int ct);
int obterPagina(FILE *arqAB);
int criarPagina(ABPagina *novaPag);
void abEscrever(FILE *arqAB, int rrn, ABPagina *abPagina); // VER SE ISSO FUNCIONA
int inserirRaiz(FILE *arqAB, int raiz);
void lerArvoreAB(FILE *arqAB, int rrn, ABPagina *abPagina);
int inserirAB(FILE *arqAB, int rrn, char chave[KEY_SIZE], int *paginaFilhoPromovido, char *chavePromovida, int ct, int *posChavePromovida);
int procurarNo(char chave[KEY_SIZE], ABPagina *abPagina, int *pos);
int criarRaiz(FILE *arqAB, int ct, char isbn[KEY_SIZE], int esq, int dir);
int insereNaPagina(FILE *arqAB, char chave[KEY_SIZE], int rrnPromovidaBaixo, int posChavePosPromovidoBaixo, ABPagina *abPagina);
void split(FILE *arqAB, char chavePromovidaDeBaixo[KEY_SIZE], int rrnPromovidoDeBaixo, int posPromovidaDeBaixo, ABPagina *paginaAntiga, char *chavePromovida, int *paginaFilhoPromovido, int *posChavePromovida, ABPagina *paginaNova);
void percorrerInOrdemAB(FILE *arqAB, FILE *arqDados, char key[KEY_SIZE]);

/* Sub-rotinas doo exercicio */
FILE *abrirArquivo(char *ch, char *tipoAbertura);
void fecharArquivo(FILE *arq);
void obterRegistro(FILE *arq, Livro *livro, char key[KEY_SIZE], char tipoArq);
void inserir(FILE *arqInserir, FILE *arqDados, FILE *arqAB);

/**********************************MAIN****************************************/
int main()
{
    printf("TAMANHO PAGINA: %d\n", TAM_PAGINA);

    setlocale(LC_ALL, "");

    FILE *arqInserir, *arqDados, *arqAB, *arqBusca;
    int op;

    // Menu
    printf("*-------- BIBLIOTECA --------*\n");

    do
    {
        printf("*______________________________________________*\n");
        printf("|  Opcao      | Funcao                         |\n");
        printf("|    1.       | Inserir                        |\n");
        printf("|    2.       | Listar todos os livros         |\n");
        printf("|    3.       | Listar livro especifico        |\n");
        printf("|    0.       | Sair do programa               |\n");
        printf("*----------------------------------------------*\n");
        printf("Digite a opcao: ");
        scanf("%d", &op);

        switch (op)
        {
        case 1:
        {
            arqInserir = NULL;
            arqDados = NULL;
            arqAB = NULL;
            inserir(arqInserir, arqDados, arqAB);
            break;
        }
        case 2:
        {
            printf("Listar dados de todos os livros");
            break;
        }
        case 3:
        {
            char key[KEY_SIZE];
            arqAB = abrirArquivo(ARQ_AB, "r+b");
            arqBusca = abrirArquivo(ARQ_BUSCA, "r+b");
            arqDados = abrirArquivo(ARQ_DADOS, "r+b");
            obterRegistro(arqBusca, NULL, key, 'b');

            percorrerInOrdemAB(arqAB, arqDados, key);
            fecharArquivo(arqAB);
            fecharArquivo(arqBusca);
            fecharArquivo(arqDados);

            break;
        }
        case 0:
        {
            exit(0);
        }
        default:
            printf("Digite uma das opcoes\n");
        }
    } while (op != 0);
}

/*********************************FUNCOES**************************************/

FILE *abrirArquivo(char *ch, char *tipoAbertura)
{
    FILE *arq;

    if (((arq = fopen(ch, tipoAbertura)) == NULL))
    {
        printf("ERRO: Falha ao abrir o arquivo\n%s", ch);
        return arq;
    }

    return arq;
}

void fecharArquivo(FILE *arq)
{
    fclose(arq);
}

void obterRegistro(FILE *arq, Livro *livro, char key[KEY_SIZE], char tipoArq)
{
    int ct = 1;

    if (fgetc(arq) == '@')
    {
        fread(&ct, sizeof(int), 1, arq);

        // Pular para posicao desejada e ler o registro
        if (tipoArq == 'i')
            fseek(arq, sizeof(Livro) * ct, SEEK_SET);
        else
            fseek(arq, KEY_SIZE * ct, SEEK_SET);

        if (tipoArq == 'i')
            fread(livro, sizeof(Livro), 1, arq);
        else
            fread(key, KEY_SIZE, 1, arq);

        // Salvar o proximo registro a ser lido
        rewind(arq);
        ct++;

        fseek(arq, 1, SEEK_SET);
        fwrite(&ct, sizeof(int), 1, arq);
    }
    else
    {
        rewind(arq);

        if (tipoArq == 'i')
            fread(livro, sizeof(Livro), 1, arq);
        else
            fread(key, KEY_SIZE, 1, arq);

        rewind(arq);

        fwrite("@", 1, sizeof(char), arq);
        fwrite(&ct, sizeof(int), 1, arq);
    }
}

void inserir(FILE *arqInserir, FILE *arqDados, FILE *arqAB)
{
    Livro livro;
    char buffer[sizeof(Livro)];
    int rrn, raiz;
    int promovido; // indica se houve promocao para baixo
    int rrnPromovido;
    char chavePromovida[KEY_SIZE];
    int posChavePromovida;
    int ct;

    arqInserir = abrirArquivo(ARQ_INSERE, "r+b");
    arqDados = abrirArquivo(ARQ_DADOS, "r+b");

    obterRegistro(arqInserir, &livro, NULL, 'i');

    fseek(arqInserir, 1, SEEK_SET);
    fread(&ct, 1, sizeof(int), arqInserir);

    printf("cttttt: %d\n", ct);

    // Inserir sempre no final do arquivo de DADOS
    sprintf(buffer, "%s#%s#%s#%s#", livro.isbn, livro.titulo, livro.autor, livro.ano);
    fseek(arqDados, 0, SEEK_END);
    fwrite(buffer, 1, sizeof(Livro), arqDados);

    // se o rrn for 0, quer dizer que deverá criar tudo do zero, se não, deve procurar um lugar para inserir
    if ((arqAB = abrirArquivo(ARQ_AB, "r+b")))
    {
        printf("\nO arquivo ja existe\n");
        raiz = obterRaiz(arqAB);

        // verifica se houve promoção e onde será inserido
        promovido = inserirAB(arqAB, raiz, livro.isbn, &rrnPromovido, &chavePromovida, ct, &posChavePromovida);

        // se houver promoção, cria-se uma nova raíz
        if (promovido)
            raiz = criarRaiz(arqAB, ct, chavePromovida, raiz, rrnPromovido); // Cria a página promovida
    }
    else
    {
        arqAB = abrirArquivo(ARQ_AB, "w+b");
        printf("\nO arquivo nao ja existe\n");
        raiz = criarArvoreB(arqAB, livro.isbn, ct);
    }

    fecharArquivo(arqAB);
    fecharArquivo(arqInserir);
    fecharArquivo(arqDados);
}

int obterPagina(FILE *arqAB)
{
    // Pegando a quantidade de bits no arquivo (de acordo com a quantidade de registros inserido)
    int totalRegistros, rrn;

    fseek(arqAB, 0, SEEK_END);
    totalRegistros = ftell(arqAB) - sizeof(int);

    // se não houver registros ainda
    if (totalRegistros <= 0)
        return 0;
    else
        rrn = totalRegistros / TAM_PAGINA;
    return rrn;
}

int obterRaiz(FILE *arqAB)
{
    int raiz;

    fseek(arqAB, 0, SEEK_SET);

    if ((fread(&raiz, 1, sizeof(int), arqAB)) == 0)
    {
        printf("ERRO: Nao foi possivel obter a raiz\n");
        exit(0);
    }

    return raiz;
}

void abEscrever(FILE *arqAB, int rrn, ABPagina *abPagina)
{
    int posEscrever;
    posEscrever = (rrn * TAM_PAGINA) + sizeof(int);

    printf("RRN AB ESCREVER PORA: %d\n\n", rrn);

    fseek(arqAB, posEscrever, SEEK_SET);
    fwrite(abPagina, TAM_PAGINA, 1, arqAB);
}

void lerArvoreAB(FILE *arqAB, int rrn, ABPagina *abPagina)
{
    int addr, i;

    addr = rrn * TAM_PAGINA + sizeof(int);

    printf("rrn: %d * TAM_PAGINA: %d = %d \n", rrn, TAM_PAGINA, addr);

    fseek(arqAB, addr, SEEK_SET);
    fread(abPagina, TAM_PAGINA, 1, arqAB);

    printf("chave dentro do ler arv ab: %s\n", abPagina->chave[0]);
}

// CRIAR

int criarPagina(ABPagina *novaPag)
{
    int i, j;

    for (i = 0; i < MAX_KEYS; i++)
    {

        for (j = 0; j < KEY_SIZE - 1; j++)
        {
            novaPag->chave[i][j] = NO_KEY;
        }

        novaPag->chave[i][KEY_SIZE - 1] = '\0';

        novaPag->posChave[i] = NIL;
        novaPag->filho[i] = NIL;
    }

    novaPag->filho[MAX_KEYS] = NIL;

    printf("Pagina criada com sucesso. \n");
}

int criarRaiz(FILE *arqAB, int ct, char isbn[KEY_SIZE], int esq, int dir)
{
    ABPagina abPagina;
    int rrn;

    printf("esquerda: %d // direita: %d\n", esq, dir);

    rrn = obterPagina(arqAB);
    criarPagina(&abPagina);

    strcpy(abPagina.chave[0], isbn);
    abPagina.posChave[0] = ct;
    abPagina.filho[0] = esq;
    abPagina.filho[1] = dir;
    abPagina.ctChaves = 1;
    abEscrever(arqAB, rrn, &abPagina);
    inserirRaiz(arqAB, rrn);

    printf("Raiz criada com sucesso. \n");

    return rrn;
}

int criarArvoreB(FILE *arqAB, char isbn[KEY_SIZE], int ct)
{
    int raiz = -1;

    fwrite(&raiz, 1, sizeof(int), arqAB);
    raiz = criarRaiz(arqAB, ct, isbn, NIL, NIL);

    printf("Arvore criada com sucesso. \n");

    return raiz;
}

// INSERIR
int inserirRaiz(FILE *arqAB, int raiz)
{
    rewind(arqAB);
    fwrite(&raiz, 1, sizeof(int), arqAB);

    return raiz;
}

int inserirAB(FILE *arqAB, int rrn, char chave[KEY_SIZE], int *paginaFilhoPromovido, char *chavePromovida, int ct, int *posChavePromovida)
{
    // Função para inserir isbn dentro da árvore
    ABPagina pagina, novaPagina;
    int encontrado, promovido;

    char chavePromovidaDeBaixo[KEY_SIZE];                      // chave promovida de baixo
    int pos, rrnPromovidoDeBaixo, posChavePosPromovidoDeBaixo; // rrn e pos promovidos de baixo

    // se o rrn for igual a -1 (dizendo que ainda nao possui filhos)
    if (rrn == NIL)
    {
        strcpy(chavePromovida, chave);
        *posChavePromovida = ct;
        *paginaFilhoPromovido = NIL;
        return YES;
    }

    lerArvoreAB(arqAB, rrn, &pagina); // adaptacao

    // procura se a chave já se encontra na árvore e se não, a posição que deve ser inserido
    encontrado = procurarNo(chave, &pagina, &pos);

    // se a chave já se encontra na árvore retorna um erro
    if (encontrado)
    {
        printf("ERRO: Chave duplicada (%s)\n", chave);
        return 0;
    }

    promovido = inserirAB(arqAB, pagina.filho[pos], chave, &rrnPromovidoDeBaixo, chavePromovidaDeBaixo, ct, &posChavePosPromovidoDeBaixo);

    if (!promovido) // não houve promoção
        return NO;

    printf("\n");
    printf("chave promovida baixo: %s\n", chavePromovidaDeBaixo);
    printf("pos chave promovida baixo: %d\n", posChavePosPromovidoDeBaixo);
    printf("%d %d %d\n", pagina.posChave[0], pagina.posChave[1], pagina.posChave[2]);
    printf("%d\n", ct);
    printf("\n");

    // Insere sem a necessidade de split se a quantidade de chaves for menor que 4 (ordem 4)
    if (pagina.ctChaves < MAX_KEYS)
    {
        insereNaPagina(arqAB, chavePromovidaDeBaixo, rrnPromovidoDeBaixo, posChavePosPromovidoDeBaixo, &pagina);

        printf("%d %d %d\n\n", pagina.posChave[0], pagina.posChave[1], pagina.posChave[2]);

        abEscrever(arqAB, rrn, &pagina);

        printf("CHAVE ADICIONADA SEM SPLIT! \n");

        return NO;
    }
    else
    {
        split(arqAB, chavePromovidaDeBaixo, rrnPromovidoDeBaixo, posChavePosPromovidoDeBaixo, &pagina, chavePromovida, paginaFilhoPromovido, posChavePromovida, &novaPagina);

        abEscrever(arqAB, rrn, &pagina);
        abEscrever(arqAB, *paginaFilhoPromovido, &novaPagina);

        return YES;
    }
}

int insereNaPagina(FILE *arqAB, char chave[KEY_SIZE], int rrnPromovidaBaixo, int posChavePosPromovidoBaixo, ABPagina *abPagina)
{
    int i;

    printf("\ninserir Na pagina /// chave: %s / posChaveBaixo: %d\n\n", chave, posChavePosPromovidoBaixo);

    /*
    verificando se a chave é menor que a chave já existente na página e se a posição é diferente de 0
    até achar uma posição para escrever
    */

    for (i = abPagina->ctChaves; strcmp(chave, abPagina->chave[i - 1]) < 0 && i > 0; i--)
    {
        strcpy(abPagina->chave[i], abPagina->chave[i - 1]);
        abPagina->posChave[i] = abPagina->posChave[i - 1];
        abPagina->filho[i + 1] = abPagina->filho[i];
    }

    abPagina->ctChaves++;
    strcpy(abPagina->chave[i], chave);
    abPagina->posChave[i] = posChavePosPromovidoBaixo;
    abPagina->filho[i + 1] = rrnPromovidaBaixo;
}

int procurarNo(char chave[KEY_SIZE], ABPagina *abPagina, int *pos)
{
    // retorna a posicao que a chave está se já existir ou a posição que ela deve ser inserida
    int i;

    for (i = 0; i < abPagina->ctChaves && strcmp(chave, abPagina->chave[i]) > 0; i++)
        ;

    *pos = i;

    if (*pos < abPagina->ctChaves && strcmp(chave, abPagina->chave[*pos]) == 0)
        return YES; // chave esta na pagina

    return NO; // chave nao esta na pagina
}

// SPLIT
void split(FILE *arqAB, char chavePromovidaDeBaixo[KEY_SIZE], int rrnPromovidoDeBaixo, int posPromovidaDeBaixo, ABPagina *paginaAntiga, char *chavePromovida, int *paginaFilhoPromovido, int *posChavePromovida, ABPagina *paginaNova)
{
    int i, j, k;
    char splitChaveBuffer[MAX_KEYS + 1][KEY_SIZE]; // buffer de chave antes do split
    int splitPosBuffer[MAX_KEYS + 1];              // buffer das pos antes do split
    int splitFilhoBuffer[MAX_KEYS + 2];            // buffer dos filhos antes do split

    double div = (double)MAX_KEYS / 2;
    int mid = ceil(div);

    // passa tudo da pagina antiga para os bufferss
    for (i = 0; i < MAX_KEYS; i++)
    {
        strcpy(splitChaveBuffer[i], paginaAntiga->chave[i]);
        splitPosBuffer[i] = paginaAntiga->posChave[i];
        splitFilhoBuffer[i] = paginaAntiga->filho[i];

        printf("SplitPosBuffer[ %d ] = %d\n", i, splitPosBuffer[i]);
    }

    // pega o ultimo filho
    splitFilhoBuffer[i] = paginaAntiga->filho[i];

    // simulando o insert in page no buffer
    for (i = MAX_KEYS; (strcmp(chavePromovidaDeBaixo, splitChaveBuffer[i - 1])) < 0 && i > 0; i--)
    {
        strcpy(splitChaveBuffer[i], splitChaveBuffer[i - 1]);
        splitPosBuffer[i] = splitPosBuffer[i - 1];
        splitFilhoBuffer[i + 1] = splitFilhoBuffer[i];
    }

    strcpy(splitChaveBuffer[i], chavePromovidaDeBaixo);
    splitPosBuffer[i] = posPromovidaDeBaixo;
    splitFilhoBuffer[i + 1] = rrnPromovidoDeBaixo;

    // pegando o rrn da nova pagina
    // o filho a direita da chave recebe o rrn
    *paginaFilhoPromovido = obterPagina(arqAB);

    criarPagina(paginaNova);

    for (i = 0; i < mid; i++)
    {
        // a pagina antiga recebe os valores a esquerda do meio selecionado (filho a esquerda)
        strcpy(paginaAntiga->chave[i], splitChaveBuffer[i]); // 4444444444444
        paginaAntiga->posChave[i] = splitPosBuffer[i];
        paginaAntiga->filho[i] = splitFilhoBuffer[i];

        // pega os valores a direita da metade e joga para a nova pagina (filho a direita)
        strcpy(paginaNova->chave[i], splitChaveBuffer[i + 1 + mid]);
        paginaNova->posChave[i] = splitPosBuffer[i + 1 + mid];
        paginaNova->filho[i] = splitFilhoBuffer[i + mid];

        // Remove os elementos promovidos e dividos da página antiga
        for (j = 0; j < KEY_SIZE - 1; j++)
            paginaAntiga->chave[i + mid - 1][j] = NO_KEY;

        for (k = 0; k < MAX_KEYS; k++)
            paginaAntiga->posChave[i + mid - 1] = NIL;

        paginaAntiga->chave[i + 1 + mid][KEY_SIZE - 1] = '\0';
    }

    // ultimos ponteiros
    paginaAntiga->filho[mid] = splitFilhoBuffer[mid];
    paginaNova->filho[mid] = splitFilhoBuffer[i + mid];

    // contagem de chaves da nova pagina(filho a direita) e o valor antigo menos a quantidade dividida e a pagina antiga é a metade dividida
    paginaNova->ctChaves = MAX_KEYS - mid;
    paginaAntiga->ctChaves = mid;

    // pegando o valor que vai subir e passando para chave promovida
    strcpy(chavePromovida, splitChaveBuffer[mid]);
    *posChavePromovida = splitPosBuffer[mid];

    printf("SPLITADO!\n");
}

void percorrerInOrdemAB(FILE *arqAB, FILE *arqDados, char key[KEY_SIZE])
{
    ABPagina pagina;
    int i = 0;
    int raiz, rrn;

    raiz = obterRaiz(arqAB);
    lerArvoreAB(arqAB, raiz, &pagina);

    while ((strcmp(pagina.chave[i], key) != 0))
    {
        while ((strcmp(pagina.chave[i], key) < 0) && (i < pagina.ctChaves))
            i++;

        if ((strcmp(pagina.chave[i], key) == 0) && (i < pagina.ctChaves))
        {
            break;
        }
        else
        {
            if (pagina.filho[i] != NIL)
            {
                raiz = pagina.filho[i];
                i = 0;
                lerArvoreAB(arqAB, raiz, &pagina);
            }
            else
            {
                printf("Chave nao encontrada! \n");
                return;
            }
        }
    }

    printf("Chave %s encontrada, posChave: %d, pagina: %d, posicao: %d\n", pagina.chave[i], pagina.posChave[i], raiz, i);
}

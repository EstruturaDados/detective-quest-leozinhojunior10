#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------------------------------------
// Estrutura da Sala (nó da árvore binária da mansão)
// ------------------------------------------------------------
typedef struct sala {
    char nome[50];
    char pista[100];       // pista opcional da sala
    struct sala *esq;      // caminho à esquerda
    struct sala *dir;      // caminho à direita
} Sala;

// ------------------------------------------------------------
// Estrutura do nó da árvore de pistas (BST)
// ------------------------------------------------------------
typedef struct pistaNode {
    char pista[100];
    struct pistaNode *esq;
    struct pistaNode *dir;
} PistaNode;

// ------------------------------------------------------------
// Função: criarSala
// Cria dinamicamente um cômodo com nome e pista
// ------------------------------------------------------------
Sala* criarSala(const char* nome, const char* pista) {
    Sala* nova = (Sala*)malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memória para sala!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    if (pista != NULL)
        strcpy(nova->pista, pista);
    else
        strcpy(nova->pista, "");

    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

// ------------------------------------------------------------
// Função: inserirPista
// Insere uma pista na árvore BST em ordem alfabética
// ------------------------------------------------------------
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*)malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else
        raiz->dir = inserirPista(raiz->dir, pista);

    return raiz;
}

// ------------------------------------------------------------
// Função: exibirPistas
// Percorre a BST em-ordem para exibir pistas em ordem alfabética
// ------------------------------------------------------------
void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) return;

    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// ------------------------------------------------------------
// Função: explorarSalasComPistas
// Controla a navegação do jogador pela mansão e coleta de pistas
// ------------------------------------------------------------
void explorarSalasComPistas(Sala* atual, PistaNode** arvorePistas) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está na sala: %s\n", atual->nome);

        // Se a sala tem uma pista, coleta automaticamente
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        }

        printf("\nEscolha um caminho:\n");
        printf("  (e) Ir para a esquerda\n");
        printf("  (d) Ir para a direita\n");
        printf("  (s) Encerrar exploração\n");
        printf("Opção: ");
        scanf(" %c", &escolha);

        if (escolha == 'e') {
            if (atual->esq != NULL)
                atual = atual->esq;
            else
                printf("Não há sala à esquerda!\n");
        }
        else if (escolha == 'd') {
            if (atual->dir != NULL)
                atual = atual->dir;
            else
                printf("Não há sala à direita!\n");
        }
        else if (escolha == 's') {
            printf("\nExploração encerrada pelo jogador.\n");
            return;
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// ------------------------------------------------------------
// Função auxiliar para liberar memória das salas
// ------------------------------------------------------------
void liberarSalas(Sala* raiz) {
    if (raiz == NULL) return;
    liberarSalas(raiz->esq);
    liberarSalas(raiz->dir);
    free(raiz);
}

// ------------------------------------------------------------
// Função auxiliar para liberar memória da BST de pistas
// ------------------------------------------------------------
void liberarPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

// ------------------------------------------------------------
// Função principal
// Monta o mapa da mansão, inicia exploração e mostra as pistas
// ------------------------------------------------------------
int main() {

    // --------- Montagem fixa da mansão -----------
    Sala* hall         = criarSala("Hall de Entrada",        "Pegadas suspeitas no carpete");
    Sala* salaEstar    = criarSala("Sala de Estar",          "Um relógio parado às 03h17");
    Sala* cozinha      = criarSala("Cozinha",                "Uma faca fora do lugar");
    Sala* escritorio   = criarSala("Escritório",             "Documentos rasgados no chão");
    Sala* biblioteca   = criarSala("Biblioteca",             "Um livro faltando na estante");

    // Conexões da árvore
    hall->esq = salaEstar;
    hall->dir = cozinha;

    salaEstar->esq = escritorio;
    salaEstar->dir = biblioteca;

    // ---------------- Início da exploração ----------------
    printf("=== Detective Quest – Coleta de Pistas ===\n");

    PistaNode* arvorePistas = NULL;

    explorarSalasComPistas(hall, &arvorePistas);

    // ---------------- Exibição final das pistas ----------------
    printf("\n=========================================\n");
    printf("       PISTAS COLETADAS (A-Z)\n");
    printf("=========================================\n");

    if (arvorePistas == NULL)
        printf("Nenhuma pista foi coletada!\n");
    else
        exibirPistas(arvorePistas);

    // ---------------- Liberação de memória ----------------
    liberarSalas(hall);
    liberarPistas(arvorePistas);

    return 0;
}

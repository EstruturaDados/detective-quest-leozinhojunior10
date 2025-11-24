#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------
// Estrutura da sala (nó da árvore binária)
// -----------------------------------------------------------
typedef struct sala {
    char nome[50];
    struct sala *esq;   // Caminho para a esquerda
    struct sala *dir;   // Caminho para a direita
} Sala;

// -----------------------------------------------------------
// Função: criarSala
// Cria dinamicamente uma sala com o nome indicado
// -----------------------------------------------------------
Sala* criarSala(const char* nome) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

// -----------------------------------------------------------
// Função: explorarSalas
// Permite ao jogador navegar pela mansão escolhendo
// esquerda (e), direita (d) ou sair (s)
// -----------------------------------------------------------
void explorarSalas(Sala* atual) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está na: %s\n", atual->nome);

        // Chegou a um nó-folha
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Você chegou ao fim da exploração! Não há mais caminhos.\n");
            return;
        }

        printf("Escolha o caminho:\n");
        printf("  (e) Ir para a esquerda\n");
        printf("  (d) Ir para a direita\n");
        printf("  (s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &escolha);

        if (escolha == 'e') {
            if (atual->esq != NULL) {
                atual = atual->esq;
            } else {
                printf("Não há sala à esquerda!\n");
            }
        }
        else if (escolha == 'd') {
            if (atual->dir != NULL) {
                atual = atual->dir;
            } else {
                printf("Não há sala à direita!\n");
            }
        }
        else if (escolha == 's') {
            printf("Exploração encerrada pelo jogador.\n");
            return;
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// -----------------------------------------------------------
// Função: main
// Monta o mapa da mansão (árvore binária) e inicia exploração
// -----------------------------------------------------------
int main() {

    // Criando salas (nós da árvore)
    Sala* hall = criarSala("Hall de Entrada");
    Sala* salaEstar = criarSala("Sala de Estar");
    Sala* cozinha   = criarSala("Cozinha");
    Sala* escritorio = criarSala("Escritório");
    Sala* biblioteca = criarSala("Biblioteca");

    // Ligando as salas (estrutura fixa da mansão)
    hall->esq = salaEstar;
    hall->dir = cozinha;

    salaEstar->esq = escritorio;   // folha
    salaEstar->dir = biblioteca;   // folha

    // cozinha -> sem filhos (folha)

    printf("=== Detective Quest – Exploração da Mansão ===\n");
    explorarSalas(hall);

    // Liberação de memória (boa prática)
    free(escritorio);
    free(biblioteca);
    free(salaEstar);
    free(cozinha);
    free(hall);

    return 0;
}

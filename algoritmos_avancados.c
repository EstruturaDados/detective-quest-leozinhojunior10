#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------
  Estrutura que representa uma sala da mansão.
  Cada sala possui:
   - nome: string com o nome da sala
   - esquerda: ponteiro para a sala à esquerda
   - direita: ponteiro para a sala à direita
---------------------------------------------------------*/
typedef struct sala {
    char nome[50];
    struct sala *esquerda;
    struct sala *direita;
} Sala;

/*---------------------------------------------------------
  Função criarSala()
  Cria dinamicamente uma sala com o nome informado.
---------------------------------------------------------*/
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;

    return nova;
}

/*---------------------------------------------------------
  Função explorarSalas()
  Permite ao jogador navegar pela mansão.
  O jogador escolhe ir para a esquerda (e), direita (d)
  ou sair (s). A exploração termina quando o jogador
  chega a um nó-folha, que não possui caminhos.
---------------------------------------------------------*/
void explorarSalas(Sala *atual) {

    while (atual != NULL) {
        printf("\nVocê está na sala: %s\n", atual->nome);

        // Se for um cômodo sem caminhos, interrompe a exploração
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Não há mais caminhos a partir daqui.\n");
            printf("Exploração encerrada!\n");
            return;
        }

        printf("Escolha seu caminho (e = esquerda, d = direita, s = sair): ");
        char opcao;
        scanf(" %c", &opcao);

        if (opcao == 'e') {
            if (atual->esquerda != NULL) atual = atual->esquerda;
            else printf("Não existe sala à esquerda!\n");
        }
        else if (opcao == 'd') {
            if (atual->direita != NULL) atual = atual->direita;
            else printf("Não existe sala à direita!\n");
        }
        else if (opcao == 's') {
            printf("Saindo da exploração...\n");
            return;
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

/*---------------------------------------------------------
  Função main()
  Monta a estrutura fixa da mansão em forma de árvore
  binária e inicia a exploração.
---------------------------------------------------------*/
int main() {

    // Criação manual da árvore (estrutura da mansão)
    Sala *hall        = criarSala("Hall de Entrada");
    Sala *salaEstar   = criarSala("Sala de Estar");
    Sala *biblioteca  = criarSala("Biblioteca");
    Sala *cozinha     = criarSala("Cozinha");
    Sala *jardim      = criarSala("Jardim");

    // Montando o mapa
    hall->esquerda = salaEstar;
    hall->direita  = biblioteca;

    salaEstar->esquerda = cozinha;
    salaEstar->direita  = jardim;

    // Inicia a exploração
    printf("=== DETECTIVE QUEST: EXPLORAÇÃO DA MANSÃO ===\n");
    explorarSalas(hall);

    return 0;
}

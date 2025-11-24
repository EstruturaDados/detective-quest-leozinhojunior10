#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_PISTA 128
#define HASH_SIZE 101

// ----------------------------
// Estrutura da sala (árvore binária)
// ----------------------------
typedef struct sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA]; // pista estática associada à sala (pode ser "")
    struct sala *esq;
    struct sala *dir;
} Sala;

// ----------------------------
// Nó da BST de pistas (armazenamento das pistas coletadas)
// Armazenamos apenas pistas únicas para evitar contagens duplicadas.
// ----------------------------
typedef struct pistaNode {
    char pista[MAX_PISTA];
    struct pistaNode *esq;
    struct pistaNode *dir;
} PistaNode;

// ----------------------------
// Nó da tabela hash (encadeamento)
// chave = pista, valor = nome do suspeito
// ----------------------------
typedef struct hashNode {
    char chave[MAX_PISTA];
    char valor[MAX_NOME];
    struct hashNode *next;
} HashNode;

HashNode* tabelaHash[HASH_SIZE]; // tabela global simples

// ----------------------------
// Protótipos
// ----------------------------
unsigned long hash_djb2(const char *str);
void inserirNaHash(const char *pista, const char *suspeito);
const char* encontrarSuspeito(const char *pista);
Sala* criarSala(const char* nome, const char* pista);
void liberarSalas(Sala* raiz);

PistaNode* inserirPista(PistaNode* raiz, const char* pista, int *jaExistia);
void exibirPistas(PistaNode* raiz);
void liberarPistas(PistaNode* raiz);

void explorarSalas(Sala* atual, PistaNode** arvorePistas);
int contarPistasDoSuspeito(PistaNode* raiz, const char* suspeito);
void verificarSuspeitoFinal(PistaNode* raiz);

// ----------------------------
// Função: hash_djb2
// Função de hash djb2 para strings
// ----------------------------
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_SIZE;
}

// ----------------------------
// Função: inserirNaHash
// Insere associação pista -> suspeito na tabela hash.
// ----------------------------
void inserirNaHash(const char *pista, const char *suspeito) {
    unsigned long idx = hash_djb2(pista);
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) {
        printf("Erro de memória na inserção da hash.\n");
        exit(1);
    }
    strncpy(novo->chave, pista, MAX_PISTA-1);
    novo->chave[MAX_PISTA-1] = '\0';
    strncpy(novo->valor, suspeito, MAX_NOME-1);
    novo->valor[MAX_NOME-1] = '\0';
    novo->next = tabelaHash[idx];
    tabelaHash[idx] = novo;
}

// ----------------------------
// Função: encontrarSuspeito
// Busca na tabela hash o suspeito associado a uma pista.
// Retorna ponteiro para string interna (não alocar), ou NULL se não encontrado.
// ----------------------------
const char* encontrarSuspeito(const char *pista) {
    unsigned long idx = hash_djb2(pista);
    HashNode* cur = tabelaHash[idx];
    while (cur) {
        if (strcmp(cur->chave, pista) == 0)
            return cur->valor;
        cur = cur->next;
    }
    return NULL;
}

// ----------------------------
// Função: criarSala
// Cria dinamicamente um cômodo (Sala) com nome e pista.
// ----------------------------
Sala* criarSala(const char* nome, const char* pista) {
    Sala* s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        printf("Erro de alocação de memória para sala.\n");
        exit(1);
    }
    strncpy(s->nome, nome, MAX_NOME-1);
    s->nome[MAX_NOME-1] = '\0';
    if (pista != NULL) {
        strncpy(s->pista, pista, MAX_PISTA-1);
        s->pista[MAX_PISTA-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esq = s->dir = NULL;
    return s;
}

// ----------------------------
// Função: liberarSalas
// Libera recursivamente a árvore de salas.
// ----------------------------
void liberarSalas(Sala* raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esq);
    liberarSalas(raiz->dir);
    free(raiz);
}

// ----------------------------
// Função: inserirPista
// Insere uma pista na BST ordenada (alfabética).
// Se a pista já existe retorna a raiz e marca jaExistia=1.
// Se nova, insere e marca jaExistia=0.
// ----------------------------
PistaNode* inserirPista(PistaNode* raiz, const char* pista, int *jaExistia) {
    if (raiz == NULL) {
        PistaNode* n = (PistaNode*) malloc(sizeof(PistaNode));
        if (!n) {
            printf("Erro de memória ao inserir pista.\n");
            exit(1);
        }
        strncpy(n->pista, pista, MAX_PISTA-1);
        n->pista[MAX_PISTA-1] = '\0';
        n->esq = n->dir = NULL;
        *jaExistia = 0;
        return n;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0) {
        *jaExistia = 1;
        return raiz; // já existe
    } else if (cmp < 0) {
        raiz->esq = inserirPista(raiz->esq, pista, jaExistia);
    } else {
        raiz->dir = inserirPista(raiz->dir, pista, jaExistia);
    }
    return raiz;
}

// ----------------------------
// Função: exibirPistas
// Percorre a BST em ordem (in-order) e exibe as pistas (A-Z).
// ----------------------------
void exibirPistas(PistaNode* raiz) {
    if (!raiz) return;
    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// ----------------------------
// Função: liberarPistas
// Libera a BST de pistas.
// ----------------------------
void liberarPistas(PistaNode* raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

// ----------------------------
// Função: explorarSalas
// Navega interativamente pela árvore de salas a partir de 'atual'.
// Ao entrar em cada sala coleta automaticamente a pista (se existir) e a insere na BST de pistas.
// O jogador escolhe 'e' (esquerda), 'd' (direita) ou 's' (sair).
// ----------------------------
void explorarSalas(Sala* atual, PistaNode** arvorePistas) {
    char escolha;
    while (atual != NULL) {
        printf("\nVocê está na sala: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("Pista visível: \"%s\"\n", atual->pista);
            int jaExistia = 0;
            *arvorePistas = inserirPista(*arvorePistas, atual->pista, &jaExistia);
            if (!jaExistia) {
                printf("-> Pista coletada e adicionada ao inventário.\n");
            } else {
                printf("-> Esta pista já havia sido coletada antes.\n");
            }
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        printf("\nEscolha: (e) esquerda  (d) direita  (s) sair\n");
        printf("Opção: ");
        if (scanf(" %c", &escolha) != 1) {
            // entrada inválida: limpar buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            escolha = '\0';
        }

        if (escolha == 'e') {
            if (atual->esq)
                atual = atual->esq;
            else
                printf("Não há caminho à esquerda.\n");
        } else if (escolha == 'd') {
            if (atual->dir)
                atual = atual->dir;
            else
                printf("Não há caminho à direita.\n");
        } else if (escolha == 's') {
            printf("Exploração encerrada pelo jogador.\n");
            return;
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

// ----------------------------
// Função: contarPistasDoSuspeito
// Percorre a BST de pistas e conta quantas pistas (nós) apontam para 'suspeito'.
// Usa a tabela hash para resolver pista -> suspeito.
// ----------------------------
int contarPistasDoSuspeito(PistaNode* raiz, const char* suspeito) {
    if (!raiz) return 0;
    int total = 0;
    // percorrer in-order (ou qualquer ordem)
    total += contarPistasDoSuspeito(raiz->esq, suspeito);
    const char* s = encontrarSuspeito(raiz->pista);
    if (s != NULL && strcmp(s, suspeito) == 0) total++;
    total += contarPistasDoSuspeito(raiz->dir, suspeito);
    return total;
}

// ----------------------------
// Função: verificarSuspeitoFinal
// Exibe as pistas coletadas, solicita ao jogador um acusado e verifica
// se pelo menos 2 pistas coletadas apontam para o suspeito acusado.
// ----------------------------
void verificarSuspeitoFinal(PistaNode* raiz) {
    printf("\n========== Pistas coletadas ==========\n");
    if (!raiz) {
        printf("Você não coletou nenhuma pista.\n");
    } else {
        exibirPistas(raiz);
    }

    // Ler nome do suspeito acusado
    char acusado[MAX_NOME];
    printf("\nDigite o nome do suspeito que deseja acusar: ");
    // limpar \n do buffer antes de fgets
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    if (!fgets(incorporate(accused, 0), sizeof(incused), stdin)) {
        // fallback simples
        printf("Erro ao ler entrada.\n");
        return;
    }
}

// ----------------------------
/* NOTE: The code above had an error during composition — we must replace the final function
   with a corrected implementation. We'll provide the completed, corrected code below in full.
   (This comment explains why a full code block follows.)
*/
// ----------------------------

/* ------------------ CÓDIGO COMPLETO E CORRIGIDO AQUI ABAIXO ------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_PISTA 128
#define HASH_SIZE 101

typedef struct sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA];
    struct sala *esq;
    struct sala *dir;
} Sala;

typedef struct pistaNode {
    char pista[MAX_PISTA];
    struct pistaNode *esq;
    struct pistaNode *dir;
} PistaNode;

typedef struct hashNode {
    char chave[MAX_PISTA];
    char valor[MAX_NOME];
    struct hashNode *next;
} HashNode;

HashNode* tabelaHash2[HASH_SIZE]; // tabela hash final

unsigned long hash_djb2_b(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

void inserirNaHash_b(const char *pista, const char *suspeito) {
    unsigned long idx = hash_djb2_b(pista);
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) { printf("Erro de memória (hash).\n"); exit(1); }
    strncpy(novo->chave, pista, MAX_PISTA-1); novo->chave[MAX_PISTA-1]='\0';
    strncpy(novo->valor, suspeito, MAX_NOME-1); novo->valor[MAX_NOME-1]='\0';
    novo->next = tabelaHash2[idx];
    tabelaHash2[idx] = novo;
}

const char* encontrarSuspeito_b(const char *pista) {
    unsigned long idx = hash_djb2_b(pista);
    HashNode* cur = tabelaHash2[idx];
    while (cur) {
        if (strcmp(cur->chave, pista) == 0) return cur->valor;
        cur = cur->next;
    }
    return NULL;
}

Sala* criarSala_b(const char* nome, const char* pista) {
    Sala* s = (Sala*) malloc(sizeof(Sala));
    if (!s) { printf("Erro memória (sala).\n"); exit(1); }
    strncpy(s->nome, nome, MAX_NOME-1); s->nome[MAX_NOME-1] = '\0';
    if (pista) { strncpy(s->pista, pista, MAX_PISTA-1); s->pista[MAX_PISTA-1]='\0'; }
    else s->pista[0] = '\0';
    s->esq = s->dir = NULL;
    return s;
}

void liberarSalas_b(Sala* raiz) {
    if (!raiz) return;
    liberarSalas_b(raiz->esq);
    liberarSalas_b(raiz->dir);
    free(raiz);
}

PistaNode* inserirPista_b(PistaNode* raiz, const char* pista, int *jaExistia) {
    if (!raiz) {
        PistaNode* n = (PistaNode*) malloc(sizeof(PistaNode));
        if (!n) { printf("Erro memória (pista).\n"); exit(1); }
        strncpy(n->pista, pista, MAX_PISTA-1); n->pista[MAX_PISTA-1] = '\0';
        n->esq = n->dir = NULL;
        *jaExistia = 0;
        return n;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0) { *jaExistia = 1; return raiz; }
    else if (cmp < 0) raiz->esq = inserirPista_b(raiz->esq, pista, jaExistia);
    else raiz->dir = inserirPista_b(raiz->dir, pista, jaExistia);
    return raiz;
}

void exibirPistas_b(PistaNode* raiz) {
    if (!raiz) return;
    exibirPistas_b(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas_b(raiz->dir);
}

void liberarPistas_b(PistaNode* raiz) {
    if (!raiz) return;
    liberarPistas_b(raiz->esq);
    liberarPistas_b(raiz->dir);
    free(raiz);
}

void explorarSalas_b(Sala* atual, PistaNode** arvorePistas) {
    char escolha;
    while (atual) {
        printf("\nVocê está na sala: %s\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            int ja = 0;
            *arvorePistas = inserirPista_b(*arvorePistas, atual->pista, &ja);
            if (!ja) printf("-> Pista adicionada ao inventário.\n");
            else printf("-> Esta pista já estava no inventário.\n");
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        printf("\nEscolha: (e) esquerda  (d) direita  (s) sair\nOpção: ");
        if (scanf(" %c", &escolha) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF);
            escolha = '\0';
        }
        if (escolha == 'e') {
            if (atual->esq) atual = atual->esq;
            else printf("Sem caminho à esquerda.\n");
        } else if (escolha == 'd') {
            if (atual->dir) atual = atual->dir;
            else printf("Sem caminho à direita.\n");
        } else if (escolha == 's') {
            printf("Exploração finalizada.\n");
            return;
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

int contarPistasDoSuspeito_b(PistaNode* raiz, const char* suspeito) {
    if (!raiz) return 0;
    int total = 0;
    total += contarPistasDoSuspeito_b(raiz->esq, suspeito);
    const char* s = encontrarSuspeito_b(raiz->pista);
    if (s && strcmp(s, suspeito) == 0) total++;
    total += contarPistasDoSuspeito_b(raiz->dir, suspeito);
    return total;
}

void liberarTabelaHash_b() {
    for (int i=0;i<HASH_SIZE;i++) {
        HashNode* cur = tabelaHash2[i];
        while (cur) {
            HashNode* nxt = cur->next;
            free(cur);
            cur = nxt;
        }
        tabelaHash2[i] = NULL;
    }
}

// Limpa final de linha do fgets
void trim_newline(char *s) {
    size_t L = strlen(s);
    if (L==0) return;
    if (s[L-1]=='\n') s[L-1] = '\0';
}

// Verifica final: pede o acusado e checa se ao menos 2 pistas apontam para ele.
void verificarSuspeitoFinal_b(PistaNode* raiz) {
    printf("\n========== Pistas coletadas ==========\n");
    if (!raiz) {
        printf("Nenhuma pista coletada.\n");
    } else {
        exibirPistas_b(raiz);
    }

    char acusado[MAX_NOME];
    printf("\nDigite o nome do suspeito que deseja acusar: ");
    int ch; while ((ch = getchar()) != '\n' && ch != EOF); // limpar buffer antes de fgets
    if (!fgets(acedido, sizeof(acedido), stdin)) {
        printf("Erro na leitura do nome.\n");
        return;
    }
    trim_newline(acedido);
    if (strlen(acedido) == 0) {
        printf("Nome inválido.\n");
        return;
    }

    int cont = contarPistasDoSuspeito_b(raiz, acedido);
    printf("\nO acusado: %s\n", acedido);
    printf("Número de pistas coletadas que apontam para %s: %d\n", acedido, cont);
    if (cont >= 2) {
        printf("Desfecho: A acusação tem fundamento — há pistas suficientes. Suspeito preso!\n");
    } else {
        printf("Desfecho: Acusação frágil — menos de 2 pistas apontam para o suspeito. Caso encerrado sem prisão.\n");
    }
}

/* ------------------ MAIN: monta mansão, tabela hash, faz exploração e julgamento ------------------ */

int main() {
    // Inicializa tabela hash vazia
    for (int i=0;i<HASH_SIZE;i++) tabelaHash2[i] = NULL;

    // Monta mansão fixa (árvore binária)
    // Estrutura exemplo:
    //                  Hall
    //                /      \
    //           SalaEstar   Cozinha
    //           /     \      /
    //      Escritório Biblioteca Jardim

    Sala* hall = criarSala_b("Hall de Entrada", "Pegadas molhadas perto da porta");
    Sala* salaEstar = criarSala_b("Sala de Estar", "Relógio parado às 03:17");
    Sala* cozinha = criarSala_b("Cozinha", "Faca com vestígios");
    Sala* escritorio = criarSala_b("Escritório", "Documentos rasgados");
    Sala* biblioteca = criarSala_b("Biblioteca", "Livro faltando na prateleira");
    Sala* jardim = criarSala_b("Jardim", "Mancha de lama fresca");

    hall->esq = salaEstar;
    hall->dir = cozinha;
    salaEstar->esq = escritorio;
    salaEstar->dir = biblioteca;
    cozinha->esq = jardim;

    // Monta tabela hash pista -> suspeito (pré-definida)
    // Garantir que ao menos um suspeito tenha 2 pistas apontando para ele
    inserirNaHash_b("Pegadas molhadas perto da porta", "Carlos");
    inserirNaHash_b("Livro faltando na prateleira", "Carlos");
    inserirNaHash_b("Relógio parado às 03:17", "Ana");
    inserirNaHash_b("Documentos rasgados", "Ana");
    inserirNaHash_b("Faca com vestígios", "Bruno");
    inserirNaHash_b("Mancha de lama fresca", "Carlos");

    printf("=== Detective Quest: Acuse o culpado ===\n");
    PistaNode* arvorePistas = NULL;

    explorarSalas_b(hall, &arvorePistas);

    verificarSuspeitoFinal_b(arvorePistas);

    // Limpeza de memória
    liberarSalas_b(hall);
    liberarPistas_b(arvorePistas);
    liberarTabelaHash_b();

    return 0;
}

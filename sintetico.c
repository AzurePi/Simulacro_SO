#include "sintetico.h"

BCP *BCP_From_Sintetico(FILE *programa) {
    //se não há programa para ler, retorna NULL
    if (!programa) {
        printf("ERRO: arquivo não encontrado");
        return NULL;
    }

    BCP *processo = malloc(sizeof(BCP));

    // lê os campos do BCP no programa sintético; se houver erro na leitura, informa que há problema no arquivo
    if (!fscanf(programa, "%[60]s", processo->nome) ||
        !fscanf(programa, "%d", &processo->id_seg) ||
        !fscanf(programa, "%d", &processo->prioridade_OG) ||
        !fscanf(programa, "%d", &processo->tamanho_seg)
            ) {
        printf("ERRO: programa sintético contém erro no BCP");
        return NULL;
    }

    processo->estado = 0; //diz que o programa está pronto para ser executado

    char *semaforos = (char *) malloc(51 * sizeof(char));
    char caractere;
    int i = 0;

    //enquanto não chegamos no fim da linha e há caracteres para ler
    while ((caractere = fgetc(programa)) != '\n' && caractere != EOF && i < 50) {
        semaforos[i] = caractere; //guardamos o caractere
        i++;
    }
    semaforos[i] = '\0'; //finaliza a string

    //para cada semáforo guardado, cria uma struct semáforo e coloca no BCP


    free(semaforos);
    return processo;
}


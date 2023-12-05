#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5   // Total de vagas no estacionamento
#define VAGAS_PORTA 10   // Vagas na porta do estacionamento
#define QUANTIDADE_CARROS 20


sem_t sem_vagas, sem_vagas_porta, nao_pode;
sem_t mutex;
int contador = 0;

void* carro_func(void* arg) {
    int carro_id = *(int*)arg;
    if (sem_trywait(&sem_vagas) == 0) 
    {
        printf("Carro %d estacionou em uma vaga.\n", carro_id);
        contador++;
        sleep(rand() % 5 + 1); //Sorteia um número aleatório relacionado ao tempo que o carro deverá ficar estacionado
        sem_post(&sem_vagas);
        contador--;
        printf("Carro %d saiu do estacionamento.\n", carro_id);
    } 
    else if (sem_trywait(&sem_vagas_porta) == 0) 
    {
        printf("Carro %d estacionou na porta do estacionamento.\n", carro_id);
        sleep(rand() % 10 + 5);

        if(contador < N)
        {
            printf("Carro %d saiu da porta e entrou no estacionamento.\n", carro_id);
            sem_post(&sem_vagas_porta);
            sleep(rand() % 5 + 1);
            printf("Carro %d saiu do estacionamento.\n", carro_id);
            contador--;
        }
    } 
    else{
        printf("Carro %d não encontrou vaga e procurará outro estacionamento.\n", carro_id);
    }

    free(arg);
    return NULL;
}

int main() {
    srand(time(NULL));

    sem_init(&sem_vagas, 0, N);
    sem_init(&sem_vagas_porta, 0, VAGAS_PORTA);
    sem_init(&mutex, 0, 1);

    pthread_t carros[QUANTIDADE_CARROS];

    for (int i = 0; i < QUANTIDADE_CARROS; ++i) {
        int* carro_id = (int*)malloc(sizeof(int));
        *carro_id = i;
        pthread_create(&carros[i], NULL, carro_func, (void*)carro_id);
    }

    for (int i = 0; i < QUANTIDADE_CARROS; ++i) {
        pthread_join(carros[i], NULL);
    }

    sem_destroy(&sem_vagas);
    sem_destroy(&sem_vagas_porta);
    sem_destroy(&mutex);

    return 0;
}

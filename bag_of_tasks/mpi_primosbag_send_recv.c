#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

#define TAG_TASK 0
#define TAG_RESULT 1
#define TAG_TERMINATE 2

int primo(long int n) {
    int i;

    for (i = 3; i < (int)(sqrt(n) + 1); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    double t_inicial, t_final;
    int total = 0;
    long int n;
    int meu_ranque, num_procs;
    int task = 0;

    if (argc < 2) {
        printf("Valor inválido! Entre com um valor do maior inteiro\n");
        return 0;
    } else {
        n = strtol(argv[1], (char **)NULL, 10);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    t_inicial = MPI_Wtime();

    while (1) {
        if (meu_ranque == 0) {
            // Mestre
            int source, received_result;
            MPI_Status status;

            // Envia uma nova tarefa para o processo que solicitou
            MPI_Recv(&source, 1, MPI_INT, MPI_ANY_SOURCE, TAG_TASK, MPI_COMM_WORLD, &status);

            if (task <= n) {
                MPI_Send(&task, 1, MPI_LONG, source, TAG_TASK, MPI_COMM_WORLD);
                task += 2;
            } else {
                // Se todas as tarefas foram concluídas, envia uma mensagem de término
                for (int i = 1; i < num_procs; i++) {
                    MPI_Send(&task, 1, MPI_LONG, i, TAG_TERMINATE, MPI_COMM_WORLD);
                }
                break;
            }

            // Recebe o resultado da tarefa concluída
            MPI_Recv(&received_result, 1, MPI_INT, source, TAG_RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Soma o resultado total
            total += received_result;
        } else {
            // Escravo
            int my_task;
            MPI_Status status;

            // Solicita uma nova tarefa
            MPI_Send(&meu_ranque, 1, MPI_INT, 0, TAG_TASK, MPI_COMM_WORLD);
            MPI_Recv(&my_task, 1, MPI_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TAG_TASK) {
                // Executa a tarefa
                int my_result = primo(my_task);
                
                // Envia o resultado de volta para o mestre
                MPI_Send(&my_result, 1, MPI_INT, 0, TAG_RESULT, MPI_COMM_WORLD);
            } else if (status.MPI_TAG == TAG_TERMINATE) {
                // Se receber uma mensagem de término, termina o loop
                break;
            }
        }
    }

    t_final = MPI_Wtime();

    if (meu_ranque == 0) {
        printf("Quant. de primos entre 1 e n: %d \n", total + 1);
        printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);
    }

    MPI_Finalize();
    return 0;
}

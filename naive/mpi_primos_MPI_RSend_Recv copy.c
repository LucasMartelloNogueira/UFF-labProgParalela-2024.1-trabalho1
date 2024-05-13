#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo(long int n) {
    int i;

    for (i = 3; i < (int)(sqrt(n) + 1); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}
//so funciona para numeros impares

int main(int argc, char *argv[]) {
    double t_inicial, t_final;
    int cont = 0, total = 0;
    long int i, n;
    int meu_ranque, num_procs, inicio, salto;

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
    inicio = 3 + meu_ranque * 2;
    salto = num_procs * 2;

    //o processo 1: start 3
    //o processo 2: start 5
    //o processo 3: start 7
    //o processo 4: start 9

    for (i = inicio; i <= n; i += salto) {
        if (primo(i) == 1) cont++;
    }

    if (meu_ranque != 0) {
        // Envia o valor de cont para o processo mestre (rank 0)
        MPI_Rsend(&cont, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        // Recebe os resultados dos outros processos e soma ao total
        total = cont;
        for (int source = 1; source < num_procs; source++) {
            int received_cont;
            MPI_Recv(&received_cont, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += received_cont;
        }

        total += 1; // Acrescenta o dois, que também é primo
        t_final = MPI_Wtime();

        printf("Quant. de primos entre 1 e n: %d \n", total);
        printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);
    }

    MPI_Finalize();
    return 0;
}

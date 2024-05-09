#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo (long int n) { /* mpi_primos.c  */
	int i;
       
	for (i = 3; i < (int)(sqrt(n) + 1); i+=2) {
			if(n%i == 0) return 0;
	}
    // printf("%d eh primo\n", n);
	return 1;
}

int main(int argc, char *argv[]) {
	double t_inicial, t_final;
	int cont = 0, total = 0;
	long int i, n;
	int meu_ranque, num_procs, inicio, salto;
    int destino = 0;
    int tamMsg = 1;
    int etiq = 1;
    MPI_Status estado;

    // printf("argv[1]: %s\n", argv[1]);

	if (argc < 2) {
        	printf("Valor inválido! Entre com um valor do maior inteiro\n");
       	 	return 0;
    	} else {
        	n = strtol(argv[1], (char **) NULL, 10);
       	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);	
    t_inicial = MPI_Wtime();
    inicio = 3 + meu_ranque*2;
    salto = num_procs*2;
	for (i = inicio; i <= n; i += salto) {	
		if(primo(i) == 1) cont++;
	}

    // printf("processo %d achou %d primos\n", meu_ranque, cont);
		
	if (meu_ranque > 0) {
        MPI_Ssend(&cont, tamMsg, MPI_INT, destino, etiq, MPI_COMM_WORLD);

        t_final = MPI_Wtime();

        // printf("processo %d / tempo de execucao: %1.3f\n", meu_ranque, t_final - t_inicial);
    }

	if (meu_ranque == 0) {

        int processosFinalizados = 0;
        int recebido = 0;
        total += cont; // contabilizando as contagens do processo 0

		// recebendo a contagem de primos dos outros processos
        while (processosFinalizados < num_procs - 1){
            MPI_Recv(&recebido, tamMsg, MPI_INT, MPI_ANY_SOURCE, etiq, MPI_COMM_WORLD, &estado);
            processosFinalizados++;
            total += recebido;
            // printf("recebido: %d / total parcial: %d\n", recebido, total);
        }

        t_final = MPI_Wtime();

        printf("processo %d / tempo de execucao: %1.3f\n", meu_ranque, t_final - t_inicial);

        total += 1;    /* Acrescenta o dois, que também é primo */
		printf("Quant. de primos entre 1 e %d: %d \n", n, total);
		// printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);	 
	}
	MPI_Finalize();
	return(0);
}
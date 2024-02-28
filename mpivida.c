#include <mpi.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define N 8
unsigned char g_Game[N][N];

void Process(int rank, int baseXProcess, int baseYProcess, int numPerProcess)
{
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(0));

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int numPerProcess = (int)ceil((N * N) / size);
    const int baseProcess = (numPerProcess * rank);
    const int baseYProcess = (int)(baseProcess / N);
    const int baseXProcess = (int)(baseProcess % N);

    //printf("Process(%d/%d): numPerProcess %d baseProcess %d base [%d, %d]\n", rank, size, numPerProcess, baseProcess, baseYProcess, baseXProcess);

    if (rank == 0)
    {
        memset(g_Game, 0, sizeof(g_Game));

        int min = (N / 10);
        if (min < 1)
            min = 1;

        for (int i = 0; i < N; ++i)
            for (int j = 0; j < min; ++j)
                g_Game[i][rand() % N] = 1;

        for (int i = 0; i < 10; ++i)
        {
            // Envia todo o jogo para os outros processos:
            for (int rankId = 1; rankId < size; ++rankId)
                if (MPI_Ssend(g_Game, N * N, MPI_UNSIGNED_CHAR, rankId, 0, MPI_COMM_WORLD) != MPI_SUCCESS)
                    printf("Process(%d/%d): MPI_Ssend fail.\n", rank, size);

            // Processa a parte do rank=0.
            Process(rank, baseXProcess, baseYProcess, numPerProcess);

            // Recebe as partes dos outros processos:
            MPI_Status st;

            for (int rankId = 1; rankId < size; ++rankId)
            {
                const int otherBaseProcess = (numPerProcess * rankId);
                const int otherBaseYProcess = (int)(otherBaseProcess / N);
                const int otherBaseXProcess = (int)(otherBaseProcess % N);

                if (MPI_Recv(&g_Game[otherBaseYProcess][otherBaseXProcess], numPerProcess, MPI_UNSIGNED_CHAR, rankId, 1, MPI_COMM_WORLD, &st) != MPI_SUCCESS)
                    printf("Process(%d/%d): MPI_Recv fail.\n", rank, size);
            }

            // Exibe o tabuleiro do jogo:
            printf("Jogo da Vida #%02d:\n", i);

            for (int y = 0; y < N; ++y)
            {
                for (int x = 0; x < N; ++x)
                    printf("%d ", g_Game[y][x]);

                printf("\n");
            }
        }
    }
    else
    {
        MPI_Status st;

        for (int i = 0; i < 10; ++i)
        {
            // Recebe todo o jogo e processa somente a parte do processo atual:
            if (MPI_Recv(g_Game, N * N, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, &st) == MPI_SUCCESS)
                Process(rank, baseXProcess, baseYProcess, numPerProcess);
            else
                printf("Process(%d/%d): MPI_Recv fail.\n", rank, size);

            // Envia a sua parte processada para o processo rank=0:
            if (MPI_Ssend(&g_Game[baseYProcess][baseXProcess], numPerProcess, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD) != MPI_SUCCESS)
                printf("Process(%d/%d): MPI_Ssend fail.\n", rank, size);
        }
    }

    MPI_Finalize();
    return 0;
}


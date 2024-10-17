#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

constexpr int RANK_MASTER = 0;      // Rango del proceso maestro
constexpr int NUM_DATOS = 3;        // Número de datos a escribir y leer
const char* NOMBRE_FICHERO = "Practica5.dat";  // Nombre del fichero

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double startTime = MPI_Wtime();

	// Inicializamos el fichero
    MPI_File fh;
    MPI_File_open(
        MPI_COMM_WORLD, 
        const_cast<char*>(NOMBRE_FICHERO), 
        MPI_MODE_CREATE | MPI_MODE_WRONLY, 
        MPI_INFO_NULL, 
        &fh
    );

    // Escribir el rango del proceso en el fichero 3 veces
    char write_values[NUM_DATOS];
    for (int i = 0; i < NUM_DATOS; ++i) {
        write_values[i] = rank + 48; // Convertir el rango a ASCII
    }
    MPI_Offset offset = rank * NUM_DATOS * sizeof(char);
    MPI_File_write_at(fh, offset, write_values, NUM_DATOS, MPI_CHAR, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    // Leer los valores del fichero
    MPI_File_open(MPI_COMM_WORLD, const_cast<char*>(NOMBRE_FICHERO), MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    char read_values[NUM_DATOS];
    MPI_File_read_at(fh, offset, read_values, NUM_DATOS, MPI_CHAR, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);

    // Recolectar los valores leídos en el proceso maestro
    vector<char> all_values(size * NUM_DATOS);
    MPI_Gather(read_values, NUM_DATOS, MPI_CHAR, all_values.data(), NUM_DATOS, MPI_CHAR, RANK_MASTER, MPI_COMM_WORLD);

    // Calcular el tiempo de ejecución
    double endTime = MPI_Wtime();

    if (rank == RANK_MASTER) {
        for (int i = 0; i < size; ++i) {
            printf("Proceso %d leyó los valores: ", i);
            for (int j = 0; j < NUM_DATOS; ++j) {
                printf("%c ", all_values[i * NUM_DATOS + j]);
            }
            printf("del fichero.\n");
        }
        printf("\nTiempo de ejecución: %f segundos\n", endTime - startTime);
        system("notepad Practica5.dat");
    }

    MPI_Finalize();
    return 0;
}
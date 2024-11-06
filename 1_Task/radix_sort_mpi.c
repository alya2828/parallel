#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

// Функция подсчета сортировки
void counting_sort(int *array, int size, int exp) {
    int *output = (int *)malloc(size * sizeof(int)); // Временный массив для хранения отсортированных элементов
    int count[10] = {0}; // Массив для подсчета

    // Подсчет частоты появления каждого разряда
    for (int i = 0; i < size; i++) {
        count[(array[i] / exp) % 10]++;
    }

    // Изменение count[i], чтобы каждый элемент содержал позицию этого разряда в output
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // Построение выходного массива
    for (int i = size - 1; i >= 0; i--) {
        output[count[(array[i] / exp) % 10] - 1] = array[i];
        count[(array[i] / exp) % 10]--;
    }

    // Копирование отсортированных элементов обратно в исходный массив
    for (int i = 0; i < size; i++) {
        array[i] = output[i];
    }

    free(output);
}

// Функция поразрядной сортировки
void radix_sort(int *array, int size) {
    // Находим максимальный элемент
    int max = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }

    // Применяем сортировку поразрядной сортировки
    for (int exp = 1; max / exp > 0; exp *= 10) {
        counting_sort(array, size, exp);
    }
}

// Функция для параллельной поразрядной сортировки
void parallel_radix_sort(int *array, int size) {
    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    // Распределение размера подмассива
    int local_size = size / num_processes;
    // Обработка остатка
    if (rank < size % num_processes) {
        local_size++;
    }

    // Выделение памяти для подмассива
    int *sub_array = (int *)malloc(local_size * sizeof(int));

    // Рассылка подмассива каждому процессу
    MPI_Scatter(array, local_size, MPI_INT, sub_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Поразрядная сортировка для каждого подмассива
    radix_sort(sub_array, local_size);

    // Сборка отсортированных подмассивов обратно в основной массив
    MPI_Gather(sub_array, local_size, MPI_INT, array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    free(sub_array);
}

// Функция для чтения массива из файла
int read_array_from_file(const char *filename, int **array) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        return -1;
    }
    int size = 0;
    // Считаем количество элементов в файле
    while (!feof(file)) {
        int temp;
        if (fscanf(file, "%d", &temp) == 1) {
            size++;
        }
    }
    rewind(file); // Возвращаемся в начало файла

    *array = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        fscanf(file, "%d", &(*array)[i]);
    }

    fclose(file);
    return size;
}

// Функция для записи отсортированного массива в файл
void write_array_to_file(const char *filename, int *array, int size) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Unable to open file for writing");
        return;
    }

    for (int i = 0; i < size; i++) {
        fprintf(file, "%d\n", array[i]);
    }

    fclose(file);
}

// Функция для сравнения двух массивов
int compare_arrays(int *array1, int *array2, int size) {
    for (int i = 0; i < size; i++) {
        if (array1[i] != array2[i]) {
            return 0; // Массивы не совпадают
        }
    }
    return 1; // Массивы совпадают
}

int main(int argc, char **argv) {
    int rank, num_processes;
    int *array = NULL;
    int n;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (rank == 0) {
        // Проверка на наличие аргументов командной строки
        if (argc < 2) {
            fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        // Чтение массива из файла
        n = read_array_from_file(argv[1], &array);
        if (n < 0) {
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
    }

    // Распространение размера массива на все процессы
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Добавлено: вывод размера массива для каждого процесса
    printf("Process %d: n = %d\n", rank, n);

    // Запуск последовательной сортировки на процессе 0
    int *sequential_array = NULL;
    if (rank == 0) {
        sequential_array = (int *)malloc(n * sizeof(int));
        memcpy(sequential_array, array, n * sizeof(int)); // Копируем исходный массив

        double sequential_start_time = MPI_Wtime();
        radix_sort(sequential_array, n);
        double sequential_end_time = MPI_Wtime();

        // Запись результата последовательной сортировки в файл
        write_array_to_file("sorted_seq.txt", sequential_array, n);

        printf("Sequential sort time: %f seconds\n", sequential_end_time - sequential_start_time);
    }

    // Распределение размера подмассива
    int local_size = n / num_processes;
    // Обработка остатка
    if (rank < n % num_processes) {
        local_size++;
    }

    // Выделение памяти для подмассива
    int *sub_array = (int *)malloc(local_size * sizeof(int));

    // Рассылка подмассива каждому процессу
    MPI_Scatter(array, local_size, MPI_INT, sub_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Поразрядная сортировка для каждого подмассива
    double parallel_start_time = MPI_Wtime();
    radix_sort(sub_array, local_size);
    double parallel_end_time = MPI_Wtime();

    // Сборка отсортированных подмассивов обратно в основной массив
    int *gathered_array = NULL;
    if (rank == 0) {
        gathered_array = (int *)malloc(n * sizeof(int));
    }

    MPI_Gather(sub_array, local_size, MPI_INT, gathered_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // После получения всех подмассивов, сортируем весь массив
        // Это необходимо для корректного сравнения с последовательной сортировкой
        radix_sort(gathered_array, n);

        // Запись результата параллельной сортировки в файл
        write_array_to_file("sorted_par.txt", gathered_array, n);

        printf("Parallel sort time: %f seconds\n", parallel_end_time - parallel_start_time);

        // Сравнение результатов
        if (compare_arrays(sequential_array, gathered_array, n)) {
            printf("Results match!\n");
        } else {
            printf("Results do not match!\n");
        }

        free(sequential_array);
        free(array);
        free(gathered_array);
    }

    free(sub_array);
    MPI_Finalize();
    return 0;
}

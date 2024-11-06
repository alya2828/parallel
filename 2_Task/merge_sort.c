#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
void merge_sort(int *arr, int left, int right);
void merge(int *arr, int left, int mid, int right) {
    // Размеры двух подмассивов, которые нужно объединить
    int n1 = mid - left + 1;
    int n2 = right - mid;
    // Создание временных массивов для левой и правой половины
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));
 // Копирование данных в временные массивы
    for (int i = 0; i < n1; i++)
     L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) 
    R[j] = arr[mid + 1 + j];

    // Слияние двух временных массивов обратно в основной массив arr
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    // Копирование оставшихся элементов из L (если есть)
    while (i < n1) arr[k++] = L[i++];
     // Копирование оставшихся элементов из R (если есть)
    while (j < n2) arr[k++] = R[j++];
    free(L);
    free(R);
}
// Пораллельная сортировка слиянием
void merge_sort_parallel(int *arr, int left, int right, int depth) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        if (depth > 0) {
            #pragma omp parallel sections
            {
                #pragma omp section
                merge_sort_parallel(arr, left, mid, depth - 1);
                #pragma omp section
                merge_sort_parallel(arr, mid + 1, right, depth - 1);
            }
        } else {
            merge_sort(arr, left, mid);
            merge_sort(arr, mid + 1, right);
        }
        merge(arr, left, mid, right);
    }
}

// Последовательная сортировка слиянием
void merge_sort(int *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}
// Количество чисел в файле
int count_elements_in_file(FILE *file) {
    int count = 0;
    int temp;
    while (fscanf(file, "%d", &temp) == 1) {
        count++;
    }
    rewind(file); // Возвращаем указатель в начало файла
    return count;
}

void write_to_file(const char *filename, int *array, int size) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }
    for (int i = 0; i < size; i++) {
        fprintf(file, "%d\n", array[i]);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <file> <num_threads>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    int num_threads = atoi(argv[2]);

    omp_set_num_threads(num_threads);

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return 1;
    }

    int size = count_elements_in_file(file);
    int *array_seq = (int *)malloc(size * sizeof(int));
    int *array_par = (int *)malloc(size * sizeof(int));

    // Чтение данных из файла
    for (int i = 0; i < size; i++) {
        fscanf(file, "%d", &array_seq[i]);
        array_par[i] = array_seq[i];
    }
    fclose(file);

    // Последовательная сортировка 
    double start_time = omp_get_wtime();
    merge_sort(array_seq, 0, size - 1);
    double end_time = omp_get_wtime();
    double seq_time = end_time - start_time;
    printf("Sequential Selection Sort Time: %f seconds\n", seq_time);

    // Параллельная сортировка 
    start_time = omp_get_wtime();
    merge_sort_parallel(array_par, 0, size - 1, (num_threads));
    end_time = omp_get_wtime();
    double par_time = end_time - start_time;
    printf("Parallel Merge Sort Time: %f seconds\n", par_time);

    // Запись отсортированных данных в файлы
    write_to_file("sorted_seq.txt", array_seq, size);
    write_to_file("sorted_par.txt", array_par, size);

    // Сравнение результатов
    int is_equal = 1;
    for (int i = 0; i < size; i++) {
        if (array_seq[i] != array_par[i]) {
            is_equal = 0;
            break;
        }
    }
    if (is_equal) {
        printf("Results match!\n");
    } else {
        printf("Results do not match!\n");
    }

    // Освобождение памяти
    free(array_seq);
    free(array_par);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_random_numbers(const char *filename, int count)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }
    srand(time(NULL));
    for (int i = 0; i < count; i++)
    {
        int num = rand() % 1000000; // Случайные числа от 0 до 999
        fprintf(file, "%d\n", num);
    }

    fclose(file);
    printf("Successfully generated %d random numbers in %s\n", count, filename);
}

int main()
{
    const char *filename = "input.txt";
    int count = 1000000;

    generate_random_numbers(filename, count);

    return 0;
}

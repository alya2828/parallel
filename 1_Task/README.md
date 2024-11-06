# Задание:  Реализовать поразрядную сортировку методом параллельного программирования с использованием технологии MPI и библиотеки профилирования MPE.

## Как использовать
1. **Компиляция программы:**
- make
2. **Для тестирования можно запустить прогамму для создания файла с рандомными числами и изменить количество**
- gcc -o generator 
- ./generator
3. **Для тестиования программы при коммпиляции программы можно указать количество процессов**
- mpirun -np 2 ./radix_sort_mpi input.txt // где 2 - количество процессов,в данном примере можно указать 2 и 4 процесса 
4. **Проверьте установку MPI**
- sudo apt update
- sudo apt install mpich
5. **В случае если программа не видит библиотеку**
- обавьте путь к заголовочным файлам MPI в настройках includePath. В файле c_cpp_properties.json добавьте путь, где находится mpi.h
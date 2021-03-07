# Требования:

- Код должен быть написан на языке C и собираться под Linux.
- Исходники должны быть выложены на github.

# Задание:

1. Сформировать в памяти двусвязный список определенного размера, содержащий случайные значения, которые следует получить из генератора случайных чисел.

2. Запустить два потока (threads).

3. Потоку №1: начиная с головы списка подсчитывать количество нулевых битов в значениях элементов и количество пройденных элементов, освобождать учтённый элемент сразу после учёта.

4. Потоку №2: начиная с хвоста списка подсчитывать количество единичных битов в значениях элементов и количество пройденных элементов, освобождать учтённый элемент сразу после учёта.

5. По окончании элементов списка вывести результаты подсчёта.

6. Обеспечить однократную обработку каждого элемента списка (каждый должен быть учтён, но только один раз, одним из потоков).

7. Желательно, чтобы в потоках работали не две разные функции, а одна, принимающая особенности работы через аргументы.
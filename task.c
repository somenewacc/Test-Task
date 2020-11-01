// S-Terra test task
// Author: Bezborodov G.A.

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

// Количество элементов в списке
#define N 1000

// Создаём булевые переменные для удобства
typedef enum {false, true} bool;

// Создаём структуру для двусвязного списка
struct Node { 
    int data; // Информация в узле
    struct Node* next; // Указатель на следующий узел списка
    struct Node* prev; // Указатель на предыдущий узел списка
};

// Инициализируем списки
struct Node* head = NULL;
struct Node* tail = NULL;

// Структура для проверки какой поток работает
typedef struct{
	bool check;
} pthrData;

// Инициализируем глобальные переменные
int counthead = 0, counttail = N;
bool headend = false, tailend = false;
int sumzeros = 0, sumones = 0;

// Функция потока
void* threadFunc(void* thread_data) 
{
    pthrData *data = (pthrData*) thread_data;
    // Проверка потока
    if (data->check == true)
    {
        int zeros = 0;
        struct Node *last;
        while (head != NULL && headend != true)
        {
            usleep(1);
            if ((counthead >= counttail) && (counthead != 0))
            {
                headend = true;
                tailend = true;
            }
            else
            {
                zeros = 0;
                int datatmp = head->data;
                // Подсчёт нулей в узле списка
                while (datatmp > 0)
                {
                    if (~(datatmp) & 1)
                        zeros += 1;
                    datatmp = datatmp >> 1;
                }
                // Переход к следующему узлу
                last = head;
                head = head->next;
                if (headend != true && tailend != true)
                {
                    sumzeros  += zeros;
                    counthead  = counthead + 1;
                }
            }
        }
        if (counttail == 0 && counthead > 0)
        {
            counthead = 0;
            sumzeros  = 0;
        }
    }
    else if (data->check == false)
    {
        int ones = 0;
        struct Node *lasttail;
        while (tail != NULL && tailend != true)
        {
            usleep(1);
            if ((counthead >= counttail) && (counttail != 0))
            {
                headend = true;
                tailend = true;
            }
            else
            {
                ones = 0;
                int datatmptail = tail->data;
                // Подсчёт единиц в узле списка
                while (datatmptail > 0)
                {
                    if (datatmptail & 1)
                        ones += 1;
                    datatmptail = datatmptail >> 1;
                }
                // Переход к следующему узлу
                lasttail = tail;
                tail     = tail->next;
                if (headend != true && tailend != true)
                {
                    sumones   += ones;
                    counttail  = counttail - 1;
                }   
            }
        }
        if (counthead == N && counttail < N)
        {
            counttail  = 0;
            sumones    = 0;
        }
    }
    pthread_exit(0);
}

// Функция для добавление узла в начало списка
void push(struct Node** head_ref, int new_data)
{
    // Создаём новый узел
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
 
    // Записываем в новый узел данные
    new_node->data  = new_data;
 
    // Делаем следующий узел после текущего головным, предыдущий NULL
    new_node->next = (*head_ref);
    new_node->prev = NULL;
 
    // Меняем предыдущий узел головного новым
    if((*head_ref) !=  NULL)
      (*head_ref)->prev = new_node ;
 
    // Делаем новый узел головным
    (*head_ref)    = new_node;
}

// Функция для добавления узла в начало списка
void append(struct Node** head_ref, int new_data)
{
    // Создаём новый узел
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
 
    struct Node *last = *head_ref;
 
    // Записываем в новый узел данные
    new_node->data  = new_data;
 
    // Делаем следующий узел после нового NULL
    new_node->next = NULL;
 
    // Если список пуст, делаем новый узел как головной
    if (*head_ref == NULL)
    {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }
 
    // Иначе проходим список до последнего узла
    while (last->next != NULL)
        last = last->next;
 
    // Делаем новый узел следующим после последнего
    last->next = new_node;
 
    // Делаем последний узел предыдущим после нового
    new_node->prev = last;
 
    return;
}

// Функция для вывода списка на экран
void printList(struct Node *node)
{
    struct Node *last;
    printf("\nDoubly linked list: \n");
    while (node != NULL)
    {
        printf(" %d ", node->data);
        last = node;
        node = node->next;
    }
    printf("\n");
}

// Функция для реверсивного копирования списка
void copyrevnode(struct Node *node)
{
    struct Node *last;
    while (node != NULL)
    {
        push(&tail, node->data);
        last = node;
        node = node->next;
    }
}

int main()
{
    pthrData threadDataHead;
    threadDataHead.check = true;

    pthrData threadDataTail;
    threadDataTail.check = false;

    // Поток для прохода с начала списка
    pthread_t threadhead;

    // Поток для прохода с конца списка
    pthread_t threadtail;

    srand(time(NULL));
    
    // Заполняем список случайными значениями
    for (int i = 0; i < N; i++)
    {
        append(&head, rand() % 100 + 1);
    }

    
    // Вызов функции печати списка в консоль
    // printList(head);
    

    // Реверсивно копируем список. Это необходимо для того, чтобы в потоках не было лишнего перекоса
    copyrevnode(head);
    
    // Запускаем потоки
    pthread_create(&threadhead, NULL, threadFunc, &threadDataHead);
    pthread_create(&threadtail, NULL, threadFunc, &threadDataTail);

    // Конец потоков
    pthread_join(threadhead, NULL);
    pthread_join(threadtail, NULL);

    // Вывод подсчётов на экран
    printf("Zeros from head: %d\nNumber of elements from head: %d\n", sumzeros, counthead);
    printf("Ones from tail: %d\nNumber of elements from tail: %d\n", sumones, N - counttail);

    return 0;
}
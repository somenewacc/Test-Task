// Test Task
// Author: Bezborodov G.A.

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

// Количество элементов в списке
#define N 10000

// Создаём булевые переменные для удобства
typedef enum {false, true} bool;

// Создаём структуру для двусвязного списка
struct Node { 
    int data;          // Информация в узле
    struct Node* next; // Указатель на следующий узел списка
    struct Node* prev; // Указатель на предыдущий узел списка
};

pthread_mutex_t mutex;

// Инициализируем списки
struct Node* head = NULL;
struct Node* tail = NULL;

// Структура для проверки какой поток работает
typedef struct{
	bool check;
} pthrData;

// Инициализируем глобальные переменные
int counthead = 0, counttail = N;
bool thread_end = false;
int zeros_sum = 0, ones_sum = 0;

// Функция потока
void* threadFunc(void* thread_data) 
{
    pthrData *data = (pthrData*) thread_data;
    int bits = 0;
    struct Node *last;
    struct Node *lasttail;
    while (!thread_end)
    {
        pthread_mutex_lock(&mutex);
        if (counthead >= counttail)
        {
            thread_end = true;
        }
        else
        {
            bits = 0;
            int datatmp = data->check ? head->data : tail->data;
            while (datatmp > 0)
            {
                if ((~(datatmp) & 1) && data->check)
                    bits += 1;
                else if ((datatmp & 1) && !data->check)
                    bits += 1;
                datatmp = datatmp >> 1;
            }
            if (counthead < counttail)
            {
                last = data->check ? head : tail;
                if (data->check){
                    zeros_sum += bits;
                    head = head->next;
                    counthead  = counthead + 1;
                }
                else{
                    ones_sum += bits;
                    tail = tail->prev;
                    counttail  = counttail - 1;
                }
                free(last);
            }
            else
            {
                thread_end = true;
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    head = NULL;
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

    tail = head;
    for (int i = 0; i < N-1; i++)
    {
        tail = tail->next;
    }

    pthread_mutex_init (&mutex, NULL);
    
    // Запускаем потоки
    pthread_create(&threadhead, NULL, threadFunc, &threadDataHead);
    pthread_create(&threadtail, NULL, threadFunc, &threadDataTail);

    // Конец потоков
    pthread_join(threadhead, NULL);
    pthread_join(threadtail, NULL);

    // Вывод подсчётов на экран
    printf("Zeros from head: %d\nNumber of elements from head: %d\n", zeros_sum, counthead);
    printf("Ones from tail: %d\nNumber of elements from tail: %d\n", ones_sum, N - counttail);

    return 0;
}

#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int size = 0;
pthread_mutex_t mutex_size = PTHREAD_MUTEX_INITIALIZER;

int clamb_index(int index)
{
    return (index < 0) ? 0 : (index >= size) ? size - 1
                                             : index;
}
void increase_size()
{
    pthread_mutex_lock(&mutex_size);
    size++;
    pthread_mutex_unlock(&mutex_size);
}

void decrease_size()
{
    pthread_mutex_lock(&mutex_size);
    size--;
    pthread_mutex_unlock(&mutex_size);
}

void print_list(int_ll_t *list)
{
    int_ll_t *node = list;
    for (int i = 0; i < size + 1; i++)
    {
        printf("%d ", node->value);
        node = node->next;
    }
    printf("\n");
}

int_ll_t *make_node(int value, int_ll_t *next)
{
    int_ll_t *node = (int_ll_t *)malloc(sizeof(int_ll_t));
    node->value = value;
    node->next = next;
    pthread_mutex_init(&node->mutex, NULL);
    return node;
}

// Init list structure
int init_list(int_ll_t *list)
{
    list = make_node(1, NULL);
    return 0;
}

// Free list structure
int free_list(int_ll_t *list)
{
    pthread_mutex_lock(&list->mutex);
    // printf("Liberando la lista con tamanno %d\n", size);
    // print_list(list);

    int_ll_t *node = list;
    while (node != NULL)
    {
        int_ll_t *next = node->next;
        pthread_mutex_destroy(&node->mutex);
        free(node);
        node = next;
    }
    size = 0;
    pthread_mutex_unlock(&list->mutex);
    // exit(0);
    return 0;
}

// Get list size
int size_list(int_ll_t *list)
{
    return size;
}

// Get element at index
int index_list(int_ll_t *list, int index, int *out_value)
{
    pthread_mutex_lock(&list->mutex);

    // printf("Getting el indice %d, tamanno de la lista %d\n", index, size);
    // print_list(list);

    if (size == 0)
        *out_value = 0;

    else
    {
        int_ll_t *node = list->next;
        for (int i = 0; i < clamb_index(index); i++)
            node = node->next;

        *out_value = node->value;
    }
    // printf("Obtenido %d\n", *out_value);
    pthread_mutex_unlock(&list->mutex);
    return 0;
}

// Insert element at index
int insert_list(int_ll_t *list, int index, int value)
{

    pthread_mutex_lock(&list->mutex);
    // printf("Insertando %d en el indice %d, tamanno de la lista %d\n", value, index, size);
    // print_list(list);
    int_ll_t *node = list;

    for (int i = 0; i < clamb_index(index); i++)
        node = node->next;

    node->next = make_node(value, node->next);
    node->next->value = value;
    increase_size();
    // print_list(list);
    // printf("Insertado %d en el indice %d, tamanno de la lista %d\n", value, index, size);

    pthread_mutex_unlock(&list->mutex);
    return 0;
}

// Remove element at index
int remove_list(int_ll_t *list, int index, int *out_value)
{
    pthread_mutex_lock(&list->mutex);

    // printf("Removiendo el indice %d, tamanno de la lista %d\n", index, size);
    // print_list(list);

    if (size <= 0)
        *out_value = 1;

    else
    {
        int_ll_t *node = list;
        for (int i = 0; i < clamb_index(index); i++)
            node = node->next;

        *out_value = node->next->value;
        int_ll_t *temp = node->next;
        node->next = node->next->next;

        pthread_mutex_destroy(&temp->mutex);
        free(temp);

        decrease_size();
    }

    pthread_mutex_unlock(&list->mutex);
    return 0;
}

#include "list.h"
#include <stdlib.h>
#include <stdio.h>

// Lock
static inline void lock_init(volatile ptlock_t *l)
{
    *l = (int)0;
}

static inline void lock_destroy(volatile ptlock_t *l)
{
    /* do nothing */
}

static inline int lock_lock(volatile ptlock_t *l)
{
    while (*l)
        ;
    *l = (int)1;
    return 0;
}

static inline int lock_unlock(volatile ptlock_t *l)
{
    *l = (int)0;
    return 0;
}

#define INIT(l) lock_init(l)
#define DESTROY(l) lock_destroy(l)
#define LOCK(l) lock_lock(l)
#define UNLOCK(l) lock_unlock(l)
//

int size = 0;
ptlock_t mutex_size = 0;

int clamb_index(int index)
{
    return (index < 0) ? 0 : (index >= size) ? size - 1
                                             : index;
}
void increase_size()
{
    LOCK(&mutex_size);
    size++;
    UNLOCK(&mutex_size);
}

void decrease_size()
{
    LOCK(&mutex_size);
    size--;
    UNLOCK(&mutex_size);
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
    node->mutex = (ptlock_t *)malloc(sizeof(ptlock_t));
    INIT(node->mutex);
    return node;
}

// Init list structure
int init_list(int_ll_t *list)
{
    size = 0;
    INIT(&mutex_size);
    list = make_node(1, NULL);
    return 0;
}

// Free list structure
int free_list(int_ll_t *list)
{
    LOCK(list->mutex);
    // printf("Liberando la lista con tamanno %d\n", size);
    // print_list(list);

    int_ll_t *node = list;
    while (node != NULL)
    {
        int_ll_t *next = node->next;
        DESTROY(node->mutex);
        free(node->mutex);
        free(node);
        node = next;
    }
    size = 0;
    UNLOCK(list->mutex);
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
    LOCK(list->mutex);
    if (size == 0)
        *out_value = 0;
    else
    {
        int_ll_t *node = list->next;
        for (int i = 0; i < clamb_index(index); i++)
            node = node->next;
        *out_value = node->value;
    }
    UNLOCK(list->mutex);
    return 0;
}

// Insert element at index
int insert_list(int_ll_t *list, int index, int value)
{

    LOCK(list->mutex);
    int_ll_t *node = list;
    for (int i = 0; i < clamb_index(index); i++)
        node = node->next;
    node->next = make_node(value, node->next);
    node->next->value = value;
    increase_size();
    UNLOCK(list->mutex);
    return 0;
}

// Remove element at index
int remove_list(int_ll_t *list, int index, int *out_value)
{
    LOCK(list->mutex);
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

        DESTROY(temp->mutex);
        free(temp->mutex);
        free(temp);

        decrease_size();
    }

    UNLOCK(list->mutex);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// Talvez precise mudar o tipo da lista

// Node ============================================
typedef struct node {
    int value;
    struct node *next;
} Node;

Node *create_node(int value, Node *next) {
    Node *n = (Node *) malloc(sizeof(Node));
    n->value = value; n->next = next;
    return n;
}

Node *create_node_head(Node *next) {
    Node *n = (Node *) malloc(sizeof(Node));
    n->next = next;
    return n;
}
// =================================================

// List ============================================
typedef struct {
    Node *head;
    Node *tail;
    Node *curr;
    int size;
} List;

List *create_list() {
    List *l =(List *) malloc(sizeof(List));
    l->curr = create_node_head(NULL);
    l->tail = l->curr;
    l->head = l->curr;
    l->size = 0;
    return l;
}

void insert(List *l, int value) {
    l->curr->next = create_node(value, l->curr->next);
    if (l->tail == l->curr) l->tail = l->curr->next;
    //l->curr = l->curr->next;
    l->size++;
}

void append(List *l, int value) {
    l->tail->next = create_node(value, l->tail->next);
    l->tail = l->tail->next;
    l->size++;
}

int remove_curr(List *l) {
    if (l->curr->next == NULL) {
        return -1;
    }
    int value = l->curr->next->value;
    Node *removed = l->curr->next;
    if (l->tail == l->curr->next) l->tail = l->curr;
    l->curr->next = l->curr->next->next;
    l->size--;
    free(removed);
    return value;
}

int remove_first(List *l) { // nao altera o cursor
    Node *backup = l->curr;
    l->curr = l->head;
    int value = remove_curr(l);
    l->curr = backup;
    return value;
}

void clear(List *l) {
    Node *temp = l->head->next;
    while (temp != NULL) {
        Node *removed = temp;
        temp = temp->next;
        free(removed);
    }
    l->head->next = NULL;
    l->size = 0;
}

void destroy(List *l) {
    clear(l);
    free(l);
}

void print_rec(Node *node) {
    if (node != NULL) {
        printf("%d ", node->value);
        print_rec(node->next);
    }
}

void print(List *l) {
    Node *temp = l->head->next;
    print_rec(temp);
    printf("\n");
}

// =================================================

int main() {

    List *l = create_list();
    append(l, 1);
    append(l, 2);
    append(l, 3);
    //remove_first(l);
    //remove_first(l);
    //remove_first(l);
    //remove_first(l);
    print(l);
    destroy(l);

    return 0;
}

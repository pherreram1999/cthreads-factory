//
// Created by sistemas on 28/09/24.
//

#ifndef TYPES_H
#define TYPES_H

typedef struct nodo {
    int id;
    struct nodo *next;
    struct nodo *prev;
    int len;
} listnode;

#endif //TYPES_H



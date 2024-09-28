#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "types.h"

#define MAX_INJECTIONS 1000

// definimos mutex
pthread_mutex_t mutexOne;

// variables globales que se compartes


static int inyeccionesLen = 0;
listnode *cintaInyecciones = nullptr;
listnode *cintaEmbalajes = nullptr;


listnode *nuevaLista() {
    return (listnode *)malloc(sizeof(listnode));
}

/**
 * Retorna el ultimo nodo de la lista
 * @param lista
 * @return
 */
listnode *getLastNode(listnode **lista) {
    listnode *nav = *lista;
    while (nav != nullptr) {
        nav = nav->next;
    }
    return nav;
}

/**
 * Inserta un nuevo id, creando su nodo en la lista
 * @param lista
 * @param idinyeccion
 */
void insertIntoList(listnode **lista,int idinyeccion) {
    if (*lista == nullptr) { // primer nodo
        listnode *nuevaInyeccion = nuevaLista();
        nuevaInyeccion->id = idinyeccion;
        *lista = nuevaInyeccion;
        return;
    }

    listnode *nav = *lista;
    while(nav->next != nullptr) {
        nav = nav->next; // nos colocamos en el ultimo nodo
    }
    listnode *nuevaInyeccion = nuevaLista();
    nuevaInyeccion->id = idinyeccion;
    nuevaInyeccion->prev = nav;
    nav->next = nuevaInyeccion;
}


void appendNodeToList(listnode **lista,listnode *node) {
    if (*lista == nullptr) {
        // primer elemento de la lista
        *lista = node;
        return;
    }
    listnode *lastNode = getLastNode(lista);
    lastNode->next = node;
    node->next = nullptr;
}

/**
 * remueve un nodo de la lista y regresa la direccion del nodo removido
 * @param lista
 * @param id
 * @return nodo removido de la lista
 */
listnode* removeNodeFromList(listnode **lista, int id) {
    if (*lista == nullptr) {
        return nullptr; // no esta en la lista
    }

    listnode *nav = *lista;
    // si el nodo queremos es el primero
    if(nav->id == id) {
        // recorremos la lista
        *lista = nav->next; // este movimiento puede dejar nulo la lista

        if(*lista == nullptr) {
            // si la lista actual es no null,
            // ajustamos el prev
            (*lista)->prev = nullptr; // ya no apunta nada
        }

        // se libera las relaciones del nodo encontrado
        nav->next = nullptr;
        nav->prev = nullptr;
        return nav; // regresamos
    }

    // recorremos hasta encontrarlo

    while(nav != nullptr && nav->id != id) {
        nav = nav->next;
    }

    // si despues de recorrerlo es nulo, entonces no existe
    if(nav == nullptr) {
        return nullptr;
    }

    // recorremos el puntero de prev

    nav->prev->next = nav->next;

    // si tenemos next recorremos su prev
    if(nav->next != nullptr) {
        nav->next->prev = nav->prev;
    }

    // liberamos el nodo
    nav->next = nullptr;
    nav->prev = nullptr;

    return nav;
}


/**
 * llena la lista con ids de las "inyecciones"
 * @param arg
 * @return
 */
void *llenadoCinta(void *arg) {
    // recibimos el apuntador del apuntador de la lista
    int counter = 1;
    while(true) {
        pthread_mutex_lock(&mutexOne);
        insertIntoList(&cintaInyecciones,counter);
        pthread_mutex_unlock(&mutexOne);
        counter++;
    }
    /*listnode *nav = &(*data);

    */

}

void verLista(listnode **lista) {
    listnode *nav = *lista;
    while(nav != nullptr) {
        printf("inyeccion id: %d\n",nav->id);
        nav = nav->next;
    }
}

/**
 * Saca valores de la cinta de inyecciones y lo coloca en la de embalaje
 * @param arg
 * @return
 */
void embalar() {
    if(cintaInyecciones == nullptr) {
        return; // sin nodos por mover
    }
    // siempre obtenemos el primer elemento de lista
    listnode * removedNode = removeNodeFromList(&cintaInyecciones,(cintaInyecciones)->id);

    // ese elemento loa agregamos a la lista de embalajes
    appendNodeToList(&cintaEmbalajes,removedNode);
}


// ** se mantiene correidno sacando valores de la lista
void *moveToEmbalajes(void *arg) {
    while(true) {
        pthread_mutex_lock(&mutexOne);
        embalar();
        pthread_mutex_lock(&mutexOne);
    }
}





int main(void) {
    // declaramos los identificadores de los threads
    pthread_t cintaTransportadoraThread, embalajesThread;
    // inciamos los procesos en paralelo
    pthread_create(&cintaTransportadoraThread,nullptr,llenadoCinta,nullptr);
    pthread_create(&embalajesThread,nullptr,moveToEmbalajes,nullptr);
    // esperamos los resultados
    pthread_join(cintaTransportadoraThread,nullptr);
    pthread_join(embalajesThread,nullptr);


    /*removeNodeFromList(&cintaInyecciones,3);
    removeNodeFromList(&cintaInyecciones,4);*/


    //embalar(nullptr);
    // en otro proceso mostramos un cotador en otro thead

    return 0;
}

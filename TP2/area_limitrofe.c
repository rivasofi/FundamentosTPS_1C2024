#include "feria.h"
#include "area_limitrofe.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_FILAS 20
#define MAX_COLUMNAS 20

const int TIMER_BOMBA_EXPLOTA = 0;
const int DESACTIVAR_BOMBA = 10;

const int CERCA_ROBOT = 2;
const int CERCA_FAMILIAR = 1;

const int CONSUMO_GOLOSINA = 20;

const int MOVIMIENTOS_APARICION_ROBOT = 10;
const int DESTRUCCION_ROBOT = 5;

#define ARRIBA 'W'
#define IZQUIERDA 'A'
#define ABAJO 'S'
#define DERECHA 'D'
#define CAMUFLARSE 'Q'

/*Pre: Las coordenadas proporcionadas deben ser válidas.*/

/*Pos: Devuelve la distancia manhattan entre las coordenadas pasadas.*/

int distancia_manhattan(coordenada_t posicion1, coordenada_t posicion2)
{
    int distancia = abs(posicion1.fil - posicion2.fil) + abs(posicion1.col - posicion2.col);

    return distancia;
}

/*Pre: La coordenada proporcionada debe ser valida, el parámetro juego debe ser una estructura con sus valores ya inicializados y el puntero "coordenada_existente" debe ser válido. */

/*Pos: Si la coordenada pasada ya existe (y en este caso está ocupada por una bomba), actualiza el valor de coordenada_existente a true.*/

void chequear_bombas(coordenada_t coordenada, juego_t juego, int i, bool *coordenada_existente)
{
    i = 0;

    while (i < juego.tope_bombas && !(*coordenada_existente))
    {
        if (coordenada.fil == juego.bombas[i].posicion.fil && coordenada.col == juego.bombas[i].posicion.col)
        {
            *coordenada_existente = true;
        }
        i++;
    }
}

/*Pre: La coordenada proporcionada debe ser valida, el parámetro juego debe ser una estructura con sus valores ya inicializados y el puntero "coordenada_existente" debe ser válido. */

/*Pos: Si la coordenada pasada ya existe (y en este caso está ocupada por una herramienta), actualiza el valor de coordenada_existente a true.*/

void chequear_herramientas(coordenada_t coordenada, juego_t juego, int i, bool *coordenada_existente)
{
    i = 0;

    while (i < juego.tope_herramientas && !(*coordenada_existente))
    {
        if (coordenada.fil == juego.herramientas[i].posicion.fil && coordenada.col == juego.herramientas[i].posicion.col)
        {
            *coordenada_existente = true;
        }
        i++;
    }
}

/*Pre: La coordenada proporcionada debe ser valida, el parámetro juego debe ser una estructura con sus valores ya inicializados y el puntero "coordenada_existente" debe ser válido. */

/*Pos: Si la coordenada pasada ya existe (y en este caso está ocupada por un familiar), actualiza el valor de coordenada_existente a true.*/

void chequear_familiares(coordenada_t coordenada, juego_t juego, int i, bool *coordenada_existente)
{
    i = 0;

    while (i < juego.tope_familiares && !(*coordenada_existente))
    {
        if (coordenada.fil == juego.familiares[i].posicion.fil && coordenada.col == juego.familiares[i].posicion.col)
        {
            *coordenada_existente = true;
        }
        i++;
    }
}

/*Pre: El parámetro "coordenada" debe estar dentro de los límites establecidos del juego.*/

/*Post: Retorna "true" si la coordenada pasada ya existe en alguna de las posiciones ya ocupadas. Sino, retorna false.*/

bool coordenada_existe(coordenada_t coordenada, juego_t juego)
{
    bool coordenada_existente = false;
    int i = 0;

    if (coordenada.fil == juego.perry.posicion.fil && coordenada.col == juego.perry.posicion.col)
    {
        coordenada_existente = true;
    }

    chequear_bombas(coordenada, juego, i, &coordenada_existente);

    chequear_herramientas(coordenada, juego, i, &coordenada_existente);

    chequear_familiares(coordenada, juego, i, &coordenada_existente);

    return coordenada_existente;
}

/*Pre: "Perry" debe tener su posicion debidamente actualizada, el puntero "robots" debe ser válido, la cantidad de estos debe estar actualizada.*/

/*Pos: Devuelve un booleano que representa si Perry está a una distancia que le permite destruir el robot (o perder una vida si esto no es posible).*/

bool cercano_a_robot(personaje_t perry, coordenada_t *robots, int cantidad_robots, int *indice_cercano)
{
    bool cercano = false;
    int i = 0;

    while (i < cantidad_robots && !cercano)
    {
        if (distancia_manhattan(perry.posicion, robots[i]) <= CERCA_ROBOT)
        {
            cercano = true;
            *indice_cercano = i;
        }
        i++;
    }

    return cercano;
}

/*Pre: "Perry" debe tener sus valores debidamente actualizados, y el array "bombas" debe ser válido y haber sido inicializado.*/

/*Pos: Devuelve un booleano que representa si Perry cumple las condiciones necesarias para desactivar una bomba.*/

bool puede_desactivar_bomba(bomba_t bombas[MAX_BOMBAS], personaje_t *perry, int i)
{
    return (bombas[i].posicion.fil == perry->posicion.fil &&
            bombas[i].posicion.col == perry->posicion.col &&
            !bombas[i].desactivada &&
            !perry->camuflado &&
            perry->energia >= DESACTIVAR_BOMBA);
}

/*Pre: El array "bombas" debe ser un array válido, inicializado y debidamente actualizado.*/

/*Pos: Devuelve un booleano que representa si la bomba "explotó" en base a una serie de condiciones.*/

bool bomba_explota(bomba_t bombas[MAX_BOMBAS], int i)
{
    return (bombas[i].timer == TIMER_BOMBA_EXPLOTA && !bombas[i].desactivada);
}

/*Pre: "juego" debe ser válido y estar inicializado y debidamente actualizado.*/

/*Pos: Devuelve un booleano que representa si Perry está "muy cerca" de un familiar.*/

bool cercania_peligrosa(juego_t juego, int indice_familiar)
{
    return (distancia_manhattan(juego.perry.posicion, juego.familiares[indice_familiar].posicion) <= CERCA_FAMILIAR);
}

/*Pre: El puntero "familiar" debe ser un puntero válido a una estructura de tipo familiar_t.*/

/*Pos: Actualiza la posición del familiar en el juego de acuerdo a su dirección actual en sentido vertical.*/

void mover_familiar_vertical(familiar_t *familiar)
{
    if (familiar->sentido == ARRIBA)
    {
        if (familiar->posicion.fil > 0)
        {
            familiar->posicion.fil--;
        }
        else
        {
            familiar->sentido = ABAJO;
            familiar->posicion.fil++;
        }
    }
    else if (familiar->sentido == ABAJO)
    {
        if (familiar->posicion.fil < MAX_FILAS - 1)
        {
            familiar->posicion.fil++;
        }
        else
        {
            familiar->sentido = ARRIBA;
            familiar->posicion.fil--;
        }
    }
}

/*Pre: El puntero "familiar" debe ser un puntero válido a una estructura de tipo familiar_t.*/

/*Pos: Actualiza la posición del familiar en el juego de acuerdo a su dirección actual en sentido horizontal.*/

void mover_familiar_horizontal(familiar_t *familiar)
{
    if (familiar->sentido == IZQUIERDA)
    {
        if (familiar->posicion.col > 0)
        {
            familiar->posicion.col--;
        }
        else
        {
            familiar->sentido = DERECHA;
            familiar->posicion.col++;
        }
    }
    else if (familiar->sentido == DERECHA)
    {
        if (familiar->posicion.col < MAX_COLUMNAS - 1)
        {
            familiar->posicion.col++;
        }
        else
        {
            familiar->sentido = IZQUIERDA;
            familiar->posicion.col--;
        }
    }
}

/*Pre: El puntero "familiar" debe ser un puntero válido a una estructura de tipo familiar_t.*/

/*Pos: Actualiza la posición del familiar en el juego de acuerdo a su sentido.*/

void mover_familiar(familiar_t *familiar)
{
    switch (familiar->sentido)
    {
    case ARRIBA:
    case ABAJO:
        mover_familiar_vertical(familiar);
        break;
    case IZQUIERDA:
    case DERECHA:
        mover_familiar_horizontal(familiar);
        break;
    }
}

/*Pre: El array de herramientas debe ser válido y ya estar inicializado. El tope de este mismo debe estar actualizado.*/

/*Pos: Elimina la herramienta en la posición "indice" del array "herramientas" y ajusta la cantidad de estas en el array.*/

void eliminar_herramienta_arreglo(herramienta_t herramientas[MAX_HERRAMIENTAS], int *tope_herramientas, int indice)
{
    for (int i = indice; i < *tope_herramientas - 1; i++)
    {
        herramientas[i] = herramientas[i + 1];
    }

    (*tope_herramientas)--;
}

/*Pre: El array de herramientas debe ser válido y ya estar inicializado. El tope de este mismo debe estar actualizado. Perry debe haber sido inicializado y actualizado.*/

/*Pos: Si Perry se encuentra en la misma posición que una golosina en el array de herramientas, se modifica su energía y se elimina la golosina del array.*/

void consumir_golosina(herramienta_t herramientas[MAX_HERRAMIENTAS], personaje_t *perry, int *tope_herramientas)
{
    for (int i = 0; i < *tope_herramientas; i++)
    {
        if (herramientas[i].tipo == 'G' && herramientas[i].posicion.fil == perry->posicion.fil && herramientas[i].posicion.col == perry->posicion.col)
        {
            perry->energia += CONSUMO_GOLOSINA;
            eliminar_herramienta_arreglo(herramientas, tope_herramientas, i);
            i--;
        }
    }
}

/*Pre: El array de herramientas debe ser válido y ya estar inicializado. El tope de este mismo debe estar actualizado. Perry debe haber sido inicializado y actualizado.*/

/*Pos: Si Perry se encuentra en la misma posición que un sombrero en el array de herramientas, se modifica su energía y se elimina el sombrero del array.*/

void consumir_sombrero(herramienta_t herramientas[MAX_HERRAMIENTAS], personaje_t *perry, int *tope_herramientas)
{
    for (int i = 0; i < *tope_herramientas; i++)
    {
        if (herramientas[i].tipo == 'S' && herramientas[i].posicion.fil == perry->posicion.fil && herramientas[i].posicion.col == perry->posicion.col)
        {
            perry->vida++;
            eliminar_herramienta_arreglo(herramientas, tope_herramientas, i);
            i--;
        }
    }
}

/*Pre: El array de robots debe ser válido y ya estar inicializado. El tope de este mismo (cantidad de robots) debe estar actualizado.*/

/*Pos: Elimina el robot en la posición "indice" del array "herramientas" y ajusta la cantidad de estos.*/

void eliminar_robot_arreglo(coordenada_t *robots, int *cantidad_robots, int indice)
{

    for (int i = indice; i < *cantidad_robots - 1; i++)
    {
        robots[i] = robots[i + 1];
    }

    (*cantidad_robots)--;
}

/*Pre: "juego" debe ser válido, estar inicializado y debidamente actualizado.*/

/*Pos: Reasigna "dinámicamente" el tamaño del array de robots en base a la cantidad actualizada de los mismos. Si esto no es posible, sale del juego e imprime por pantalla un mensaje informándolo.*/

void redefinir_arreglo(juego_t *juego)
{
    juego->robots = realloc(juego->robots, sizeof(coordenada_t) * (size_t)juego->cantidad_robots);

    if (juego->robots == NULL && juego->cantidad_robots > 0)
    {
        printf("Error: No se pudo asignar memoria correctamente.\n");
        exit(-1);
    }
}

coordenada_t generar_coordenada_aleatoria_unica(juego_t *juego)
{
    coordenada_t posicion_aleatoria = {-1, -1};

    do
    {
        posicion_aleatoria.fil = rand() % MAX_FILAS;
        posicion_aleatoria.col = rand() % MAX_COLUMNAS;
    } while (coordenada_existe(posicion_aleatoria, *juego));

    return posicion_aleatoria;
}

void crear_robot(juego_t *juego)
{
    if (juego->movimientos != 0 && juego->movimientos % MOVIMIENTOS_APARICION_ROBOT == 0)
    {
        juego->cantidad_robots++;

        juego->robots = realloc(juego->robots, sizeof(coordenada_t) * (size_t)juego->cantidad_robots);

        if (juego->robots == NULL)
        {
            printf("Error: No se pudo asignar memoria correctamente.\n");
            exit(-1);
        }

        coordenada_t posicion_aleatoria = generar_coordenada_aleatoria_unica(juego);

        juego->robots[juego->cantidad_robots - 1] = posicion_aleatoria;
    }
}

void interactuar_robot(juego_t *juego, personaje_t *perry)
{
    bool interaccion_realizada = false;
    int indice_cercano;
    while (cercano_a_robot(*perry, juego->robots, juego->cantidad_robots, &indice_cercano) && !interaccion_realizada)
    {
        if (perry->camuflado)
        {
            perry->vida--;
            interaccion_realizada = true;
        }
        else
        {

            while (cercano_a_robot(*perry, juego->robots, juego->cantidad_robots, &indice_cercano))
            {
                eliminar_robot_arreglo(juego->robots, &juego->cantidad_robots, indice_cercano);
                if (perry->energia < DESTRUCCION_ROBOT)
                {
                    perry->vida--;
                }
                else
                {
                    perry->energia -= DESTRUCCION_ROBOT;
                }
            }
            redefinir_arreglo(juego);
            interaccion_realizada = true;
        }
    }
}

void mover_familiares(familiar_t familiares[MAX_FAMILIARES], char accion, personaje_t perry, int tope_familiares)
{
    for (int i = 0; i < tope_familiares; i++)
    {
        mover_familiar(&familiares[i]);
    }
}

void consumir_herramienta(herramienta_t herramientas[MAX_HERRAMIENTAS], personaje_t *perry, int *tope_herramientas)
{
    consumir_golosina(herramientas, perry, tope_herramientas);
    consumir_sombrero(herramientas, perry, tope_herramientas);
}

void efectuar_cercania_peligrosa(juego_t *juego)
{
    for (int i = 0; i < juego->tope_familiares; i++)
    {
        if (cercania_peligrosa(*juego, i) && !juego->perry.camuflado)
        {
            juego->perry.vida--;
        }
    }
}

void desactivar_bomba(bomba_t bombas[MAX_BOMBAS], personaje_t *perry, int tope_bombas)
{
    for (int i = 0; i < tope_bombas; i++)
    {
        if (puede_desactivar_bomba(bombas, perry, i))
        {
            bombas[i].desactivada = true;
            perry->energia -= DESACTIVAR_BOMBA;
        }
    }
}

void actualizar_timer_bombas(bomba_t bombas[MAX_BOMBAS], char accion, personaje_t perry, int tope_bombas)
{
    for (int i = 0; i < tope_bombas; i++)
    {
        if (!bombas[i].desactivada)
        {
            if (bomba_explota(bombas, i))
            {
                bombas[i].timer = 0;
            }
            else
            {
                bombas[i].timer--;
            }
        }
    }
}

void explotar_bomba(bomba_t bombas[MAX_BOMBAS], personaje_t *perry, int tope_bombas)
{
    for (int i = 0; i < tope_bombas; i++)
    {
        if (bomba_explota(bombas, i))
        {
            bombas[i].desactivada = true;
            perry->vida--;
            i--;
        }
    }
}
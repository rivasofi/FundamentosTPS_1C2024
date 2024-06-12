#ifndef __AREA_LIMITROFE_H__
#define __AREA_LIMITROFE_H__
#include "feria.h"

/*Pre: "juego" debe ser válido, estar inicializado y debidamente actualizado.*/

/* Pos: Si el movimiento es múltiplo de diez, se crea un nuevo robot en el juego, se actualiza la cantidad de robots en el juego y se "realoca" el arreglo de coordenadas de robots.
En caso de fallar la realocación de memoria, se imprime un mensaje de error y el programa termina con un código de error.*/

void crear_robot(juego_t *juego);

/*Pre: "juego" y "perry" deben ser punteros válidos a estructuras de tipo juego_t y personaje_t respectivamente.*/

/*Pos: Realiza una interacción entre Perry y los robots en el juego en base al estado actual de perry y su posición.*/

void interactuar_robot(juego_t *juego, personaje_t *perry);

/*Pre: Tanto MAX_FILAS como MAX_COLUMNAS deben tener un valor positivo.*/

/*Pos: Asigna valores válidos (entre 0 y 20) a los datos dentro del struct coordenada_t (que no se repiten).*/

coordenada_t generar_coordenada_aleatoria_unica(juego_t *juego);

/*Pre: EL array "familiares" debe ser válido y estar actualizado, al igual que su tope. Perry debe estar debidamente inicializado y actualizado.*/

/*Pos: Mueve todos los familiares en el juego de acuerdo a la acción realizada.*/

void mover_familiares(familiar_t familiares[MAX_FAMILIARES], char accion, personaje_t perry, int tope_familiares);

/*Pre: El array de herramientas debe ser válido y ya estar inicializado. El tope de este mismo debe estar actualizado. Perry debe haber sido inicializado y actualizado.*/

/*Pos: Si Perry se encuentra sobre una herramienta, interactúa con ella dependiendo su tipo y actualiza los valores de Perry correspondientes.*/

void consumir_herramienta(herramienta_t herramientas[MAX_HERRAMIENTAS], personaje_t *perry, int *tope_herramientas);

/*Pre: "juego" deben ser un puntero válido a la estructura de tipo juego_t.*/

/*Pos: Si Perry se encuentra "muy cerca" de un familiar, realiza la interacción correspondiente con el mismo (reducir la vida en 1).*/

void efectuar_cercania_peligrosa(juego_t *juego);

/*Pre: "Perry" debe tener sus valores debidamente actualizados, y el array "bombas" debe ser válido y haber sido inicializado junto a su tope.*/

/*Pos: Si Perry puede desactivar una bomba en el arreglo de bombas, la bomba en la posición "i" se marca como desactivada y la energía de Perry se reduce en 10.*/

void desactivar_bomba(bomba_t bombas[MAX_BOMBAS], personaje_t *perry, int tope_bombas);

/*Pre: "Perry" debe tener sus valores debidamente actualizados, y el array "bombas" debe ser válido y haber sido inicializado junto a su tope.*/

/*Pos: Cada vez que el jugador realiza una acción, se resta 1 al timer de las bombas activadas. Si la bomba ya explotó se setea su timer en 0.*/

void actualizar_timer_bombas(bomba_t bombas[MAX_BOMBAS], char accion, personaje_t perry, int tope_bombas);

/*Pre: "Perry" debe tener sus valores debidamente actualizados, y el array "bombas" debe ser válido y haber sido inicializado junto a su tope.*/

/*Pos: Si alguna bomba "explotó", se marca como desactivada y se reduce la vida de Perry en 1.*/

void explotar_bomba(bomba_t bombas[MAX_BOMBAS], personaje_t *perry, int tope_bombas);

#endif /* __AREA_LIMITROFE_H__ */
#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "feria.h"

#define MAX_FILAS 20
#define MAX_COLUMNAS 20
#define JUEGO_EN_CURSO 0

#define COLOR_ERROR "\033[91m"
#define RESET_DE_COLOR "\033[39m"

#define MAX_ENERGIA 100
#define MAX_ACCION 10

#define ARRIBA 'W'
#define ABAJO 'S'
#define IZQUIERDA 'A'
#define DERECHA 'D'
#define CAMUFLARSE 'Q'

/*Pre: El objeto "perry" debe tener sus valores inicializados y/o actualizados (al menos el de vida).*/
/*Pos: Imprime por pantalla una cantidad de corazones que simboliza la vida restante de perry.*/

void imprimir_vidas(personaje_t perry)
{
    switch(perry.vida)
    {
        case 3:
            printf("\nVidas: ♥️  ♥️  ♥️\n\n");
            break;
        case 2:
            printf("\nVidas: ♥️  ♥️\n\n");
            break;
        case 1:
            printf("\nVidas: ♥️\n\n");
            break;
    }
}

/*Pre: El objeto "perry" debe tener sus valores inicializados y/o actualizados (al menos el de energía).*/
/*Pos: Imprime por pantalla una cantidad de rayos que simboliza la energía restante de perry.*/

void imprimir_energia(personaje_t perry)
{
    if (perry.energia <= MAX_ENERGIA)
    {
        printf("Energia: ⚡  ⚡  ⚡\n\n");
    }
    else if (perry.energia <= (MAX_ENERGIA / 3) * 2)
    {
        printf("Energia: ⚡  ⚡\n\n");
    }
    else if (perry.energia <= (MAX_ENERGIA / 3))
    {
        printf("Energia: ⚡\n\n");
    }
}

/*Pre: El objeto "perry" debe tener sus valores inicializados y/o actualizados (al menos el de estado de camuflaje).*/
/*Pos: Imprime por pantalla un mensaje que informa al usuario sobre su estado actual de camuflaje. */

void imprimir_camuflaje(personaje_t perry)
{
    if (perry.camuflado)
    {
        printf("Camuflado: Si. Solo es un ornitorrinco, no hace gran cosa...\n\n");
    }
    else
    {
        printf("Camuflado: No, ¡Buenos días Agente P!\n\n");
    }
}

/*Pre: No debe cumplir ningún requerimiento específico para ejecutarse adecuadamente. */
/*Post: Imprimirá por pantalla las instrucciones del juego. */

void imprimir_instrucciones()
{
    printf("¿Un ornitorrinco...? ¡Perry El Ornitorrinco!\n");
    printf("\nPara mover a Perry, por favor utilice las teclas W (arriba), A (izquierda), S (abajo) y D (derecha).\n");
    printf("Para camuflar a Perry, por favor utilice la tecla Q.\n");
    printf("Para salir del juego, por favor utilice la combinación de teclas CTRL + C.\n\n");
}

/*Pre: El struct juego debe tener datos válidos y actualizados sobre el estado de Perry. */
/*Post: Imprimirá por pantalla el estado actual del personaje. */

void imprimir_estado_personaje(personaje_t perry)
{
    imprimir_vidas(perry);
    imprimir_energia(perry);
    imprimir_camuflaje(perry);
}

/*Pre: Tanto juego como perry deben estar inicializados.*/
/*Pos: Imprime por pantalla lo necesario para que el usuario pueda jugar.*/

void ejecutar_ciclo_juego_pantalla(juego_t juego)
{   
    system("clear");
    imprimir_instrucciones(juego);
    imprimir_terreno(juego);
    imprimir_estado_personaje(juego.perry);
}

/*Pre: "acción" no debe ser un puntero nulo.*/
/*Pos: Guarda un caracter ingresado por el usuario en "acción".*/

void recibir_accion(char* accion)
{   
    printf("Acción: ");
    scanf(" %c", accion);
    *accion = (char)toupper(*accion);
}

/*Pre: El parámetro "acción" debería idealmente ser una letra.*/
/*Pos: Devuelve "true" si la acción ingresada coincide con las consideradas "válidas".*/

bool es_jugada_valida(char accion)
{
    bool es_valida = false;
    char accion_valida[MAX_ACCION] = {ARRIBA, ABAJO, IZQUIERDA, DERECHA, CAMUFLARSE};
    int tope = sizeof(accion_valida) / sizeof(accion_valida[0]);

    for (int i = 0; i < tope; i++)
    {
        if (accion == accion_valida[i])
        {
            es_valida = true; 
        }
    }

    return es_valida;  
}

/*Pre: La posición de perry debe estar inicializada*/
/*Pos: Imprime por pantalla un mensaje de error si el caracter ingresado no es válido. Caso contrario, omite esta función en la ejecución final.*/

void validar_jugada(char *accion, personaje_t perry)
{
    while (!es_jugada_valida(*accion))
    {
        printf(COLOR_ERROR "Error: La acción ingresada no es válida. Por favor, ingrese una acción válida.\n" RESET_DE_COLOR);
        recibir_accion(accion);
    }
}

int main()
{
    srand((unsigned int) time(NULL));

    juego_t juego;
    char accion = ' ';

    inicializar_juego(&juego);

    do {
        ejecutar_ciclo_juego_pantalla(juego);
        recibir_accion(&accion);
        validar_jugada(&accion, juego.perry);
        realizar_jugada(&juego, accion);
    } while (estado_juego(juego) == JUEGO_EN_CURSO);

    return 0;
}
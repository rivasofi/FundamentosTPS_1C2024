#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "feria.h"

/*Pre: "acción" no debe ser un puntero nulo.*/
/*Pos: Solicita y recibe un caracter, que pasará a mayúsculas con la función toupper(), y lo guardará en "acción", variable que será utilizada para realizar la jugada.*/

void recibir_accion(char* accion)
{   
    printf("Acción: ");
    scanf(" %c", accion);
    *accion = (char)toupper(*accion); /*Se castean estos valores ya que la funcion toupper(), al convertir lo que le paso a mayúsculas, devuelve un int relacionado a la 
                                        tabla ASCII. Si no realizo el casteo de manera explícita corro riesgo de tener una "perdida de data", haciéndolo de esta 
                                        manera me aseguro de que el resultado se interprete únicamente como caracter.*/
}

int main()
{

    srand((unsigned int) time(NULL));

    juego_t juego;
    char accion = ' ';
    int estado = 0;

    inicializar_juego(&juego);
    imprimir_terreno(juego);
    estado_juego(juego);

    while (estado == 0)
    {   
        recibir_accion(&accion);
        realizar_jugada(&juego, accion);
        imprimir_terreno(juego);
    }

    return 0;
}
#include "feria.h" 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define COLOR_PERRY "\033[96m" 
#define COLOR_BOMBAS "\033[33m" 
#define COLOR_GOLOSINAS "\033[35m" 
#define COLOR_SOMBREROS "\033[94m"
#define COLOR_PHINEAS "\033[91m"
#define COLOR_FERB "\033[92m"
#define COLOR_CANDACE "\033[93m"
#define COLOR_DEFAULT "\033[39m"

const char VACIO = ' ';

const int MAX_VIDAS = 3;
const int MAX_ENERGIA = 100;
const int MAX_FILAS = 20;
const int MAX_COLUMNAS = 20;

const char PERRY = 'P';

const char BOMBA = 'B';
const int CANTIDAD_BOMBAS = 10;

const char SOMBRERO = 'S';
const int CANTIDAD_SOMBREROS = 3;
const char GOLOSINA = 'G';
const int CANTIDAD_HERRAMIENTAS = 8;

const char PHINEAS = 'H';
const char FERB = 'F';
const char CANDACE = 'C';
const int CANTIDAD_FAMILIARES = 3;

#define ARRIBA 'W'
#define IZQUIERDA 'A'
#define ABAJO 'S'
#define DERECHA 'D'
#define CAMUFLARSE 'Q'

const int JUEGO_EN_CURSO = 0;
const int JUEGO_GANADO = 1;
const int JUEGO_PERDIDO = -1;

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

    while (i < juego.tope_bombas && !coordenada_existente)
    {
        if (coordenada.fil == juego.bombas[i].posicion.fil && coordenada.col == juego.bombas[i].posicion.col)
        {
            coordenada_existente = true;
        } else 
        {
            i++;
        }
    }

    i = 0;

    while (i < juego.tope_herramientas && !coordenada_existente)
    {
        if (coordenada.fil == juego.herramientas[i].posicion.fil && coordenada.col == juego.herramientas[i].posicion.col)
        {
            coordenada_existente = true;
        }
        else
        {
            i++;
        }
    }

    i = 0;

    while (i < juego.tope_familiares && !coordenada_existente)
    {
        if (coordenada.fil == juego.familiares[i].posicion.fil && coordenada.col == juego.familiares[i].posicion.col)
        {
            coordenada_existente = true;
        }
        else
        {
        i++;
        }
    }

    return coordenada_existente;
}

/*Pre: Tanto MAX_FILAS como MAX_COLUMNAS deben tener un valor positivo. No olvidar además añadir las bibliotecas <stdlib.h> y <time.h>.*/

/*Post: Asigna valores válidos (entre 0 y 20) a los datos dentro del struct coordenada_t (que no se repiten).*/

coordenada_t generar_coordenada_aleatoria_unica(juego_t* juego)
{
    coordenada_t posicion_aleatoria = {-1,-1};

    do
    {
        posicion_aleatoria.fil = rand() % MAX_FILAS;
        posicion_aleatoria.col = rand() % MAX_COLUMNAS;
    } 
    while (coordenada_existe(posicion_aleatoria, *juego));

    return posicion_aleatoria;
}

/*Pre: El puntero *perry debe ser no nulo y tener validez para así poder modificar un struct. */

/*Post: Asigna valores a cada dato dentro del struct de tipo personaje_t aplicado a Perry.*/

void inicializar_personaje(personaje_t *perry)
{
    perry->vida = MAX_VIDAS;
    perry->energia = MAX_ENERGIA;
    perry->camuflado = false;
    perry->posicion.fil = rand() % MAX_FILAS;
    perry->posicion.col = rand() % MAX_COLUMNAS;
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar un struct.*/

/*Post: Asigna valores válidos a cada dato dentro del struct de tipo bomba_t.*/    

void inicializar_bombas(juego_t* juego)
{
    juego->tope_bombas = 0;

    for (int i = 0; i < CANTIDAD_BOMBAS; i++) 
    {
        juego->bombas[i].posicion = generar_coordenada_aleatoria_unica(juego);
        juego->bombas[i].timer = rand() % 251 + 50;
        juego->bombas[i].desactivada = false;
        (juego->tope_bombas)++;
    }
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar un struct.*/
/*Pos: Asigna valores válidos, dentro del struct herramientas, a la cantidad de sombreros indicada.*/

void inicializar_sombreros(juego_t* juego)
{

    for (int i = 0; i < CANTIDAD_SOMBREROS; i++) 
    {
        juego->herramientas[i].tipo = SOMBRERO;
        juego->herramientas[i].posicion = generar_coordenada_aleatoria_unica(juego);
        (juego->tope_herramientas)++;
    }
}

/*Pre:El puntero *juego debe ser no nulo y tener validez para así poder modificar un struct.*/
/*Pos: Asigna valores válidos, dentro del struct herramientas, a la cantidad de golosinas indicada.*/

void inicializar_golosinas(juego_t* juego)
{

    for (int i = CANTIDAD_SOMBREROS; i < CANTIDAD_HERRAMIENTAS; i++) 
    {
        juego->herramientas[i].tipo = GOLOSINA;
        juego->herramientas[i].posicion = generar_coordenada_aleatoria_unica(juego);
        (juego->tope_herramientas)++;
    }
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar un struct.*/

/*Post: Asigna valores válidos a cada dato dentro del struct de tipo herramienta_t.*/

void inicializar_herramientas(juego_t* juego)
{
    juego->tope_herramientas = 0;

    inicializar_sombreros(juego);
    inicializar_golosinas(juego);
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar un struct.*/

/*Post: Asigna valores válidos a cada dato dentro el struct de tipo familiar_t.*/

void inicializar_familiares(juego_t* juego)
{
    juego->tope_familiares = 0;

    for (int i = 0; i < CANTIDAD_FAMILIARES; i++) 
    {
        if (i == 0) 
        {
            juego->familiares[i].inicial_nombre = PHINEAS;
        }
        else if (i == 1) 
        {
            juego->familiares[i].inicial_nombre = FERB;
        } 
        else 
        {
            juego->familiares[i].inicial_nombre = CANDACE;
        }

        juego->familiares[i].posicion = generar_coordenada_aleatoria_unica(juego);
        (juego->tope_familiares)++;
    }
}

/*Pre: La matriz a inicializar debe estar previamente definida.*/

/*Post: Inicializa todos los valores de la matriz con un caracter vacío.*/

void inicializar_matriz_vacia(char matriz[MAX_FILAS][MAX_COLUMNAS])
{
    for (int i = 0; i < MAX_FILAS; i++)
    {
        for (int j = 0; j < MAX_COLUMNAS; j++)
        {
            matriz[i][j] = VACIO;
        }
    }
}

/*Pre: La matriz mapa debe ser de MAX[FILAS]xMAX[COLUMNAS]*/
/*Post: Carga (o actualiza) la posición de los objetos en el mapa.*/

void cargar_objetos_en_mapa(juego_t juego, char mapa[MAX_FILAS][MAX_COLUMNAS])
{
    for (int i = 0; i < juego.tope_bombas; i++)
    {
        mapa[juego.bombas[i].posicion.fil][juego.bombas[i].posicion.col] = BOMBA;
    }
    
    for (int i = 0; i < juego.tope_herramientas; i++)
    {
        mapa[juego.herramientas[i].posicion.fil][juego.herramientas[i].posicion.col] = juego.herramientas[i].tipo;
    }

    for (int i = 0; i < juego.tope_familiares; i++)
    {
        mapa[juego.familiares[i].posicion.fil][juego.familiares[i].posicion.col] = juego.familiares[i].inicial_nombre;
    }

    mapa[juego.perry.posicion.fil][juego.perry.posicion.col] = PERRY;
}

/*Pre: El parámetro "caracter" debe haberse asignado previamente a un dato con el fin de identificarlo. El parámetro "color" debe representar un código en formato
ANSI cuya visualización sea correcta en la terminal.*/

/*Post: Imprime el caracter deseado con el color que se le asigne.*/

void asignar_color_caracter(char caracter, const char *color)
{
    printf("%s%c%s", color, caracter, COLOR_DEFAULT);
}

/*Pre: El arreglo de bombas debe estar ya inicializado y "bombas" debe ser de tipo bomba_t.*/

/*Pos: Retorna true si encuentra alguna bomba activada en el array de bombas. Sino, retorna false.*/

bool hay_bombas_activadas(bomba_t bombas[MAX_BOMBAS])
{
    int i = 0;
    bool bombas_activadas = false;

    while ((!bombas_activadas) && i < CANTIDAD_BOMBAS)
    {
        if (bombas[i].desactivada == false)
        {
            bombas_activadas = true;
        }

        i++;

    }

    return bombas_activadas;
}

/*Pre: El parámetro "acción" debe ser una letra, sino no hará movimiento y no verificará nada. La posición de perry debe estar inicializada.*/
/*Pos: Retorna un booleano que simboliza si el movimiento a realizar está dentro de los límites de la matriz.*/

bool jugada_dentro_de_limites(char accion, personaje_t perry)
{
    switch (accion)
    {
        case ARRIBA:
            return perry.posicion.fil > 0;
        case ABAJO:
            return perry.posicion.fil < MAX_FILAS - 1;
        case IZQUIERDA:
            return perry.posicion.col > 0;
        case DERECHA:
            return perry.posicion.col < MAX_COLUMNAS - 1;
        default:
            return false;
    }
}

/*Pre: El elemento "vida" del objeto "perry" debe ya tener inicializado su valor. */

/*Pos: Retorna true si perry aún tiene vidas (dentro de los valores válidos)*/

bool perry_tiene_vida(personaje_t perry)
{
    return (perry.vida > 0 && perry.vida <= MAX_VIDAS);
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar el struct, inicializando los datos que correspondan.*/

/*Post: Se inicializan los objetos necesarios para el adecuado funcionamiento del juego, siendo estos Perry, los obstaculos, las herramientas y la familia Flynn.*/

void inicializar_juego(juego_t* juego)
{
    juego->tope_bombas = 0;
    juego->tope_herramientas = 0;
    juego->tope_familiares = 0;

    inicializar_personaje(&juego->perry);

    inicializar_bombas(juego);

    inicializar_herramientas(juego);

    inicializar_familiares(juego);
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para asi poder ver el estado actual del juego junto a las posiciones. El caracter "acción"
ya debe ingresarse validado*/

/*Post: Cambia la coordenada actual de Perry (a no ser que se salga del límite) o su estado de camuflaje de acuerdo a lo que ingrese el usuario.*/

void realizar_jugada(juego_t* juego, char accion)
{
    if (jugada_dentro_de_limites(accion, juego->perry))
    {
        switch (accion)
        {
            case ARRIBA:
                juego->perry.posicion.fil--;
                break;
            case ABAJO:
                juego->perry.posicion.fil++;
                break;
            case IZQUIERDA:
                juego->perry.posicion.col--;
                break;
            case DERECHA:
                juego->perry.posicion.col++;
                break;
        }
    }
    else if (accion == CAMUFLARSE)
    {
        juego->perry.camuflado = !juego->perry.camuflado;
    }
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para asi poder ver el estado actual del juego junto a las posiciones. Los valores de MAX_FILAS y 
MAX_COLUMNAS deben ser positivos y coherentes al tamaño del mapa de juego. Debe estar declarada la biblioteca <stdlib.h> para el uso de system("clear")*/

/*Post: Imprime por pantalla el terreno de juego.*/

void imprimir_terreno(juego_t juego)
{   

    char mapa[MAX_FILAS][MAX_COLUMNAS];

    inicializar_matriz_vacia(mapa);

    cargar_objetos_en_mapa(juego, mapa);

    for (int i = 0; i < MAX_FILAS; i++)
    {
        for (int j = 0; j<MAX_COLUMNAS; j++)
        {
            if (j == 0) 
            {
                printf("|_");
            } 
            else if (j == MAX_COLUMNAS-1) 
            {
                printf("_|__");
            } 
            else 
            {
                printf("_|_");
            }

            switch(mapa[i][j])
            {
                case 'P':
                    asignar_color_caracter(mapa[i][j], COLOR_PERRY);
                    break;
                case 'B':
                    asignar_color_caracter(mapa[i][j], COLOR_BOMBAS);
                    break;
                case 'S':
                    asignar_color_caracter(mapa[i][j], COLOR_SOMBREROS);
                    break;
                case 'G':
                    asignar_color_caracter(mapa[i][j], COLOR_GOLOSINAS);
                    break;
                case 'H':
                    asignar_color_caracter(mapa[i][j], COLOR_PHINEAS);
                    break;
                case 'F':
                    asignar_color_caracter(mapa[i][j], COLOR_FERB);
                    break;
                case 'C':
                    asignar_color_caracter(mapa[i][j], COLOR_CANDACE);
                    break;
                default:
                    printf("%c", mapa[i][j]);
                    break;
            }

        }
        printf("\n");
    }

}

/*Pre: El puntero *juego debe ser no nulo y tener validez para asi poder ver el estado actual del juego a partir del valor de diversos datos dentro de los struct.*/

/*Post: Devuelve un entero que representa el estado actual del juego.*/

int estado_juego(juego_t juego)
{
    int estado = 0;

    if (perry_tiene_vida(juego.perry) && !hay_bombas_activadas(juego.bombas))
    {
        estado = JUEGO_GANADO;
    } 
    else if (!perry_tiene_vida(juego.perry))
    {
        estado = JUEGO_PERDIDO;
    } 
    else 
    {
        estado = JUEGO_EN_CURSO;
    }

    return estado;
}  
#include "feria.h" 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*COLORES*/

#define ANSI_COLOR_CYAN "\033[96m" /*PERRY*/
#define ANSI_COLOR_YELLOW "\033[33m" /*BOMBAS*/
#define ANSI_COLOR_PINK "\033[35m" /*GOLOSINAS*/
#define ANSI_COLOR_BLUE "\033[94m" /*SOMBREROS*/
#define ANSI_COLOR_RED "\033[91m" /*PHINEAS*/
#define ANSI_COLOR_BRIGHTGREEN "\033[92m" /*FERB*/
#define ANSI_COLOR_LIGHT_MAGENTA "\033[93m" /*CANDACE*/
#define ANSI_COLOR_RESET "\033[39m" /*RESET*/

/*VALOR VACÍO*/

const char VACIO = ' ';

/*VALORES MÁXIMOS*/

const int MAX_VIDAS = 3;
const int MAX_ENERGIA = 100;
const int MAX_FILAS = 20;
const int MAX_COLUMNAS = 20;

/*PERRY*/

const char PERRY = 'P';

/*BOMBAS*/

const char BOMBA = 'B';
const int CANTIDAD_BOMBAS = 10;

/*HERRAMIENTAS*/

const char SOMBRERO = 'S';
const int CANTIDAD_SOMBREROS = 3;
const char GOLOSINA = 'G';
const int CANTIDAD_HERRAMIENTAS = 8;

/*FAMILIA*/

const char PHINEAS = 'H';
const char FERB = 'F';
const char CANDACE = 'C';
const int CANTIDAD_FAMILIARES = 3;

/*MOVIMIENTOS Y ACCIONES*/

#define ARRIBA 'W'
#define IZQUIERDA 'A'
#define ABAJO 'S'
#define DERECHA 'D'
#define CAMUFLARSE 'Q'

/*Funciones propias*/

/*Pre: Tanto MAX_FILAS como MAX_columnas deben tener un valor positivo. No olvidar además añadir las bibliotecas <stdlib.h> y <time.h>.*/

/*Post: Asigna valores válidos (entre 0 y 20) a los datos dentro del struct coordenada_t.*/

coordenada_t generar_coordenada_aleatoria()
{
    coordenada_t coordenada;
    coordenada.fil = rand() % MAX_FILAS;
    coordenada.col = rand() % MAX_COLUMNAS;

    return coordenada;
}

/*Pre: El parámetro "coordenada" debe estar dentro de los límites establecidos del juego, al ser esta función normalmente trabajada junto a
la función "generar_coordenada_aleatoria" esto ocurre ya por defecto.*/

/*Post: La función devolverá "true" si la coordenada pasada ya existe en alguna de las posiciones especificadas. De lo contrario devolverá false.*/

bool coordenada_existe(coordenada_t coordenada, juego_t juego)
{
    
    if (coordenada.fil ==juego.perry.posicion.fil && coordenada.col == juego.perry.posicion.col) {
        return true;
    }

    for (int i = 0; i < CANTIDAD_BOMBAS; i++) {
        if (coordenada.fil == juego.bombas[i].posicion.fil && coordenada.col == juego.bombas[i].posicion.col) {
            return true;
        }
    }
    
    for (int i = 0; i < CANTIDAD_HERRAMIENTAS; i++) {
        if (coordenada.fil == juego.herramientas[i].posicion.fil && coordenada.col == juego.herramientas[i].posicion.col) {
            return true;
        }
    }
    
    for (int i = 0; i < CANTIDAD_FAMILIARES; i++) {
        if (coordenada .fil == juego.familiares[i].posicion.fil && coordenada.col == juego.familiares[i].posicion.col) {
            return true;
        }
    }

    return false;

}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar el struct. */

/*Post: Asigna valores a cada dato dentro del struct de tipo personaje_t aplicado a Perry. Verifica además la no-existencia de la coordenada asignada, caso contrario
generará una nueva para evitar repeticiones hasta que esto deje de suceder.*/

void inicializar_personaje(juego_t* juego)
{
    coordenada_t nueva_posicion = generar_coordenada_aleatoria();

    while (coordenada_existe(nueva_posicion, *juego)){
        nueva_posicion = generar_coordenada_aleatoria();
    }

    personaje_t perry = {MAX_VIDAS, MAX_ENERGIA, false, nueva_posicion};

    juego->perry = perry;

}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar el struct.*/

/*Post: Asigna valores a cada dato dentro del struct bomba_t aplicado a las bombas que se usarán dentro del juego. Verifica antes la no-existencia 
de la coordenada asignada, caso contrario generará una nueva para evitar repeticiones hasta que esto deje de suceder y luego de esa verificación completará
definitivamente el struct de tipo bomba_t.*/    

void inicializar_bombas(juego_t* juego)
{
    juego->tope_bombas = CANTIDAD_BOMBAS;

    for (int i = 0; i < juego->tope_bombas; i++) {
        
        coordenada_t nueva_posicion = generar_coordenada_aleatoria();

        while (coordenada_existe(nueva_posicion, *juego)) {
            nueva_posicion = generar_coordenada_aleatoria(); 
        }

        juego->bombas[i].posicion = nueva_posicion;
        juego->bombas[i].timer = rand() % 251 + 50;
        juego->bombas[i].desactivada = false;
    }
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar el struct.*/

/*Post: Asigna valores a cada dato dentro del struct herramienta_t aplicado a las herramientas que se usarán dentro del juego, utilizando el índice de cada una 
dentro del arreglo para asignarlas correctamente. Verifica antes la no-existencia de la coordenada asignada, caso contrario generará una nueva para evitar repeticiones
hasta que esta deje de repetirse y luego completará definitivamente el struct de tipo herramienta_t correspondiente.*/

void inicializar_herramientas(juego_t* juego)
{
    juego->tope_herramientas = CANTIDAD_HERRAMIENTAS;
    for (int i = 0; i < CANTIDAD_SOMBREROS; i++) {

        coordenada_t nueva_posicion = generar_coordenada_aleatoria();

        while (coordenada_existe(nueva_posicion, *juego)) {
            nueva_posicion = generar_coordenada_aleatoria();
        }

        juego->herramientas[i].posicion = nueva_posicion;
        juego->herramientas[i].tipo = SOMBRERO;
    }

    for (int i = CANTIDAD_SOMBREROS; i < juego->tope_herramientas; i++) {

        coordenada_t nueva_posicion = generar_coordenada_aleatoria();

        while (coordenada_existe(nueva_posicion, *juego)) {
            nueva_posicion = generar_coordenada_aleatoria();
        }

        juego->herramientas[i].posicion = nueva_posicion;
        juego->herramientas[i].tipo = GOLOSINA;
    }
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar el struct.*/

/*Post: Asigna valores a cada dato dentro del struct familiar_t aplicado a las herramientas que se usarán dentro del juego, utilizando el índice de cada uno 
dentro del arreglo para asignarlos correctamente. Verifica antes la no-existencia de la coordenada asignada, caso contrario generará una nueva para evitar repeticiones
hasta que esto deje de suceder, luego completará definitivamente el struct de tipo familiar_t correspondiente.*/

void inicializar_familiares(juego_t* juego)
{
    juego->tope_familiares = CANTIDAD_FAMILIARES;

    for (int i = 0; i < juego->tope_familiares; i++) {

        if (i == 0) {
            juego->familiares[i].inicial_nombre = PHINEAS;
        } else if (i == 1) {
            juego->familiares[i].inicial_nombre = FERB;
        } else {
            juego->familiares[i].inicial_nombre = CANDACE;
        }

        coordenada_t nueva_posicion = generar_coordenada_aleatoria();

        while (coordenada_existe(nueva_posicion, *juego)) {
            nueva_posicion = generar_coordenada_aleatoria();
        }

        juego->familiares[i].posicion = nueva_posicion;
    }
}

/*Pre: La matriz a inicializar debe estar definida previamente en la función donde se la utiliza.*/
/*Post: Inicializará todos los valores de la matriz con un caracter vacío, evitando que esta se inicialice con basura.*/

void inicializar_matriz_vacia(char matriz[MAX_FILAS][MAX_COLUMNAS])
{
    for (int i = 0; i < MAX_FILAS; i++){
        for (int j = 0; j < MAX_COLUMNAS; j++){
            matriz[i][j] = VACIO;
        }
    }
}

/*Pre: La matriz mapa debe ser de MAX[FILAS]xMAX[COLUMNAS]*/
/*Post: Carga (o actualiza) la posición de los objetos en el mapa.*/

void cargar_objetos_en_mapa(juego_t juego, char mapa[MAX_FILAS][MAX_COLUMNAS]){

    for (int i = 0; i < CANTIDAD_BOMBAS; i++){
        mapa[juego.bombas[i].posicion.fil][juego.bombas[i].posicion.col] = BOMBA;
    }
    
    for (int i = 0; i < CANTIDAD_HERRAMIENTAS; i++){
        mapa[juego.herramientas[i].posicion.fil][juego.herramientas[i].posicion.col] = juego.herramientas[i].tipo;
    }

    for (int i = 0; i < CANTIDAD_FAMILIARES; i++){
        mapa[juego.familiares[i].posicion.fil][juego.familiares[i].posicion.col] = juego.familiares[i].inicial_nombre;
    }

    mapa[juego.perry.posicion.fil][juego.perry.posicion.col] = PERRY;
}

/*Pre: No debe cumplir ningún requerimiento específico para ejecutarse adecuadamente. */
/*Post: Imprimirá por pantalla las instrucciones del juego, informandole al usuario los controles del juego. */

void imprimir_instrucciones()
{
    printf("¿Un ornitorrinco...? ¡Perry El Ornitorrinco!\n");
    printf("\nPara mover a Perry, por favor utilice las teclas W (arriba), A (izquierda), S (abajo) y D (derecha).\n");
    printf("Para camuflar a Perry, por favor utilice la tecla Q.\n");
    printf("Para salir del juego, por favor utilice la combinación de teclas CTRL + C.\n\n");
}

/*Pre: El struct juego debe tener datos válidos y actualizados sobre el estado de Perry. */
/*Post: Imprimirá por pantalla el estado actual del personaje en relación a sus acciones dentro del juego. */

void imprimir_estado_personaje(juego_t juego)
{
    printf("Vidas: %i\n", juego.perry.vida);
    printf("Energía: %i\n", juego.perry.energia);
    printf("Camuflado: %s\n", juego.perry.camuflado ? "Sí" : "No");
}

/*Pre: El parámetro "caracter" debe haberse asignado previamente a un dato con el fin de identificarlo. El parámetro "color" debe representar un código en formato
ANSI cuya visualización sea correcta en la terminal.*/

/*Post: Imprime el caracter deseado con el color que se le asigne, sustituyendose %s por el color deseado, %c por el caracter y el segundo %s por un reset al color.*/

void asignar_color_caracter(char caracter, const char *color)
{
    printf("%s%c%s", color, caracter, ANSI_COLOR_RESET);
}


/*Funciones de la biblioteca asignada por la cátedra*/


/*Pre: El puntero *juego debe ser no nulo y tener validez para así poder modificar el struct, inicializando los datos que correspondan.*/

/*Post: Se inicializarán los objetos necesarios para el adecuado funcionamiento del juego, siendo estos Perry, los obstaculos, las herramientas y la familia Flynn.*/

void inicializar_juego(juego_t* juego)
{
    /*Perry*/

    inicializar_personaje(juego);

    /*Bombas*/

    inicializar_bombas(juego);

    /*HERRAMIENTAS*/

    inicializar_herramientas(juego);

    /*FAMILIARES*/

    inicializar_familiares(juego);

}

/*Pre: El puntero *juego debe ser no nulo y tener validez para asi poder ver el estado actual del juego junto a las posiciones. El caracter "acción"
puede ser cualquiera ya que se contemplan todos los casos posibles de ingreso (obligatoriamente, dado el tipo de dato, debe ser una letra).*/

/*Post: Cambiara la coordenada actual de Perry de acuerdo a lo que ingrese el usuario. Si lo que se ingresó no es válido o el movimiento ingresado sale de los
límites de la matriz, vuelve a pedirle al usuario que ingrese un caracter hasta que este tenga validez.
Adicional al movimiento, se podrá cambiar el estado de camuflaje de Perry o salir del juego si el usuario así lo quiere.*/

void realizar_jugada(juego_t* juego, char accion)
{

    switch (accion)
    {
        case ARRIBA:
            if (juego->perry.posicion.fil > 0){
                juego->perry.posicion.fil--;
            }
            break;
        case ABAJO:
            if (juego->perry.posicion.fil < MAX_FILAS-1)
            {
                juego->perry.posicion.fil++;
            }
            break;
        case DERECHA:
            if (juego->perry.posicion.col < MAX_COLUMNAS-1)
            {
                juego->perry.posicion.col++;
            }
            break;
        case IZQUIERDA:
            if (juego->perry.posicion.col > 0)
            {
                juego->perry.posicion.col--;
            }
            break;
        case CAMUFLARSE:
            if (juego->perry.camuflado == false){
                juego->perry.camuflado = true;
            } else {
                juego->perry.camuflado = false;
            }
            break;
    }
}

/*Pre: El puntero *juego debe ser no nulo y tener validez para asi poder ver el estado actual del juego junto a las posiciones. Los valores de MAX_FILAS y 
MAX_COLUMNAS deben ser positivos y coherentes al tamaño del mapa de juego. Debe estar declarada la biblioteca <stdlib.h> para el uso de system("clear")*/

/*Post: Imprimirá por pantalla el terreno, que incluirá la representación visual de todo lo que se inicializó en la función inicializar_juego. Como
funcionalidad adicional, cada tipo de elemento será diferenciado por un color específico. Además, se imprimirá también el estado actual del personaje, incluyendo
sus vidas, energía y camuflaje.*/

void imprimir_terreno(juego_t juego)
{   

    char mapa[MAX_FILAS][MAX_COLUMNAS];

    system("clear");

    imprimir_instrucciones();

    inicializar_matriz_vacia(mapa);

    cargar_objetos_en_mapa(juego, mapa);

    for (int i = 0; i < MAX_FILAS; i++){
        for (int j = 0; j<MAX_COLUMNAS; j++){
            if (j == 0) {
                printf("|_");
            } else if (j == MAX_COLUMNAS-1) {
                printf("_|__");
            } else {
                printf("_|_");
            }

            switch(mapa[i][j]){
                case 'P':
                    asignar_color_caracter(mapa[i][j], ANSI_COLOR_CYAN);
                    break;
                case 'B':
                    asignar_color_caracter(mapa[i][j], ANSI_COLOR_YELLOW);
                    break;
                case 'S':
                    asignar_color_caracter(mapa[i][j], ANSI_COLOR_BLUE);
                    break;
                case 'G':
                    asignar_color_caracter(mapa[i][j], ANSI_COLOR_PINK);
                    break;
                case 'H':
                    asignar_color_caracter(mapa[i][j], ANSI_COLOR_RED);
                    break;
                case 'F':
                    asignar_color_caracter(mapa[i][j], ANSI_COLOR_BRIGHTGREEN);
                    break;
                case 'C':
                    asignar_color_caracter(mapa[i][j], ANSI_COLOR_LIGHT_MAGENTA);
                    break;
                default:
                    printf("%c", mapa[i][j]);
                    break;
            }

        }
        printf("\n");
    }

    imprimir_estado_personaje(juego);

}

/*Pre: El puntero *juego debe ser no nulo y tener validez para asi poder ver el estado actual del juego a partir del valor de diversos datos dentro de los struct.*/

/*Post: Evaluará el estado actual del juego en base a la vida del personaje y las bombas desactivadas, devolviendo un entero que represente si el jugador
ha perdido, ganado, o puede seguir jugando. Si el jugador ha finalizado el juego, ya sea de forma exitosa o sin éxito, se imprimirá por pantalla un mensaje
de acuerdo a como ha finalizado.*/

int estado_juego(juego_t juego)
{
    int estado = 0;
    bool bombas_desactivadas = true;

    for (int i = 0; i < CANTIDAD_BOMBAS; i++){
        if (juego.bombas[i].desactivada == false){

            bombas_desactivadas = false;
        }
    }

    int perry_vida = juego.perry.vida;


    if (bombas_desactivadas == true && perry_vida>0){
        estado = 1;
    } else if (perry_vida == 0){
        estado = -1;
    } else {
        estado = 0;
    }

    return estado;
}  

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#include "feria.h"

#define MAX_FILAS 20
#define MAX_COLUMNAS 20
#define JUEGO_EN_CURSO 0

#define COLOR_ADVERTENCIA_LEVE "\033[92m"
#define COLOR_ADVERTENCIA_MODERADA "\033[93m"
#define COLOR_ADVERTENCIA_IMPORTANTE "\033[91m"
#define RESET_DE_COLOR "\033[39m"

#define MAX_ENERGIA 100
#define MAX_ACCION 10

#define MOVIMIENTOS_APARICION_ROBOT 10

#define ARRIBA 'W'
#define ABAJO 'S'
#define IZQUIERDA 'A'
#define DERECHA 'D'
#define CAMUFLARSE 'Q'

/*Pre: El puntero "*juego" debe ser válido para poder modificarlo.*/

/*Pos: Libera la memoria pedida al sistema para el array de robots si este es no-nulo y lo establece en ese valor.*/

void liberar_juego(juego_t *juego)
{
    if (juego->robots != NULL)
    {
        printf("Liberando memoria...\n");
        free(juego->robots);
        juego->robots = NULL;
    }
}

/*Pre: El parámetro "sig" debería ser igual a SIGINT (que equivale a un Ctrl+C) y el puntero "*juego" debe ser válido para poder modificarlo.*/

/*Pos: Si se detecta salida por SIGINT se libera la memoria de juego->robots y se imprime por pantalla un mensaje informando esto. Luego sale del juego.*/

void manejo_sigint(int sig, juego_t* juego)
{
    if (sig == SIGINT)
    {
        printf("\nCtrl+C detectado. Liberando juego y saliendo...\n");
        liberar_juego(juego);
        exit(0);
    }
}

/*Pre: El objeto "perry" debe tener sus valores inicializados y/o actualizados (al menos el de vida).*/

/*Pos: Imprime por pantalla una cantidad de corazones que simboliza la vida restante de perry.*/

void imprimir_vidas(personaje_t perry)
{
    printf("\nVidas: ");
    for (int i = 0; i < perry.vida; i++)
    {
        printf("❤️  ");
    }
}

/*Pre: El objeto "perry" debe tener sus valores inicializados y/o actualizados (al menos el de energía).*/

/*Pos: Imprime por pantalla una cantidad de rayos que simboliza la energía restante de perry junto al número real.*/

void imprimir_energia(personaje_t perry)
{
    if (perry.energia >= (MAX_ENERGIA / 3) * 2)
    {
        printf("\n\nEnergia: ⚡  ⚡  ⚡  [%i]\n\n", perry.energia);
    }
    else if (perry.energia >= MAX_ENERGIA / 3)
    {
        printf("\n\nEnergia: ⚡  ⚡  [%i]\n\n", perry.energia);
    }
    else
    {
        printf("\n\nEnergia: ⚡  [%i]\n\n", perry.energia);
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
        printf("Camuflado: No, ¡Buenos días Agente P! 🎩\n\n");
    }
}

/*Pre: No debe cumplir ningún requerimiento específico para ejecutarse adecuadamente. */

/*Post: Imprimirá por pantalla las instrucciones del juego. */

void imprimir_instrucciones()
{
    printf("¿Un ornitorrinco...? ¡Perry El Ornitorrinco!\n");
    printf("\nPara mover a Perry, por favor utilice las teclas W (arriba), A (izquierda), S (abajo) y D (derecha).\n");
    printf("Para camuflar a Perry, por favor utilice la tecla Q.\n");
    printf("Para salir del juego, por favor utilice la combinación de teclas CTRL + C.\n");
    printf("Para ganar el juego, tenes que desactivar todas las bombas antes de quedarte sin vidas.\n\n");
    printf("Recorda que cada 10 movimientos, aparecerá un robot en el mapa ¡Cuidado!\n\n");
}

/*Pre: El struct juego debe tener datos válidos y actualizados sobre el estado de Perry. */

/*Post: Imprime por pantalla el estado actual del personaje. */

void imprimir_estado_personaje(personaje_t perry)
{
    imprimir_vidas(perry);
    imprimir_energia(perry);
    imprimir_camuflaje(perry);
}

/*Pre: El array bombas debe tener datos válidos y actualizados sobre el estado de estas. Al parámetro tope_bombas se le debe haber asignado un valor.*/

/*Pos: Imprime por pantalla el estado actual de las bombas y su posición.*/

void imprimir_informacion_bombas(bomba_t bombas[MAX_BOMBAS], int tope_bombas)
{
    printf("Te quedan las siguientes bombas:\n\n");

    for (int i = 0; i < tope_bombas; i++)
    {
        if (!bombas[i].desactivada)
        {
            printf("Bomba %i: (%i, %i) - Timer: %i - 💣\n", i + 1, bombas[i].posicion.fil, bombas[i].posicion.col, bombas[i].timer);
        }
    }

    printf("\nPor ahora, explotaron:\n\n");

    for (int i = 0; i < tope_bombas; i++)
    {
        if (bombas[i].timer == 0 && bombas[i].desactivada)
        {
            printf("Bomba %i: (%i, %i) - 💥\n", i + 1, bombas[i].posicion.fil, bombas[i].posicion.col);
        }
    }

    printf("\n");
}

/*Pre: El struct juego debe tener datos válidos y actualizados sobre el estado de los robots y los movimientos de Perry.*/

/*Pos: Imprime por pantalla los movimientos restantes para la aparición de un robot.*/

void imprimir_informacion_robots(juego_t juego)
{
    int movimientos_perry = juego.movimientos;

    if ((MOVIMIENTOS_APARICION_ROBOT - (movimientos_perry % MOVIMIENTOS_APARICION_ROBOT)) == 3)
    {
        printf(COLOR_ADVERTENCIA_LEVE);
    }
    else if ((MOVIMIENTOS_APARICION_ROBOT - (movimientos_perry % MOVIMIENTOS_APARICION_ROBOT)) == 2)
    {
        printf(COLOR_ADVERTENCIA_MODERADA);
    }
    else if ((MOVIMIENTOS_APARICION_ROBOT - (movimientos_perry % MOVIMIENTOS_APARICION_ROBOT)) == 1)
    {
        printf(COLOR_ADVERTENCIA_IMPORTANTE);
    }

    printf("Faltan %i movimientos para que aparezca un robot\n\n", MOVIMIENTOS_APARICION_ROBOT - (movimientos_perry % MOVIMIENTOS_APARICION_ROBOT));
    printf(RESET_DE_COLOR);
}

/*Pre: Tanto juego como perry deben estar inicializados.*/

/*Pos: Imprime por pantalla lo necesario para que el usuario pueda jugar.*/

void ejecutar_ciclo_juego_pantalla(juego_t juego)
{
    system("clear");
    imprimir_instrucciones(juego);
    imprimir_terreno(juego);
    imprimir_estado_personaje(juego.perry);
    imprimir_informacion_robots(juego);
    imprimir_informacion_bombas(juego.bombas, juego.tope_bombas);
}

/*Pre: "acción" debe ser un puntero válido.*/

/*Pos: Guarda un caracter ingresado por el usuario en "acción".*/

void recibir_accion(char *accion)
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
    int i = 0;

    while (i < tope && !es_valida)
    {
        if (accion == accion_valida[i])
        {
            es_valida = true;
        }
        i++;
    }

    return es_valida;
}

/*Pre: La posición de perry debe estar inicializada*/

/*Pos: Imprime por pantalla un mensaje de error si el caracter ingresado no es válido. Caso contrario, omite esta función en la ejecución final.*/

void validar_jugada(char *accion, personaje_t perry)
{
    while (!es_jugada_valida(*accion))
    {
        printf(COLOR_ADVERTENCIA_IMPORTANTE "Error: La acción ingresada no es válida. Por favor, ingrese una acción válida.\n" RESET_DE_COLOR);
        recibir_accion(accion);
    }
}

/*Pre: La variable "estado" debe estar actualizada de manera tal que refleje si el jugador ganó o perdió.*/

/*Pos: Imprime por pantalla un ASCII ART que informa al usuario de que forma ha finalizado el juego.*/

void dar_mensaje_final(juego_t juego, int estado)
{
    if (estado == 1)
    {
        system("clear");

        printf("\n\n\n                  GGGGGGGGGGGGG                                                                               tttt                                !!! \n");
        printf("               GGG::::::::::::G                                                                             ttt:::t                              !!:!!\n");
        printf("              GG:::::::::::::::G                                                                            t:::::t                              !:::!\n");
        printf("             G:::::GGGGGGGG::::G                                                                            t:::::t                              !:::!\n");
        printf("            G:::::G       GGGGGG   aaaaaaaaaaaaa  nnnn  nnnnnnnn      aaaaaaaaaaaaa      ssssssssss   ttttttt:::::ttttttt        eeeeeeeeeeee    !:::!\n");
        printf("            G:::::G                a::::::::::::a n:::nn::::::::nn    a::::::::::::a   ss::::::::::s  t:::::::::::::::::t      ee::::::::::::ee  !:::!\n");
        printf("            G:::::G                aaaaaaaaa:::::an::::::::::::::nn   aaaaaaaaa:::::ass:::::::::::::s t:::::::::::::::::t     e::::::eeeee:::::ee!:::!\n");
        printf("            G:::::G    GGGGGGGGGG           a::::ann:::::::::::::::n           a::::as::::::ssss:::::stttttt:::::::tttttt    e::::::e     e:::::e!:::!\n");
        printf("            G:::::G    G::::::::G    aaaaaaa:::::a  n:::::nnnn:::::n    aaaaaaa:::::a s:::::s  ssssss       t:::::t          e:::::::eeeee::::::e!:::!\n");
        printf("            G:::::G    GGGGG::::G  aa::::::::::::a  n::::n    n::::n  aa::::::::::::a   s::::::s            t:::::t          e:::::::::::::::::e !:::!\n");
        printf("            G:::::G        G::::G a::::aaaa::::::a  n::::n    n::::n a::::aaaa::::::a      s::::::s         t:::::t          e::::::eeeeeeeeeee  !!:!!\n");
        printf("             G:::::G       G::::Ga::::a    a:::::a  n::::n    n::::na::::a    a:::::assssss   s:::::s       t:::::t    tttttte:::::::e            !!! \n");
        printf("              G:::::GGGGGGGG::::Ga::::a    a:::::a  n::::n    n::::na::::a    a:::::as:::::ssss::::::s      t::::::tttt:::::te::::::::e               \n");
        printf("               GG:::::::::::::::Ga:::::aaaa::::::a  n::::n    n::::na:::::aaaa::::::as::::::::::::::s       tt::::::::::::::t e::::::::eeeeeeee   !!! \n");
        printf("                 GGG::::::GGG:::G a::::::::::aa:::a n::::n    n::::n a::::::::::aa:::as:::::::::::ss          tt:::::::::::tt  ee:::::::::::::e  !!:!!\n");
        printf("                    GGGGGG   GGGG  aaaaaaaaaa  aaaa nnnnnn    nnnnnn  aaaaaaaaaa  aaaa sssssssssss              ttttttttttt      eeeeeeeeeeeeee   !!! \n\n\n\n\n\n\n");

        printf("                                                            ⠀⠀⢞⠳⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                           ⠀⠀⠀⠀⠳⣜⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⠀⠈⢦⠱⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⠀⠀⢀⣳⣽⣶⠶⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⠀⣴⡛⠉⠁⠀⠀⠈⢷⡀⠀⣴⣿⣷⣶⣦⣄⣀⠀⠀⠀⣠⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠠⠴⠖⠚⠉⠉⠉⠉⠉⠲⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⠀⠈⢹⡗⠀⠀⠀⠀⠈⠛⠾⣿⣿⣿⣿⣿⣿⣿⣷⣴⣿⣿⣿⣧⠀⠀⠀⠀⠀⢀⠀⣠⠖⣋⡀⠀⢀⡤⠔⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⠦⠤⢶⢒⣿⠙⣷⣶⣭⣙⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⣠⣴⣶⣿⡇⣿⠁⠀⣩⠏⠘⠒⠤⣀⠀⠀⠀⠀⢰⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣉⣉⣹⢸⣿⠀⣿⣿⣿⣿⣿⣶⣄⡉⠛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠀⣟⣠⠞⠁⠀⠀⠀⠀⠈⠙⢲⢤⠖⠺⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⠧⣤⢬⣿⠘⣧⣿⣿⣿⣿⣿⣿⣿⣿⣶⣄⡀⠉⠛⠿⣏⣿⣿⠟⠃⠀⠀⢻⠧⣄⣀⠀⠀⠀⣠⠄⠀⠀⠈⠓⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                        ⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⢃⡞⢹⣇⠘⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⢦⣄⠀⠈⠙⠻⣦⡀⠀⠀⢸⠂⠀⠈⢙⠆⣏⡁⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⣀⠤⢚⣩⡶⠋⠀⠀⢻⣴⣿⣿⣿⣿⣿⣿⠿⣯⡾⣿⡿⢀⡿⠳⣄⠀⠀⠈⠹⣦⡀⢸⡀⢀⠞⠁⠀⠀⠉⠓⢦⣀⣠⢴⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⡠⢖⡭⠶⠛⠁⠀⠀⣀⣤⣾⣿⣿⣿⠿⣿⣿⡗⢚⡶⠖⠿⢶⣲⣋⣀⣀⠈⠓⢤⡀⠀⠈⣧⠈⣇⢏⡀⠀⠀⠀⣠⠄⠈⠈⠳⢾⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⢠⢞⡕⠋⠀⠀⠀⠀⠐⠛⠛⠛⠉⠉⠀⢻⣷⣿⣿⡥⣞⣇⡠⠀⠀⠀⠉⠉⠉⠉⡧⠀⠙⠀⠀⣿⡀⢻⠀⠙⢢⣐⠛⠁⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⢠⢋⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⠿⠒⠋⠉⠀⣀⠤⠒⠀⠁⠀⢀⡴⠃⠀⠀⠀⠀⠈⢻⣮⡤⡰⠋⠈⠑⢦⡀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⣀⠤⡀⠀⠀⠀\n");
        printf("                                                         ⣯⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡜⠁⠀⠀⣀⣖⣉⣀⣀⣀⡀⠤⠖⠉⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⡁⠀⠀⠀⠀⠈⠳⡄⡀⢸⠀⠀⠀⢀⡠⢞⡡⠒⠁⠗⣶⣄⠀\n");
        printf("                                                         ⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣏⣀⣀⠴⠚⠻⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣆⠀⠀⣠⠤⠊⠉⠳⢼⠀⠀⡤⢋⠔⠋⠀⠀⠀⠀⢀⣼⡀\n");
        printf("                                                         ⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣾⣅⠀⠀⠀⠀⢸⢀⡞⡔⠁⠀⠀⠀⠀⠀⠀⠈⢛⡇\n");
        printf("                                                         ⣿⡅⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣷⣤⡀⡠⢼⢸⠀⣀⢀⡀⠀⠀⣀⡠⠶⠚⠉⠀\n");
        printf("                                                         ⢱⡹⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢷⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⢯⣙⢼⠀⣹⠃⠀⣷⠚⠉⠁⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠹⣝⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠻⠧⠄⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                      ⠀   ⠀⠈⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                             ⠀⠀⠀            ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣠⠴⠾⠃⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                         ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⡾⢟⡉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                             ⠀⠀⠀            ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢷⣄⠀⠀⠀⠀⠀⠀⠀⠀⣠⡞⠁⠀⠉⠓⢜⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                             ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀            ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠳⣦⡀⠀⠀⠀⠀⠞⠣⢻⠀⠀⠀⡀⡞⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                             ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀            ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⢶⣄⡀⠀⠀⠀⢸⠀⡄⡤⠕⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                             ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀            ⠀⠈⠛⢶⣤⣀⣾⠚⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀     \n");
    }
    else
    {
        system("clear");

        printf("\n\n\n            GGGGGGGGGGGGG                                                                                                                                                               \n");
        printf("         GGG::::::::::::G                                                                                                                                                               \n");
        printf("       GG:::::::::::::::G                                                                                                                                                               \n");
        printf("      G:::::GGGGGGGG::::G                                                                                                                                                               \n");
        printf("     G:::::G       GGGGGG  aaaaaaaaaaaaa      mmmmmmm    mmmmmmm       eeeeeeeeeeee                           ooooooooooo vvvvvvv           vvvvvvv eeeeeeeeeeee    rrrrr   rrrrrrrrr   \n");
        printf("    G:::::G                a::::::::::::a   mm:::::::m  m:::::::mm   ee::::::::::::ee                       oo:::::::::::oov:::::v         v:::::vee::::::::::::ee  r::::rrr:::::::::r  \n");
        printf("    G:::::G                aaaaaaaaa:::::a m::::::::::mm::::::::::m e::::::eeeee:::::ee                    o:::::::::::::::ov:::::v       v:::::ve::::::eeeee:::::eer:::::::::::::::::r \n");
        printf("    G:::::G    GGGGGGGGGG           a::::a m::::::::::::::::::::::me::::::e     e:::::e                    o:::::ooooo:::::o v:::::v     v:::::ve::::::e     e:::::err::::::rrrrr::::::r\n");
        printf("    G:::::G    G::::::::G    aaaaaaa:::::a m:::::mmm::::::mmm:::::me:::::::eeeee::::::e                    o::::o     o::::o  v:::::v   v:::::v e:::::::eeeee::::::e r:::::r     r:::::r\n");
        printf("    G:::::G    GGGGG::::G  aa::::::::::::a m::::m   m::::m   m::::me:::::::::::::::::e                     o::::o     o::::o   v:::::v v:::::v  e:::::::::::::::::e  r:::::r     rrrrrrr\n");
        printf("    G:::::G        G::::G a::::aaaa::::::a m::::m   m::::m   m::::me::::::eeeeeeeeeee                      o::::o     o::::o    v:::::v:::::v   e::::::eeeeeeeeeee   r:::::r            \n");
        printf("     G:::::G       G::::Ga::::a    a:::::a m::::m   m::::m   m::::me:::::::e                               o::::o     o::::o     v:::::::::v    e:::::::e            r:::::r            \n");
        printf("      G:::::GGGGGGGG::::Ga::::a    a:::::a m::::m   m::::m   m::::me::::::::e                              o:::::ooooo:::::o      v:::::::v     e::::::::e           r:::::r            \n");
        printf("       GG:::::::::::::::Ga:::::aaaa::::::a m::::m   m::::m   m::::m e::::::::eeeeeeee                      o:::::::::::::::o       v:::::v       e::::::::eeeeeeee   r:::::r            \n");
        printf("         GGG::::::GGG:::G a::::::::::aa:::am::::m   m::::m   m::::m  ee:::::::::::::e                       oo:::::::::::oo         v:::v         ee:::::::::::::e   r:::::r            \n");
        printf("            GGGGGG   GGGG  aaaaaaaaaa  aaaammmmmm   mmmmmm   mmmmmm    eeeeeeeeeeeeee                         ooooooooooo            vvv            eeeeeeeeeeeeee   rrrrrrr            \n\n\n\n\n\n\n");

        printf("                                                                                ⠀⠀⣀⣀⡤⠤⠤⠤⠤⣤⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⢰⣛⠉⠀⠀⠀⠀⠀⢀⡠⣽⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⢸⡈⠁⠒⠢⢤⣤⣔⠋⣰⠏⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⢀⣠⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⠘⣗⠦⣀⡴⠊⠁⠀⣩⡏⣀⡠⠴⠒⠂⠉⠉⠉⠀⣠⠤⠶⠒⠲⠭⣍⠉⠐⠲⠊⠁⢧⣆⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⠀⠹⣏⠀⠈⢙⡲⠿⠚⠉⠀⠀⠀⠀⠀⠀⠀⢀⢎⣴⢶⣦⣄⠀⠀⠀⠱⡀⠀⠀⠀⠈⠉⢙⡄⠤⠄⣀⠀⠀\n");
        printf("                                                                                ⠀⠀⠹⡖⡖⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⢸⣿⣾⣿⣿⠀⠀⠀⠀⢳⠀⠀⠀⠀⡴⠉⠀⢀⣤⣬⡣⡀\n");
        printf("                                                                                ⠀⠀⠀⢹⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⡈⠻⠿⠿⠋⠀⠀⠀⢀⡞⠀⠀⠀⠀⡇⠀⠀⢸⣿⣾⡇⡇\n");
        printf("                                                                                ⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠦⣀⣀⣀⣀⣀⠴⠋⠀⢀⡴⢹⠀⠱⣄⡀⠀⠉⣉⠜⠀\n");
        printf("                                                                                ⠀⠀⠀⠸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠔⠊⠀⠘⠒⠒⠀⠿⢭⣍⠁⠀⠀\n");
        printf("                                                                                ⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⣅⠒⠂⠀⠒⣶⠤⢀⣀⣀⡽⠀⠀⠀\n");
        printf("                                                                                ⠀⠀⠀⠀⢃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠀⠀⡆⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡸⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⠀⠀⠀⠀⠈⡆⠀⠀⡜⠉⠐⠒⢲⣦⣤⡤⣤⣀⢀⡏⠀⠀⢠⠀⢀⣀⡀⢤⠴⠒⡎⠁⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⠀⠀⠀⠀⠀⡗⠤⠜⠙⠦⣄⠀⠘⠷⣤⣀⣤⡾⠇⣇⠀⠀⡎⠉⠀⠀⠀⢸⠀⡄⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⠀⠀⠀⠀⠀⠙⢦⣄⣤⣰⠾⠇⠀⠀⠀⠀⠀⠀⠀⠸⡀⢠⡧⠀⠀⠀⠀⠈⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
        printf("                                                                                ⠀⠀⠀⠀⠀⠀⠀⠈⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠚⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀     \n\n\n\n");
    }
}

int main()
{
    srand((unsigned int)time(NULL));

    juego_t juego;

    inicializar_juego(&juego);

    signal(SIGINT, (void (*)(int)) manejo_sigint);

    int estado = estado_juego(juego);

    char accion = ' ';

    do
    {
        ejecutar_ciclo_juego_pantalla(juego);
        recibir_accion(&accion);
        validar_jugada(&accion, juego.perry);
        realizar_jugada(&juego, accion);
        estado = estado_juego(juego);
    } while (estado == JUEGO_EN_CURSO);

    dar_mensaje_final(juego, estado);

    return 0;
}
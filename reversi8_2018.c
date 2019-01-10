/*********************************************************************************************
* Fichero:		reversi8_2018.c
* Autores:		Yasmina Albero e Iván Escuín
* Descrip:		Ejecución y lógica del juego planteado para el trabajo, reversi8
* Version: 1.0 Placa
*********************************************************************************************/
#include "timer2.h"
#include "reversi8_2018.h"
#include "timer.h"
#include "timer0.h"
#include "latido.h"
#include "tp.h"
#include "botones_antirebotes.h"
#include "button.h"
#include "tp.h"
#include "lcd.h"
#include "lcd_funciones.h"
enum {MODO_C=0,MODO_ARM_C=1,MODO_ARM_ARM=2}; //Seleccion de funcion patron volteo que se va a usar: 0=patron_volteo, 1=patron_volteo_arm_c, 2=patron_volteo_arm_arm

// Valores que puede devolver la función patron_volteo())
enum {
	NO_HAY_PATRON = 0,
  PATRON_ENCONTRADO = 1
};

// Estados de las casillas del tablero
enum {
CASILLA_VACIA = 0,
FICHA_BLANCA = 1,
FICHA_NEGRA = 2,
FICHA_GRIS = 3
};
/*--- variables globales ---*/
unsigned int time;
	//Maquina de estados
typedef enum{inicio,nueva_partida,eleccion_casilla,t_cancelacion,jugada,fin_partida}
maquina_reversi;
 	//Estado actual de la maquina
maquina_reversi estado_main=inicio;
// candidatas: indica las posiciones a explorar
// Se usa para no explorar todo el tablero innecesariamente
// Sus posibles valores son NO, SI, CASILLA_OCUPADA
const char  NO              = 0;
const char  SI              = 1;
const char  CASILLA_OCUPADA = 2;

/////////////////////////////////////////////////////////////////////////////
// TABLAS AUXILIARES
// declaramos las siguientes tablas como globales para que sean más fáciles visualizarlas en el simulador
// __attribute__ ((aligned (8))): specifies a minimum alignment for the variable or structure field, measured in bytes, in this case 8 bytes

static const char __attribute__ ((aligned (8))) tabla_valor[DIM][DIM] =
{
    {8,2,7,3,3,7,2,8},
    {2,1,4,4,4,4,1,2},
    {7,4,6,5,5,6,4,7},
    {3,4,5,0,0,5,4,3},
    {3,4,5,0,0,5,4,3},
    {7,4,6,5,5,6,4,7},
    {2,1,4,4,4,4,1,2},
    {8,2,7,3,3,7,2,8}
};


// Tabla de direcciones. Contiene los desplazamientos de las 8 direcciones posibles
const char vSF[DIM] = {-1,-1, 0, 1, 1, 1, 0,-1};
const char vSC[DIM] = { 0, 1, 1, 1, 0,-1,-1,-1};

//////////////////////////////////////////////////////////////////////////////////////
// Variables globales que no deberían serlo
// tablero, fila, columna y ready son varibles que se deberían definir como locales dentro de reversi8.
// Sin embargo, las hemos definido como globales para que sea más fácil visualizar el tablero y las variables en la memoria
//////////////////////////////////////////////////////////////////////////////////////

int modo_patron_volteo=MODO_ARM_ARM;
int veces_patron_volteo=0;
unsigned int t_patron_volteo=0;
////////////////////////////////////////////////////////////////////
// Tablero sin inicializar
////////////////////////////////////////////////////////////////////
char __attribute__ ((aligned (8))) tablero[DIM][DIM] = {
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA}
	    };
////////////////////////////////////////////////////////////////////
// Tablero candidatas: se usa para no explorar todas las posiciones del tablero
// sólo se exploran las que están alrededor de las fichas colocadas
////////////////////////////////////////////////////////////////////
char __attribute__ ((aligned (8))) candidatas[DIM][DIM];
  ////////////////////////////////////////////////////////////////////
     // VARIABLES PARA INTERACCIONAR CON LA ENTRADA SALIDA
     // Pregunta: ¿hay que hacer algo con ellas para que esto funcione bien?
     // (por ejemplo añadir alguna palabra clave para garantizar que la sincronización a través de esa variable funcione)
 // char fila=0, columna=0, ready = 0;


//Funciones arm de patron volteo
extern int patron_volteo_arm_c(char tablero[][8], int *longitud,char f, char c, char SF, char SC, char color,int *veces);
extern int patron_volteo_arm_arm(char tablero[][8], int *longitud,char f, char c, char SF, char SC, char color,int *veces);
void reversi_main();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 0 indica CASILLA_VACIA, 1 indica FICHA_BLANCA y 2 indica FICHA_NEGRA
// pone el tablero a cero y luego coloca las fichas centrales.
void init_table(char tablero[][DIM], char candidatas[][DIM])
{
    int i, j;

    for (i = 0; i < DIM; i++)
    {
        for (j = 0; j < DIM; j++)
            tablero[i][j] = CASILLA_VACIA;
    }
    for (i = 0; i < DIM; i++)
    {
        for (j = 0; j < DIM; j++)
            candidatas[i][j] = NO;
    }
#if 0
    for (i = 3; i < 5; ++i) {
	for(j = 3; j < 5; ++j) {
	    tablero[i][j] = i == j ? FICHA_BLANCA : FICHA_NEGRA;
	}
    }

    for (i = 2; i < 6; ++i) {
	for (j = 2; j < 6; ++j) {
	    if((i>=3) && (i < 5) && (j>=3) && (j<5)) {
		candidatas[i][j] = CASILLA_OCUPADA;
	    } else {
		candidatas[i][j] = SI; //CASILLA_LIBRE;
	    }
	}
    }
#endif
    // arriba hay versión alternativa
    tablero[3][3] = FICHA_BLANCA;
    tablero[4][4] = FICHA_BLANCA;
    tablero[3][4] = FICHA_NEGRA;
    tablero[4][3] = FICHA_NEGRA;

    candidatas[3][3] = CASILLA_OCUPADA;
    candidatas[4][4] = CASILLA_OCUPADA;
    candidatas[3][4] = CASILLA_OCUPADA;
    candidatas[4][3] = CASILLA_OCUPADA;

    // casillas a explorar:
    candidatas[2][2] = SI;
    candidatas[2][3] = SI;
    candidatas[2][4] = SI;
    candidatas[2][5] = SI;
    candidatas[3][2] = SI;
    candidatas[3][5] = SI;
    candidatas[4][2] = SI;
    candidatas[4][5] = SI;
    candidatas[5][2] = SI;
    candidatas[5][3] = SI;
    candidatas[5][4] = SI;
    candidatas[5][5] = SI;
}

////////////////////////////////////////////////////////////////////////////////
// Espera a que ready valga 1.
// CUIDADO: si el compilador coloca esta variable en un registro, no funcionará.
// Hay que definirla como "volatile" para forzar a que antes de cada uso la cargue de memoria

void esperar_mov(char *ready)
{
    while (*ready == 0) {};  // bucle de espera de respuestas hasta que el se modifique el valor de ready (hay que hacerlo manualmente)

    *ready = 0;  //una vez que pasemos el bucle volvemos a fijar ready a 0;
}

////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// IMPORTANTE: AL SUSTITUIR FICHA_VALIDA() Y PATRON_VOLTEO()
// POR RUTINAS EN ENSAMBLADOR HAY QUE RESPETAR LA MODULARIDAD.
// DEBEN SEGUIR SIENDO LLAMADAS A FUNCIONES Y DEBEN CUMPLIR CON EL ATPCS
// (VER TRANSPARENCIAS Y MATERIAL DE PRACTICAS):
//  - DEBEN PASAR LOS PARAMETROS POR LOS REGISTROS CORRESPONDIENTES
//  - GUARDAR EN PILA SOLO LOS REGISTROS QUE TOCAN
//  - CREAR UN MARCO DE PILA TAL Y COMO MUESTRAN LAS TRANSPARENCIAS
//    DE LA ASIGNATURA (CON EL PC, FP, LR,....)
//  - EN EL CASO DE LAS VARIABLES LOCALES, SOLO HAY QUE APILARLAS
//    SI NO SE PUEDEN COLOCAR EN UN REGISTRO.
//    SI SE COLOCAN EN UN REGISTRO NO HACE FALTA
//    NI GUARDARLAS EN PILA NI RESERVAR UN ESPACIO EN LA PILA PARA ELLAS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////////////////////////////////////////////////////////////////////////////////
// Devuelve el contenido de la posición indicadas por la fila y columna actual.
// Además informa si la posición es válida y contiene alguna ficha.
// Esto lo hace por referencia (en *posicion_valida)
// Si devuelve un 0 no es válida o está vacia.
char ficha_valida(char tablero[][DIM], char f, char c, int *posicion_valida)
{
    char ficha;

    // ficha = tablero[f][c];
    // no puede accederse a tablero[f][c]
    // ya que algún índice puede ser negativo

    if ((f < DIM) && (f >= 0) && (c < DIM) && (c >= 0) && (tablero[f][c] != CASILLA_VACIA))
    {
        *posicion_valida = 1;
        ficha = tablero[f][c];
    }
    else
    {
        *posicion_valida = 0;
        ficha = CASILLA_VACIA;
    }
    return ficha;
}


////////////////////////////////////////////////////////////////////////////////
// La función patrón volteo es una función recursiva que busca el patrón de volteo
// (n fichas del rival seguidas de una ficha del jugador actual) en una dirección determinada
// SF y SC son las cantidades a sumar para movernos en la dirección que toque
// color indica el color de la pieza que se acaba de colocar
// la función devuelve PATRON_ENCONTRADO (1) si encuentra patrón y NO_HAY_PATRON (0) en caso contrario
// FA y CA son la fila y columna a analizar
// longitud es un parámetro por referencia. Sirve para saber la longitud del patrón que se está analizando. Se usa para saber cuantas fichas habría que voltear
int patron_volteo(char tablero[][DIM], int *longitud, char FA, char CA, char SF, char SC, char color,int *veces)
{
    int posicion_valida; // indica si la posición es valida y contiene una ficha de algún jugador
    int patron; //indica si se ha encontrado un patrón o no
    char casilla;   // casilla es la casilla que se lee del tablero
    FA = FA + SF;
    CA = CA + SC;
    casilla = ficha_valida(tablero, FA, CA, &posicion_valida);
    *veces = *veces + 1;
    // mientras la casilla está en el tablero, no está vacía,
    // y es del color rival seguimos buscando el patron de volteo
    if ((posicion_valida == 1) && (casilla != color))
    {
        *longitud = *longitud + 1;
        patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color,veces);
        //printf("longitud: %d \n", *longitud);
        //printf("fila: %d; columna: %d \n", FA, CA);
        return patron;
    }
    // si la ultima posición era válida y la ficha es del jugador actual,
    // entonces hemos encontrado el patrón
    else if ((posicion_valida == 1) && (casilla == color))
    {
        if (*longitud > 0) // longitud indica cuantas fichas hay que voltear
            {
            return PATRON_ENCONTRADO; // si hay que voltear una ficha o más hemos encontrado el patrón
            //printf("PATRON_ENCONTRADO \n");
            }
        else {
            return NO_HAY_PATRON; // si no hay que voltear no hay patrón
            //printf("NO_HAY_PATRON \n");
            }
    }
    // en caso contrario es que no hay patrón
    else
    {
        return NO_HAY_PATRON;
        //printf("NO_HAY_PATRON \n");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Funcion patron_volteo_test para comparar resultados entre las distintas implementaciones de patron_volteo.
//	Realiza mediciones de tiempo mediante el timer2 para cada caso de la funcion patron volteo y los almacena
//	en las variables time_c, time_arm_c y time_arm_arm para ser comprobados en debug. Estas variables deben ser
//	volatile para que puedan ser comprobadas en -o1 en adelante.
//
//	Si una de las funciones obtiene un resultado que no es igual a las demas la funcion se queda indefinidamente
//	en bucle.
// Parametros:
//		char tablero [][]: Array que contiene las fichas del juego
//		int* longitud: parametro por referencia que almacena la longitud del patron que se esta analizando
//		char FA: fila seleccionada
//		char CA: columna seleccionada
//		char SF: desplazamiento en la fila para buscar patron
//		char SC: desplazamiento en la columna para buscar patron
//		char color: tipo de ficha que ha iniciado la busqueda de patron
int patron_volteo_test(char tablero[][DIM], int *longitud, char FA, char CA, char SF, char SC, char color)
{

	unsigned int tiempos[6];	//Array para anotar tiempos del timer2
	volatile unsigned int time_c=0;
	volatile unsigned int time_arm_c=0;
	volatile unsigned int time_arm_arm=0;
	int veces;

	timer2_empezar();

	//Patron volteo c
	tiempos[0]=timer2_leer();
	int resultado_c=patron_volteo(tablero,longitud,FA,CA,SF,SC,color,&veces);
	tiempos[1]=timer2_leer();
	time_c=tiempos[1]-tiempos[0];

	//Patron volteo arm arm
	tiempos[4]=timer2_leer();
    int resultado_arm_arm=patron_volteo_arm_arm(tablero,longitud,FA,CA,SF,SC,color,&veces);
    tiempos[5]=timer2_leer();
    time_arm_arm=tiempos[5]-tiempos[4];

	//Patron volteo arm c
	tiempos[2]=timer2_leer();
	int resultado_arm_c=patron_volteo_arm_c(tablero,longitud,FA,CA,SF,SC,color,&veces);
	tiempos[3]=timer2_leer();
	time_arm_c=tiempos[3]-tiempos[2];

    timer2_parar();

    while(resultado_c != resultado_arm_c){}
    while(resultado_c != resultado_arm_arm){} //Breakpoint aqui
}

////////////////////////////////////////////////////////////////////////////////
// voltea n fichas en la dirección que toque
// SF y SC son las cantidades a sumar para movernos en la dirección que toque
// color indica el color de la pieza que se acaba de colocar
// FA y CA son la fila y columna a analizar
void voltear(char tablero[][DIM], char FA, char CA, char SF, char SC, int n, char color)
{
    int i;

    for (i = 0; i < n; i++)
    {
        FA = FA + SF;
        CA = CA + SC;
        tablero[FA][CA] = color;
    }
}
////////////////////////////////////////////////////////////////////////////////
// comprueba si hay que actualizar alguna ficha
// no comprueba que el movimiento realizado sea válido
// f y c son la fila y columna a analizar
// char vSF[DIM] = {-1,-1, 0, 1, 1, 1, 0,-1};
// char vSC[DIM] = { 0, 1, 1, 1, 0,-1,-1,-1};
int actualizar_tablero(char tablero[][DIM], char f, char c, char color, int modo_patron_volteo)
{
    char SF, SC; // cantidades a sumar para movernos en la dirección que toque
    int i, flip, patron;
    unsigned int t_patron[2]; //Variable para medir el tiempo empleado en patron_volteo

    for (i = 0; i < DIM; i++) // 0 es Norte, 1 NE, 2 E ...
    {
        SF = vSF[i];
        SC = vSC[i];
        // flip: numero de fichas a voltear
        flip = 0;
        switch (modo_patron_volteo) {
        case MODO_ARM_C:
        	t_patron[0]=timer2_leer();
        	patron = patron_volteo_arm_c(tablero, &flip, f, c, SF, SC, color,&veces_patron_volteo);
        	t_patron[1]=timer2_leer();
        	t_patron_volteo += t_patron[1]-t_patron[0];
        	break;
        case MODO_ARM_ARM:
        	t_patron[0]=timer2_leer();
            patron = patron_volteo_arm_arm(tablero, &flip, f, c, SF, SC, color,&veces_patron_volteo);
        	t_patron[1]=timer2_leer();
        	t_patron_volteo += t_patron[1]-t_patron[0];
            break;
        default:
        	t_patron[0]=timer2_leer();
        	patron = patron_volteo(tablero, &flip, f, c, SF, SC, color,&veces_patron_volteo);
        	t_patron[1]=timer2_leer();
        	t_patron_volteo += t_patron[1]-t_patron[0];
        	break;
        }
        //printf("Flip: %d \n", flip);
        if (patron == PATRON_ENCONTRADO )
        {
            voltear(tablero, f, c, SF, SC, flip, color);
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////
// Recorre todo el tablero comprobando en cada posición si se puede mover
// En caso afirmativo, consulta la puntuación de la posición y si es la mejor
// que se ha encontrado la guarda
// Al acabar escribe el movimiento seleccionado en f y c

// Candidatas
// NO    0
// SI    1
// CASILLA_OCUPADA 2
int elegir_mov(char candidatas[][DIM], char tablero[][DIM], char *f, char *c, int modo_patron_volteo)
{
    int i, j, k, found;
    int mf = -1; // almacena la fila del mejor movimiento encontrado
    int mc;      // almacena la columna del mejor movimiento encontrado
    char mejor = 0; // almacena el mejor valor encontrado
    int patron, longitud;
    char SF, SC; // cantidades a sumar para movernos en la dirección que toque
    unsigned int t_patron[2]; //Variable para medir el tiempo empleado en patron_volteo

    // Recorremos todo el tablero comprobando dónde podemos mover
    // Comparamos la puntuación de los movimientos encontrados y nos quedamos con el mejor
    for (i=0; i<DIM; i++)
    {
        for (j=0; j<DIM; j++)
        {   // indica en qué casillas quizá se pueda mover
            if (candidatas[i][j] == SI)
            {
                if (tablero[i][j] == CASILLA_VACIA)
                {
                    found = 0;
                    k = 0;

                    // en este bucle comprobamos si es un movimiento válido
                    // (es decir si implica voltear en alguna dirección)
                    while ((found == 0) && (k < DIM))
                    {
                        SF = vSF[k];    // k representa la dirección que miramos
                        SC = vSC[k];    // 1 es norte, 2 NE, 3 E ...

                        // nos dice qué hay que voltear en cada dirección
                        longitud = 0;

                        switch (modo_patron_volteo) {
                        case MODO_ARM_C:
                        	t_patron[0]=timer2_leer();
                        	patron = patron_volteo_arm_c(tablero, &longitud, i, j, SF, SC, FICHA_BLANCA,&veces_patron_volteo);
                        	t_patron[1]=timer2_leer();
                        	t_patron_volteo += t_patron[1]-t_patron[0];
                        	break;
                        case MODO_ARM_ARM:
                        	t_patron[0]=timer2_leer();
                        	patron = patron_volteo_arm_arm(tablero, &longitud, i, j, SF, SC, FICHA_BLANCA,&veces_patron_volteo);
                            t_patron[1]=timer2_leer();
							t_patron_volteo += t_patron[1]-t_patron[0];
                            break;
                        default:
                        	t_patron[0]=timer2_leer();
                        	patron = patron_volteo(tablero, &longitud, i, j, SF, SC, FICHA_BLANCA,&veces_patron_volteo);
                        	t_patron[1]=timer2_leer();
							t_patron_volteo += t_patron[1]-t_patron[0];
                        	break;
                        }
                        //  //printf("%d ", patron);
                        if (patron == PATRON_ENCONTRADO)
                        {
                            found = 1;
                            if (tabla_valor[i][j] > mejor)
                            {
                                mf = i;
                                mc = j;
                                mejor = tabla_valor[i][j];
                            }
                        }
                        k++;
                        // si no hemos encontrado nada probamos con la siguiente dirección
                    }
                }
            }
        }
    }
    *f = (char) mf;
    *c = (char) mc;
    // si no se ha encontrado una posición válida devuelve -1
    return mf;
}
////////////////////////////////////////////////////////////////////////////////
// Cuenta el número de fichas de cada color.
// Los guarda en la dirección b (blancas) y n (negras)
void contar(char tablero[][DIM], int *b, int *n)
{
    int i,j;

    *b = 0;
    *n = 0;

    // recorremos todo el tablero contando las fichas de cada color
    for (i=0; i<DIM; i++)
    {
        for (j=0; j<DIM; j++)
        {
            if (tablero[i][j] == FICHA_BLANCA)
            {
                (*b)++;
            }
            else if (tablero[i][j] == FICHA_NEGRA)
            {
                (*n)++;
            }
        }
    }
}

void actualizar_candidatas(char candidatas[][DIM], char f, char c)
{
    // donde ya se ha colocado no se puede volver a colocar
    // En las posiciones alrededor sí
    candidatas[f][c] = CASILLA_OCUPADA;
    if (f > 0)
    {
        if (candidatas[f-1][c] != CASILLA_OCUPADA)
            candidatas[f-1][c] = SI;

        if ((c > 0) && (candidatas[f-1][c-1] != CASILLA_OCUPADA))
            candidatas[f-1][c-1] = SI;

        if ((c < 7) && (candidatas[f-1][c+1] != CASILLA_OCUPADA))
            candidatas[f-1][c+1] = SI;
    }
    if (f < 7)
    {
        if (candidatas[f+1][c] != CASILLA_OCUPADA)
            candidatas[f+1][c] = SI;

        if ((c > 0) && (candidatas[f+1][c-1] != CASILLA_OCUPADA))
            candidatas[f+1][c-1] = SI;

        if ((c < 7) && (candidatas[f+1][c+1] != CASILLA_OCUPADA))
            candidatas[f+1][c+1] = SI;
    }
    if ((c > 0) && (candidatas[f][c-1] != CASILLA_OCUPADA))
        candidatas[f][c-1] = SI;

    if ((c < 7) && (candidatas[f][c+1] != CASILLA_OCUPADA))
        candidatas[f][c+1] = SI;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//	Funcion que inicializa el tablero para realizar los tests necesarios y lo restaura a su estado
//	original antes de empezar a jugar.
// 	Los casos de prueba realizados son:
//	· Tablero basico con ficha negra en 2,3. Usado para medir tiempos
//	· Tablero basico con ficha negra en 0,0.
//	· Tablero basico con ficha negra en 7,7.
//	· Tablero con diagonal, fichas negras en 0,0 y 5,5 y fichas blancas en (1,1),(2,2),(3,3) y (4,4)
// 	Parametros:
//		char tablero[][]: Array que contiene las fichas del tablero de juego, al finalizar la funcion
//						  se devuelve a su estado original tras llamar a init_table
//		char candidatas[][]: Array que contiene el estado de las casillas del tablero de juego,
//							 al finalizar se devuelve a su estado original tras llamar a init_table
int init_test(char tablero[][DIM],char candidatas[][DIM]){

	//Caso de prueba basico, tablero inicial con ficha negra en 2,3
	tablero[2][3]=FICHA_NEGRA;
	candidatas[2][3]=CASILLA_OCUPADA;
    int longitud=0;
    patron_volteo_test(tablero,&longitud,2,3,-1,0,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,2,3,1,0,FICHA_NEGRA); //Patron encontrado
    patron_volteo_test(tablero,&longitud,2,3,1,1,FICHA_NEGRA);	//Patron encontrado
    patron_volteo_test(tablero,&longitud,2,3,0,-1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,2,3,-1,-1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,2,3,0,1,FICHA_NEGRA);
    init_table(tablero,candidatas);

    //Caso de prueba, tablero inicial con ficha negra en 0,0. No se deberia encontrar patron.
	tablero[0][0]=FICHA_NEGRA;
	candidatas[0][0]=CASILLA_OCUPADA;
    longitud=0;
    patron_volteo_test(tablero,&longitud,0,0,-1,0,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,0,0,1,0,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,0,0,1,1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,0,0,0,-1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,0,0,-1,-1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,0,0,0,1,FICHA_NEGRA);
    init_table(tablero,candidatas);

    //Caso de prueba, tablero inicial con ficha negra en 7,7. No se deberia encontrar patron.
	tablero[7][7]=FICHA_NEGRA;
	candidatas[7][7]=CASILLA_OCUPADA;
    longitud=0;
    patron_volteo_test(tablero,&longitud,7,7,-1,0,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,7,7,1,0,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,7,7,1,1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,7,7,0,-1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,7,7,-1,-1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,7,7,0,1,FICHA_NEGRA);
    init_table(tablero,candidatas);

    //Caso de prueba, patron en diagonal con ficha negra en 0,0 y 5,5 y el resto fichas blancas
	tablero[0][0]=FICHA_NEGRA;
	candidatas[0][0]=CASILLA_OCUPADA;
	tablero[1][1]=FICHA_BLANCA;
	candidatas[1][1]=CASILLA_OCUPADA;
	tablero[2][2]=FICHA_BLANCA;
	candidatas[2][2]=CASILLA_OCUPADA;
	tablero[5][5]=FICHA_NEGRA;
	candidatas[5][5]=CASILLA_OCUPADA;
    longitud=0;
    patron_volteo_test(tablero,&longitud,0,0,1,0,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,0,0,0,1,FICHA_NEGRA);
    patron_volteo_test(tablero,&longitud,0,0,1,1,FICHA_NEGRA); //Patron encontrado
    init_table(tablero,candidatas);

}

//Funcion para comprobar si el usuario pasa
int comprobar_candidata(char fila, char columna,char c[][DIM]){
	return c[fila][columna]==SI;
}

////////////////////////////////////////////////////////////////////////////////
// Proceso principal del juego
// Utiliza el tablero,
// y las direcciones en las que indica el jugador la fila y la columna
// y la señal de ready que indica que se han actualizado fila y columna
// tablero, fila, columna y ready son variables globales aunque deberían ser locales de reversi8,
// la razón es que al meterlas en la pila no las pone juntas, y así jugar es más complicado.
// en esta versión el humano lleva negras y la máquina blancas
// no se comprueba que el humano mueva correctamente.
// Sólo que la máquina realice un movimiento correcto.
int reversi8_jugada(char fila_h, char col_h)
{
    int done;     // la máquina ha conseguido mover o no
    int move = 0; // el humano ha conseguido mover o no
    int blancas, negras; // número de fichas de cada color
	//Variables usadas en reversi8_jugada


    int fin = 0;  // fin vale 1 si el humano no ha podido mover
                  // (ha introducido un valor de movimiento con algún 8)
                  // y luego la máquina tampoco puede
    char f, c;    // fila y columna elegidas por la máquina para su movimiento
        // si la fila o columna son 8 asumimos que el jugador no puede mover
        //O si la posicion no es candidata
        int es_candidata = comprobar_candidata(fila_h,col_h,candidatas);
        if (((fila_h) != DIM) && ((col_h) != DIM) && es_candidata)
        {
            tablero[fila_h][col_h] = FICHA_NEGRA;
            actualizar_tablero(tablero, fila_h, col_h, FICHA_NEGRA, modo_patron_volteo);
            actualizar_candidatas(candidatas, fila_h, col_h);
            move = 1;
        }

        // escribe el movimiento en las variables globales fila columna
        done = elegir_mov(candidatas, tablero, &f, &c, modo_patron_volteo);
        if (done == -1)
        {
            //if (move == 0)
                fin = 1;
        }
        else
        {
            tablero[f][c] = FICHA_BLANCA;
            actualizar_tablero(tablero, f, c, FICHA_BLANCA, modo_patron_volteo);
            actualizar_candidatas(candidatas, f, c);
        }
        contar(tablero, &blancas, &negras);
        return fin;
}
void reversi8_init(){
   modo_patron_volteo = MODO_C;  //indica la funcion de patron_volteo que se va a usar para el juego.
   init_table(tablero, candidatas);
   t_patron_volteo=0;
   veces_patron_volteo=0;
   //init_test(tablero,candidatas);	//Inicia los tests automaticos
}

//Funcion que devuelve el tiempo acumulado en las llamadas a patron_volteo
unsigned int get_tiempo_patron_volteo(){
	return t_patron_volteo;
}

//Funcion que devuelve las veces que se ha invocado a patron_volteo
int get_veces_patron_volteo(){
	return veces_patron_volteo;
}

int final_partida(){
	int fin=1,i,j;
	for(i=0; i<DIM; i++){

		for(j=0;j<DIM;j++){
			if(tablero[i][j]==CASILLA_VACIA){
				fin=0;
			}
		}
	}
	return fin;
}

void reversi_main(){

	volatile int final=0; //indica el final de la partida

	int pintar_una_vez=0; //Flag de cosas que solo se pintan una vez
	int pintar_lcd=0; //Flag para realizar Dma_Trans
	int parpadeo_ficha=0; //Flag para realizar el parpadeo de ficha gris
	int timer_set_cancel=0; //Flag para setear el tiempo de cancelacion una sola vez
	unsigned int tiempo_patron_volteo=0; //Tiempo acumulado en las llamadas a patron_volteo
	unsigned int tiempo_calculos=0; //Tiempo acumulado en las llamadas a reversi8_jugada
	unsigned int t_calculos[2]; //Variables para medir tiempos de timer2
	int tiempo_juego=-1; //Tiempo total de juego, si -1 se desactiva
	veces_patron_volteo=0;

	int fila=0; //Fila de ficha elegida
	int columna=0; //Columna de ficha elegida
	Lcd_Clr();
	//Comienzo del bucle de juego
	while(1){
		//Compruebo interrupcion de latido
		latido_check();

		//Compruebo si se ha tocado un boton
		antirebotes_check();

		//Miramos el tiempo total de juego
		if(timer0_get(3)==0 && tiempo_juego!=-1 && estado_main!=fin_partida){
			//Pintar tiempo nuevo
			tiempo_juego++;
			Lcd_tiempo_total(tiempo_juego);
			pintar_lcd=1;
			timer0_set(3,50);
		}
		//Miramos el estado de la maquina
		switch(estado_main){
			case inicio:
				//Dibujar mensaje de entrada y esperar touchpad
				if(pintar_una_vez==0){
					Lcd_pantalla_inicio();
					pintar_lcd=1;
					pintar_una_vez=1;
				}
				if(hayToque()){
					setEspera_tp();
					pintar_una_vez=0;
					Lcd_Clr();
					estado_main=nueva_partida;
				}
				break;
			case nueva_partida:
				setEspera_tp();
				//Dibujar el trablero y el resto de la pantalla
				reversi8_init(); //Inicializa tableros
				Lcd_dibujarTablero(tablero); //Dibujo el tablero

				//Activo la cuenta del tiempo total de juego
				tiempo_juego=0;
				Lcd_tiempo_total(tiempo_juego);
				timer0_set(3,50);
				//Parpadeo de la ficha
				timer0_set(4,20);
				Lcd_pintar_ficha(fila,columna,FICHA_GRIS);
				pintar_lcd=1;
				estado_main=eleccion_casilla;
				break;
			case eleccion_casilla:
				Lcd_texto_jugar();

				//Miro el tablero para repintar

				if(get_estado_boton()==button_iz){
					//Muevo en columnas
					columna=get_elegido();
					Lcd_mover_ficha(tablero,fila,columna,FICHA_GRIS);
					pintar_lcd=1;
				}else if(get_estado_boton()==button_dr){
					//Muevo en filas
					fila=get_elegido();
					Lcd_mover_ficha(tablero,fila,columna,FICHA_GRIS);
					pintar_lcd=1;
				}
				//Dibujo parpadeo de ficha fila/columna
				if(timer0_get(4)==0 && parpadeo_ficha==1){

					//Si estoy encima de una ficha existente dibujo esa ficha
					Lcd_limpiar_casilla(fila,columna);
					if(tablero[fila][columna]==FICHA_NEGRA){
						Lcd_pintar_ficha(fila,columna,FICHA_NEGRA);
					}else if(tablero[fila][columna]==FICHA_BLANCA){
						Lcd_pintar_ficha(fila,columna,FICHA_BLANCA);
					}

					timer0_set(4,10);
					parpadeo_ficha=0;
					pintar_lcd=1;
				}else if(timer0_get(4)==0){
					//Si no dibujo la ficha en gris
					Lcd_limpiar_casilla(fila,columna);
					Lcd_pintar_ficha(fila,columna,FICHA_GRIS);
					timer0_set(4,50);
					parpadeo_ficha=1;
					pintar_lcd=1;

				}



			//Compruebo tp
			if(hayToque()){
				ULONG x_toque,y_toque;
				getXY(&x_toque,&y_toque);
				//Compruebo que se toca en el centro
				if(check_tp_centro(x_toque,y_toque)){
				DelayTime(8000);////
				estado_main=t_cancelacion;
				DelayTime(8000);
				}
				setEspera_tp();
			}
				break;
			case t_cancelacion:

				//Espero 2 segundos
				if(timer_set_cancel==0){
					timer0_set(2,100);
					timer_set_cancel=1;
					Lcd_texto_cancelar();
					pintar_lcd=1;
				}
				//Dibujo parpadeo de ficha fila/columna
				if(timer0_get(4)==0 && parpadeo_ficha){
					//Si estoy encima de una ficha existente dibujo esa ficha
					Lcd_limpiar_casilla(fila,columna);
					if(tablero[fila][columna]==FICHA_NEGRA){
						Lcd_pintar_ficha(fila,columna,FICHA_NEGRA);
					}else if(tablero[fila][columna]==FICHA_BLANCA){
						Lcd_pintar_ficha(fila,columna,FICHA_BLANCA);
					}
					timer0_set(4,20);
					parpadeo_ficha=0;
					pintar_lcd=1;
				}else if(timer0_get(4)==0){
					//Si no dibujo la ficha en gris
					Lcd_limpiar_casilla(fila,columna);
					Lcd_pintar_ficha(fila,columna,FICHA_GRIS);
					timer0_set(4,20);
					parpadeo_ficha=1;
					pintar_lcd=1;

				}
				//Compruebo tp
				if(hayToque()){
					timer_set_cancel=0;
					setEspera_tp();
					estado_main=eleccion_casilla;
				}
				//Compruebo botones
				else if(get_estado_boton()==button_iz || get_estado_boton()==button_dr){
					timer_set_cancel=0;
					estado_main=eleccion_casilla;
				}
				//Si han pasado los 2 segundos continuo
				else if(timer0_get(2)==0){
					timer_set_cancel=0;
					estado_main=jugada;
				}
				break;
			case jugada:
				t_calculos[0]=timer2_leer();
				final = reversi8_jugada(fila,columna); //Jugada
				t_calculos[1]=timer2_leer();
				//Calculo del tiempo acumulado en reversi8_jugada
				tiempo_calculos += t_calculos[1] - t_calculos[0];


//				//Obtengo el tiempo acumulado en patron_volteo de reversi8
//				tiempo_patron_volteo = t_patron_volteo;
//				//Obtengo el numero de llamadas de patron_volteo de reversi8
				int debug_veces_patron_volteo = veces_patron_volteo;


				//Actualizar tiempos y profiling
				Lcd_tiempo_acumulado(t_patron_volteo,tiempo_calculos,veces_patron_volteo);
				//Redibujo el tablero con la jugada de la maquina y la del humano
				Lcd_dibujarTablero(tablero);
				pintar_lcd=1;
				//Si el tablero esta acabado termino
				if(final==1){		//TODO: final partida
					estado_main=fin_partida;
				}else{
					fila=0;
					columna=0;
					reset_button_count();
					estado_main=eleccion_casilla;
				}
				break;
			case fin_partida:
				if(pintar_una_vez==0){
					Lcd_Clr();
					Lcd_dibujarTablero(tablero);
					Lcd_tiempo_total(tiempo_juego);
					Lcd_tiempo_acumulado(tiempo_patron_volteo,tiempo_calculos,veces_patron_volteo);
					Lcd_texto_fin();
					//Lcd_pantalla_inicio();
					pintar_una_vez=1;
					pintar_lcd=1;
				}
				if(hayToque()){
					setEspera_tp();
					final=0;
					pintar_una_vez=0;
					fila=0;
					columna=0;
					tiempo_juego=-1;
					tiempo_patron_volteo=0;
					tiempo_calculos=0;
					veces_patron_volteo=0;
					Lcd_Clr();
					pintar_lcd=1;
					estado_main=inicio;
				}

		}
		if(pintar_lcd==1){
			pintar_lcd=0;
			Lcd_Dma_Trans();

		}
	}
}

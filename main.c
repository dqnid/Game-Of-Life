#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #include <unistd.h>
    #include <curses.h>
#endif
#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
#endif

/*
 * Compilar con -lncurses
 * */

#define FIL 30
#define COL 30

void mostrarMatriz(int **matriz);
void inicioAleatorio(int **tablero, int num);
void turno(int **tablero, int **previo);
int comprobarVecinas(int **tablero, int i, int j);
void limpiar();
void liberarMatriz(int **matriz, int fil);
void oscilador(int **tablero);
void insertarSemilla(int ** tablero, int _x, int _y);

int nrows, ncols;

int main(void)
{
    int **tablero;
    int **previo;
    WINDOW *wnd;
    int i,j;
    char entrada, respuesta;
    tablero = (int**)malloc(sizeof(int*)*FIL);
    for (i=0; i<FIL; i++)
        tablero[i]=(int*)malloc(sizeof(int)*COL);
    for (i=0; i<FIL; i++)
    {
        for (j=0; j<COL; j++)
            tablero[i][j]=0;
    }    
    previo = (int**)malloc(sizeof(int*)*FIL);
    for (i=0; i<FIL; i++)
        previo[i]=(int*)malloc(sizeof(int)*COL);
    for (i=0; i<FIL; i++)
    {
        for (j=0; j<COL; j++)
            previo[i][j]=0;
    }   
    #if defined(__linux__) || defined (__unix__) || defined(__APPLE__) 
        wnd = initscr();
        cbreak();
        noecho();
        clear();
        refresh();
    #endif

    while (1)
    {
        printw("\n A) Cargar Oscilador Modelo\n B) Ejecutar\n C) Insertar Semilla\n Q) Salir\n >> ");
        respuesta = getch();
        switch(respuesta)
        {
            case 'a': case 'A':
                oscilador(tablero);
                break;
            case 'b': case 'B':
                while (1)
                {
                    mostrarMatriz(tablero);
                    entrada = getch();
                    turno(tablero, previo);
                    if (entrada=='q')
                        break;
                }
                break;
            case 'c': case 'C':
                insertarSemilla(tablero, COL, FIL);
                break;
            case 'q': case 'Q': 
                break;
            default:
                printw("\nOpción no válida\n");
        }
        if (respuesta=='q' || respuesta=='Q')
            break;
    }

    //Cierre
    for (i=0; i<FIL; i++)
        free(tablero[i]);    
    for (i=0; i<FIL; i++)
        free(previo[i]);
    free(tablero);
    free(previo);
    nocbreak();
    printf("\nFinal Existoso\n");
    endwin();
    return 0;   
}

void mostrarMatriz(int **matriz)
{
    int i,j;
    clear();
    move(0,0);
    printw("\n Matriz: (q para salir, cualquier tecla para avanzar)\n");
    for (i=0; i<FIL; i++)
    {
        for (j=0; j<COL; j++)
        {
            if (matriz[i][j]==1)
                printw(" O");
            else if (matriz[i][j]==0)
                printw(" -");
            else 
                printw(" *");
        }
        printw("\n");
    }    
}

/* 
 * Se podría modificar para admitir parámetros que hagan que algunas de las posiciones sean contiguas
 * de esta manera sería más propenso a generar patrones
 */
void inicioAleatorio(int **tablero, int num)
{
    int x,y;
    srand(time(NULL));
    for (int i=0; i<num; i++)
    {
        x=rand()%FIL;
        y=rand()%COL;
        tablero[x][y]=1;
        srand(x+y);
    }
}

//No usar FIL y COL, importar variables
void turno(int **tablero, int **previo)
{
    int i, j;
    int vivas_cercanas;
    
    for (i=0; i<FIL; i++)
    {
        for (j=0; j<COL; j++)
        {
            previo[i][j]=tablero[i][j];
        }
    }
    for (i=0; i<FIL; i++)
    {
        for (j=0; j<COL; j++)
        {
            if (previo[i][j]==0)
            {
                vivas_cercanas=comprobarVecinas(previo, i, j);
                if (vivas_cercanas==3)
                    tablero[i][j]=1;
            }else
            {
                vivas_cercanas=comprobarVecinas(previo, i, j);
                if (vivas_cercanas==3 || vivas_cercanas==2)
                    tablero[i][j]=1;
                else 
                    tablero[i][j]=0;
            }
        }
    }
}

int comprobarVecinas(int **tablero, int i, int j)
{
    int vivas_cercanas;
    vivas_cercanas=0;
    if (j<(COL-1))
    {
        if (tablero[i][j+1]==1)
            vivas_cercanas++;
    }
    if (j!=0)
    {
        if (tablero[i][j-1]==1)
            vivas_cercanas++;     
    }
    if (i!=0 && j!=0)
    {
        if (tablero[i-1][j-1]==1)
            vivas_cercanas++;         
    }
    if (i!=0 && j<(COL-1))
    {
        if (tablero[i-1][j+1]==1)
            vivas_cercanas++;   
    }
    if (i!=0)
    {
        if (tablero[i-1][j]==1)
            vivas_cercanas++;    
    }
    if (i<(FIL-1) && j<(COL-1))
    {
        if (tablero[i+1][j+1]==1)
            vivas_cercanas++; 
    }
    if (i<(FIL-1) && j!=0)
    {
        if (tablero[i+1][j-1]==1)
            vivas_cercanas++;        
    }
    if (i<(FIL-1))
    {
        if (tablero[i+1][j]==1)
            vivas_cercanas++;
    }
    return vivas_cercanas;
}

void limpiar(){
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}

void liberarMatriz(int **matriz, int fil)
{
    for (int i=0; i<fil; i++)
        free(matriz[i]);    
    free(matriz);
}

void oscilador(int **tablero)
{
    tablero[10][5]=1;
    tablero[10][6]=1;
    tablero[10][7]=1;
    tablero[11][6]=1;
    tablero[11][7]=1;
    tablero[12][7]=1;
    tablero[9][6]=1;
    tablero[9][7]=1;
    tablero[8][7]=1;
    tablero[10][14]=1;
    tablero[10][15]=1;
    tablero[10][16]=1;
    tablero[11][14]=1;
    tablero[11][15]=1;
    tablero[12][14]=1;
    tablero[9][14]=1;
    tablero[9][15]=1;
    tablero[8][14]=1;
}

void insertarSemilla(int **tablero, int _x, int _y)
{
    int **temporal;
    int valor_temporal, x, y, continuar, x_prev, y_prev, valor_prev;
    char respuesta;
    temporal = (int **)malloc(sizeof(int*)*FIL);
    for (int i=0; i<FIL; i++)
        temporal[i]=(int*)malloc(sizeof(int)*COL);
    for (int i=0; i<FIL; i++)
    {
        for (int j=0; j<COL; j++)
            temporal[i][j]=0;
    }
    valor_temporal = temporal[0][0];
    x = y = 0;
    continuar=1;
    while (continuar)
    {
        x_prev = x; y_prev = y; valor_prev = temporal[x_prev][y_prev];
        temporal[x][y]=2;
        clear();
        mostrarMatriz(temporal);
        respuesta = getch();
        if (respuesta=='q')
            continuar=0; //Se puede cambiar por un break que es más elegante
        else if (respuesta=='k' && x > 0)
            --x;
        else if (respuesta=='l' && y < (_y-1))
            ++y;
        else if (respuesta=='h' && y > 0)
            --y;
        else if (respuesta=='j' && x < (_x-1))
            ++x;
        else if (respuesta=='o')
        {
            temporal[x][y]=1;
            continue;
        }else if (respuesta=='i')
        {
            temporal[x][y]=0;
            continue;
        }
        temporal[x_prev][y_prev]=valor_prev;
    }
    printw("\n ¿Guardar? S / N");
    respuesta = getch();
    //Mejoras, cutre
    if (respuesta == 's' || respuesta == 'S')
    {
        for (int i=0; i<_x; i++)
        {
            for (int j=0; j<_y; j++)
                tablero[i][j] = temporal[i][j];
        }
        liberarMatriz(temporal, _x);
    }
}

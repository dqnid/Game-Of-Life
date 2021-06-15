#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <WinBase.h>
#include <time.h>

#define FIL 30
#define COL 30

void mostrarMatriz(char **matriz);
void inicioAleatorio(int num);
int comprobarVecinas(int i, int j);
void clear();
void oscilador();
DWORD WINAPI HiloIndividuo(LPVOID param);

SYNCHRONIZATION_BARRIER barrera_tablero;
SYNCHRONIZATION_BARRIER barrera_visualizar;

char **tablero;
bool continuar;


int main(void)
{
    int i,j;
    int info[2];
    char entrada;
    HANDLE hilos[FIL*COL];
    HANDLE hilo_mostrar;

    tablero = (char**)malloc(sizeof(char*)*FIL);
    for (i=0; i<FIL; i++)
        tablero[i]=(char*)malloc(sizeof(char)*COL);
    for (i=0; i<FIL; i++)
    {
        for (j=0; j<COL; j++)
            tablero[i][j]='-';
    }    

    InitializeSynchronizationBarrier(&barrera_tablero, FIL*COL, -1); 
    InitializeSynchronizationBarrier(&barrera_visualizar, FIL * COL, -1);

    oscilador();
    mostrarMatriz(tablero);
    scanf("%c", &entrada);

    for (i=0; i<FIL; i++)
    {
        for (j=0; j<COL; j++)
        {
            //inicio los hilos 
            info[0]=i;
            info[1]=j;
            hilos[(i+j)+(i*COL)]=CreateThread(NULL, 0, HiloIndividuo, info, 0, NULL);
        }
    }

    for (i=0; i<FIL; i++)
        free(tablero[i]);
    free(tablero);
    printf("\n Final Existoso");
    return 0;   
}

void mostrarMatriz(char **matriz)
{
    int i,j;
    clear();
    printf("\n Matriz: \n");
    for (i=0; i<FIL; i++)
    {
        for (j=0; j<COL; j++)
            printf(" %c", matriz[i][j]);
        printf("\n");
    }    
}

/* 
 * Se podría modificar para admitir parámetros que hagan que algunas de las posiciones sean contiguas
 * de esta manera sería más propenso a generar patrones
 */
void inicioAleatorio(int num)
{
    int x,y;
    srand(time(NULL));
    for (int i=0; i<num; i++)
    {
        x=rand()%FIL;
        y=rand()%COL;
        tablero[x][y]='O';
        srand(x+y);
    }
}

int comprobarVecinas(int i, int j)
{
    int vivas_cercanas;
    vivas_cercanas=0;
    if (j<(COL-1))
    {
        if (tablero[i][j+1]=='O')
            vivas_cercanas++;
    }
    if (j!=0)
    {
        if (tablero[i][j-1]=='O')
            vivas_cercanas++;     
    }
    if (i!=0 && j!=0)
    {
        if (tablero[i-1][j-1]=='O')
            vivas_cercanas++;         
    }
    if (i!=0 && j<(COL-1))
    {
        if (tablero[i-1][j+1]=='O')
            vivas_cercanas++;   
    }
    if (i!=0)
    {
        if (tablero[i-1][j]=='O')
            vivas_cercanas++;    
    }
    if (i<(FIL-1) && j<(COL-1))
    {
        if (tablero[i+1][j+1]=='O')
            vivas_cercanas++; 
    }
    if (i<(FIL-1) && j!=0)
    {
        if (tablero[i+1][j-1]=='O')
            vivas_cercanas++;        
    }
    if (i<(FIL-1))
    {
        if (tablero[i+1][j]=='O')
            vivas_cercanas++;
    }
    return vivas_cercanas;
}

void clear(){
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}

void oscilador()
{
    tablero[10][5]='O';
    tablero[10][6]='O';
    tablero[10][7]='O';
    tablero[11][6]='O';
    tablero[11][7]='O';
    tablero[12][7]='O';
    tablero[9][6]='O';
    tablero[9][7]='O';
    tablero[8][7]='O';
    tablero[10][14]='O';
    tablero[10][15]='O';
    tablero[10][16]='O';
    tablero[11][14]='O';
    tablero[11][15]='O';
    tablero[12][14]='O';
    tablero[9][14]='O';
    tablero[9][15]='O';
    tablero[8][14]='O';
}

/* 
 * Dos barreras:
 *  - Una para sincronizar los cambios en el tablero
 *  - Una para sincronizar la visualización del tablero (puede que un último proceso que lo haga)  
 */
DWORD CALLBACK HiloIndividuo(LPVOID param)
{
    char nuevo; 
    int* p;
    p = (int*)param;
    while (continuar)
    {
        if (tablero[p[0]][p[1]]=='O')
        {
            if (comprobarVecinas(p[0], p[1])==2 || comprobarVecinas(p[0], p[1])==3)
                nuevo = 'O';
            else 
                nuevo = '-';
        }else 
        {
            if (comprobarVecinas(p[0], p[1])==3)
                nuevo = 'O';
        }

        //Después de que todos llegues aquí
        if (EnterSynchronizationBarrier(&barrera_tablero, 0))
            printf("\nSincronizado\n");    
        tablero[p[0]][p[1]]=nuevo; 
        
        if (EnterSynchronizationBarrier(&barrera_visualizar, 0))
            mostrarMatriz(tablero);
    }
    return 0;
}
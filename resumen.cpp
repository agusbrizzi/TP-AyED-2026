#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include "struct.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;
using std::to_string;

//-----------------------[Declaración de funciones]-------------------------------
void corteControl();

//-----------------------[Main]-------------------------------

int main()
{

    return 0;
}

//-----------------------[Definición de funciones]-------------------------------

void corteControl()
{
    Comanda comandaActual;

    int idMozoActual;
    int productosMozo = 0;
    float comisionMozo = 0;
    int productosBuffet = 0;

    FILE *archivo = fopen("datos/comandas_semana.dat", "rb");

    if (archivo == NULL)
    {
        cout << "No se pudo abrir el archivo" << endl;
        return;
    }

    int leido = fread(&comandaActual, sizeof(Comanda), 1, archivo);

    while (leido == 1)
    {
        idMozoActual = comandaActual.idMozo;

        productosMozo = 0;
        comisionMozo = 0;

        while (leido == 1 && comandaActual.idMozo == idMozoActual)
        {
            productosMozo += comandaActual.cantidad;
            comisionMozo += comandaActual.comision;
            productosBuffet += comandaActual.cantidad;

            leido = fread(&comandaActual, sizeof(Comanda), 1, archivo);
        }

        cout << "Mozo: " << idMozoActual << endl;
        cout << "Productos vendidos: " << productosMozo << endl;
        cout << "Comision: " << comisionMozo << endl;

        cout << "Productos totales del Buffet: " << productosBuffet << endl;
    }

    fclose(archivo);
}

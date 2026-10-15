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
void nombreArchivoSemanal(char nombre[], int semana, int mes);
void corteControl(const char nombreArchivo[]);

//-----------------------[Main]-------------------------------

int main()
{
    int semana, mes;
    cout << "=== RESUMEN DEL CIERRE SEMANAL ===" << endl;
    cout << "Ingrese numero de semana (1-5) y mes (1-12): ";
    cin >> semana >> mes;

    char nombre[40];
    nombreArchivoSemanal(nombre, semana, mes);
    corteControl(nombre);
    return 0;
}

//-----------------------[Definición de funciones]-------------------------------

void nombreArchivoSemanal(char nombre[], int semana, int mes)
{
    sprintf(nombre, "comandas_semana_s%d-%02d.dat", semana, mes);
}

void corteControl(const char nombreArchivo[])
{
    FILE *archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL)
    {
        cout << "No se pudo abrir:" << nombreArchivo << endl;
        return;
    }

    Comanda c;
    int leido = fread(&c, sizeof(Comanda), 1, archivo);

    if (leido != 1)
    {
        cout << "La semana no tiene comandas registradas." << endl;
        fclose(archivo);
        return;
    }

    int productosBuffet = 0;

    while (leido == 1)
    {
        int idMozoActual = c.idMozo;
        int productosMozo = 0;
        float comisionMozo = 0;

        while (leido == 1 && c.idMozo == idMozoActual)
        {
            productosMozo += c.cantidad;
            comisionMozo += c.comision;

            leido = fread(&c, sizeof(Comanda), 1, archivo);
        }

        productosBuffet += productosMozo;

        cout << "Mozo: " << idMozoActual << endl;
        cout << "Productos vendidos: " << productosMozo << endl;
        cout << "Comision: " << comisionMozo << endl;
        cout << "--------------------------------------------" << endl;
    }

    fclose(archivo);
    cout << "TOTAL BUFFET - productos vendidos en la semana: " << productosBuffet << endl;
}
#include <iostream>
#include <string>
#include <cstdio>
#include "struct.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::to_string;

//-----------------------[Funciones de fecha]--------------------------
bool esBisiesto(int anio)
{
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

int diasDelMes(int mes, int anio)
{
    int dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes < 1 || mes > 12)
    {
        return 0;
    }
    if (mes == 2 && esBisiesto(anio))
    {
        return 29;
    }
    return dias[mes - 1];
}

void sumarUnDia(int &dia, int &mes, int &anio)
{
    dia++;
    if (dia > diasDelMes(mes, anio))
    {
        dia = 1;
        mes++;
        if (mes > 12)
        {
            mes = 1;
            anio++;
        }
    }
}

string formatearFecha(int dia, int mes, int anio)
{
    string d = (dia < 10 ? "0" : "") + to_string(dia);
    string m = (mes < 10 ? "0" : "") + to_string(mes);
    return d + "-" + m + "-" + to_string(anio);
}

string nombreArchivoComanda(int dia, int mes, int anio)
{
    return "comandas_" + formatearFecha(dia, mes, anio) + ".dat";
}

//-----------------------[Nombre del archivo semanal]------------------
string nombreArchivoSemanal(int dia, int mes)
{
    int semana = (dia - 1) / 7 + 1;
    string mm = (mes < 10 ? "0" : "") + to_string(mes);
    return "comandas_semana_s" + to_string(semana) + "-" + mm + ".dat";
}

//-----------------------[Ordenar planilla semanal]--------------------------

void ordenarPlanillaSemanal(const string &nombreArchivo)
{
    FILE *planilla = fopen(nombreArchivo.c_str(), "rb");

    if (planilla == NULL)
    {
        cout << "Error: no se pudo abrir la planilla semanal para ordenar." << endl;
        return;
    }

    fseek(planilla, 0, SEEK_END);
    long cantidadRegistros = ftell(planilla) / sizeof(Comanda);
    fseek(planilla, 0, SEEK_SET);

    if (cantidadRegistros <= 1)
    {
        fclose(planilla);
        return;
    }

    Comanda *comandas = new Comanda[cantidadRegistros];

    long leidos = fread(comandas, sizeof(Comanda), cantidadRegistros, planilla);
    fclose(planilla);

    if (leidos != cantidadRegistros)
    {
        cout << "Error al leer las comandas semanales." << endl;
        delete[] comandas;
        return;
    }

    for (long i = 0; i < cantidadRegistros - 1; i++)
    {
        for (long j = 0; j < cantidadRegistros - 1 - i; j++)
        {
            bool hayQueIntercambiar = false;

            if (comandas[j].idMozo > comandas[j + 1].idMozo)
            {
                hayQueIntercambiar = true;
            }
            else if (comandas[j].idMozo == comandas[j + 1].idMozo &&
                     comandas[j].codigoProducto > comandas[j + 1].codigoProducto)
            {
                hayQueIntercambiar = true;
            }

            if (hayQueIntercambiar)
            {
                Comanda aux = comandas[j];
                comandas[j] = comandas[j + 1];
                comandas[j + 1] = aux;
            }
        }
    }

    planilla = fopen(nombreArchivo.c_str(), "wb");

    if (planilla == NULL)
    {
        cout << "Error al reescribir la planilla semanal." << endl;
        delete[] comandas;
        return;
    }

    fwrite(comandas, sizeof(Comanda), cantidadRegistros, planilla);
    fclose(planilla);

    delete[] comandas;
}

int main()
{
    int dia, mes, anio;
    cout << "=== CIERRE SEMANAL DE COMANDAS ===\n";
    cout << "Ingrese la fecha del LUNES de la semana a cerrar (dia mes anio): ";
    cin >> dia >> mes >> anio;

    string nombreSemanal = nombreArchivoSemanal(dia, mes);

    FILE *test = fopen(nombreSemanal.c_str(), "rb");
    if (test != NULL)
    {
        fclose(test);
        char resp;
        cout << "Ya existe un cierre para esa semana. Reemplazarlo? (s/n): ";
        cin >> resp;
        if (resp != 's' && resp != 'S')
        {
            cout << "Cierre cancelado.\n";
            return 0;
        }
    }

    FILE *salida = fopen(nombreSemanal.c_str(), "wb");
    if (salida == NULL)
    {
        cout << "Error al crear el archivo semanal.\n";
        return 1;
    }

    int diasEncontrados = 0;

    FILE *dias[7];
    Comanda cabeza[7];
    int n = 0;

    int d = dia, m = mes, a = anio;
    for (int i = 0; i < 7; i++)
    {
        string nombreDia = nombreArchivoComanda(d, m, a);
        FILE *f = fopen(nombreDia.c_str(), "rb");

        if (f != NULL && fread(&cabeza[n], sizeof(Comanda), 1, f) == 1)
        {
            dias[n] = f;
            n++;
            diasEncontrados++;
            cout << "Dia " << formatearFecha(d, m, a) << ": encontrado\n";
        }
        else if (f != NULL)
        {
            fclose(f);
            cout << "Dia " << formatearFecha(d, m, a) << ": vacio, se omite\n";
        }
        else
        {
            cout << "Dia " << formatearFecha(d, m, a) << ": no hay archivo, se omite\n";
        }

        sumarUnDia(d, m, a);
    }

    int totalRegistros = 0;
    while (n > 0)
    {
        int menor = 0;
        for (int i = 1; i < n; i++)
            if (cabeza[i].idMozo < cabeza[menor].idMozo)
                menor = i;

        fwrite(&cabeza[menor], sizeof(Comanda), 1, salida);
        totalRegistros++;

        if (fread(&cabeza[menor], sizeof(Comanda), 1, dias[menor]) != 1)
        {
            fclose(dias[menor]);
            dias[menor] = dias[n - 1];
            cabeza[menor] = cabeza[n - 1];
            n--;
        }
    }

    fclose(salida);

    ordenarPlanillaSemanal(nombreSemanal);

    cout << "\n--- Resumen del cierre ---\n";
    cout << "Dias con datos encontrados: " << diasEncontrados << " de 7\n";
    cout << "Total de comandas consolidadas: " << totalRegistros << "\n";
    cout << "Archivo generado: " << nombreSemanal << "\n";

    return 0;
}
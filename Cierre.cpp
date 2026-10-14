#include <iostream>
#include <string>
#include <cstdio>
#include <fstream>
#include "../struct.hpp"

using namespace std;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::getline;
using std::to_string;


bool esBisiesto(int anio) 
{
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
};

int diasDelMes(int mes, int anio)
 {
    int dias[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (mes == 2 && esBisiesto(anio)) return 29;
    return dias[mes - 1];
}

void sumarUnDia(int &dia, int &mes, int &anio) {
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

bool archivoExiste(const string &nombre) 
{
    ifstream f(nombre, ios::binary);
    return f.good();
}

int main() 
{
    int dia, mes, anio;
    cout << "=== CIERRE SEMANAL DE COMANDAS ===\n";
    cout << "Ingrese la fecha del LUNES de la semana a cerrar (dia mes anio): ";
    cin >> dia >> mes >> anio;

    string nombreSemanal = "comandas_semanal.dat";
    if (archivoExiste(nombreSemanal))
    {
        cout << "Ya existe un archivo " << nombreSemanal << ".\n";
        cout << "¿Desea reemplazarlo por el cierre de esta nueva semana? (s/n): ";
        char resp;
        cin >> resp;
        if (resp != 's' && resp != 'S') 
        {
            cout << "Cierre cancelado.\n";
            return 0;
        }
    }

    ofstream salida(nombreSemanal, ios::binary | ios::trunc);
    if (!salida) {
        cout << "Error al crear el archivo semanal.\n";
        return 1;
    }

    int totalRegistros = 0;
    int diasEncontrados = 0;

    int d = dia, m = mes, a = anio;
    for (int i = 0; i < 7; i++) 
    {
        string nombreDia = nombreArchivoComanda(d, m, a);

        if (archivoExiste(nombreDia))
        {
            ifstream entrada(nombreDia, ios::binary);
            Comanda c;
            int contador = 0;

            while (entrada.read(reinterpret_cast<char*>(&c), sizeof(Comanda)))
            {
                salida.write(reinterpret_cast<char*>(&c), sizeof(Comanda));
                contador++;
            }
            entrada.close();

            cout << "Dia " << formatearFecha(d, m, a) << ": "
            << contador << " comanda(s) agregada(s).\n";

            totalRegistros += contador;
            diasEncontrados++;
        } else 
        {
            cout << "Dia " << formatearFecha(d, m, a)
            << ": no se encontro archivo, se omite.\n";
        }
        sumarUnDia(d, m, a);
    }
    salida.close();

    cout << "\n--- Resumen del cierre ---\n";
    cout << "Dias con datos encontrados: " << diasEncontrados << " de 7\n";
    cout << "Total de comandas consolidadas: " << totalRegistros << "\n";
    cout << "Archivo generado: " << nombreSemanal << "\n";

    return 0;
}

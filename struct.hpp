#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;
using std::to_string;

struct ComandaHistorica
{
    char fecha[11];      //"DD-MM-AAAA"
    char nombreMozo[50]; // el nombre del mozo
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Mozo
{
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Comanda
{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Producto
{
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

const float TASA_COMISION = 0.10f;
const int K = 5;

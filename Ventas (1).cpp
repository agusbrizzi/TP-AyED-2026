#include <iostream>
#include <string>
#include <cstdio>
#include "../struct.hpp"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::getline;
using std::to_string;

void obtenerNombreComanda(char nombreComanda[]);

void crearPlanilla(const char nombreComanda[]);

void encriptarClave(const char clave[], char claveEncriptada[]);

bool validarMozo(int idMozo, const char clave[]);

long buscarProducto(const char nombreInventario[], int codigoBuscado, Producto& producto);

void añadirVenta(const char nombreComanda[]);

void llenarPlanilla(const char nombreComanda[]);

void ordenarPlanilla(const char nombreComanda[]);

int main() {

    char nombreComanda[24];

    obtenerNombreComanda(nombreComanda);

    crearPlanilla(nombreComanda);

    llenarPlanilla(nombreComanda);

    ordenarPlanilla(nombreComanda);

    return 0;
}

void obtenerNombreComanda(char nombreComanda[]) {

    string dia, mes, anio;
    string nombreComandaAux;

    cout << "Ingrese el dia en formato dd: ";
    cin >> dia;

    cout << "Ingrese el mes en formato mm: ";
    cin >> mes;

    cout << "Ingrese el año en formato aaaa: ";
    cin >> anio;

    nombreComandaAux = "comandas_" + dia + "-" + mes + "-" + anio + ".dat";

    strcpy(nombreComanda, nombreComandaAux.c_str());
}

void crearPlanilla(const char nombreComanda[]) {

    FILE* planilla = fopen(nombreComanda, "ab");

    if (planilla == NULL) {
        cout << "Error: no se pudo crear/abrir la planilla." << endl;
        return;
    }

    fclose(planilla);
}

void encriptarClave(const char clave[], char claveEncriptada[]) {

    int i = 0;
    while (clave[i] != '\0'){
        claveEncriptada[i] = clave[i] + K;
        i++;
    }
    claveEncriptada[i] = '\0';
}

bool validarMozo(int idMozo, const char clave[]) {

    FILE* mozos = fopen("mozos.dat", "rb");

    if (mozos == NULL) {
        cout << "Error: no se pudo abrir mozos.dat." << endl;
        return false;
    }

    Mozo mozo;
    bool encontrado = false;

    while (!encontrado && fread(&mozo, sizeof(Mozo), 1, mozos) == 1) {
        if (mozo.idMozo == idMozo) {
            encontrado = true;
        }
    }

    fclose(mozos);

    if (!encontrado) {
        cout << "Error: el ID del mozo no existe." << endl;
        return false;
    }

    char claveEncriptada[20];
    encriptarClave(clave, claveEncriptada);

    if (strcmp(claveEncriptada, mozo.password) != 0) {
        cout << "Error: la clave es incorrecta." << endl;
        return false;
    }

    return true;
}

long buscarProducto(const char nombreInventario[], int codigoBuscado, Producto& producto) {

    FILE* inventario = fopen(nombreInventario, "rb");
    if (inventario == NULL) {
        cout << "Error: no se pudo abrir el inventario." << endl;
        return -1;
    }

    fseek(inventario, 0, SEEK_END);

    long cantidadRegistros = ftell(inventario) / sizeof(Producto);
    long primero = 0;
    long ultimo = cantidadRegistros - 1;
    long posicion = -1;

    while (primero <= ultimo && posicion == -1) {
        long medio = (primero + ultimo) / 2;

        fseek(inventario, medio * sizeof(Producto), SEEK_SET);

        if (fread(&producto, sizeof(Producto), 1, inventario) != 1) {
            fclose(inventario);
            return -1;
        }


        if (producto.codigo == codigoBuscado) {
            posicion = medio;
        } else if (codigoBuscado > producto.codigo) {
            primero = medio + 1;
        } else {
            ultimo = medio - 1;
        }
    }

    fclose(inventario);

    return posicion;
}

void añadirVenta(const char nombreComanda[]) {

    int idMozo;
    cout << "Ingrese ID del mozo: ";
    cin >> idMozo;

    char clave[20];
    cout << "Ingrese su clave: ";
    cin >> clave;

    if (!validarMozo(idMozo, clave)) {
        return;
    }

    int codigoProducto;

    cout << "Ingrese el codigo del producto: ";
    cin >> codigoProducto;

    Producto producto;
    long posicionProducto = buscarProducto("inventario.dat", codigoProducto, producto);

    if (posicionProducto == -1) {
        cout << "Error: el producto no existe." << endl;
        return;
    }

    int cantidad;
    cout << "Ingrese la cantidad: ";
    cin >> cantidad;

    if (cantidad <= 0) {
        cout << "Error: la cantidad debe ser mayor que cero." << endl;
        return;
    }

    if (producto.stockActual < cantidad) {
        cout << "Error: no hay suficiente stock." << endl;
        return;
    }

    Comanda nuevaVenta;

    nuevaVenta.idMozo = idMozo;
    nuevaVenta.codigoProducto = codigoProducto;
    nuevaVenta.cantidad = cantidad;

    nuevaVenta.comision = producto.precio * cantidad * TASA_COMISION;

    FILE* inventario =
        fopen("inventario.dat", "rb+");


    if (inventario == NULL) {

        cout << "Error: no se pudo abrir el inventario para actualizarlo."
             << endl;

        return;
    }

    fseek(inventario,
          posicionProducto * sizeof(Producto),
          SEEK_SET);


    producto.stockActual =
        producto.stockActual - cantidad;


    if (fwrite(&producto, sizeof(Producto), 1, inventario) != 1) {
        cout << "Error: no se pudo actualizar el inventario." << endl;
        fclose(inventario);
        return;
    }

    fclose(inventario);

    FILE* planilla = fopen(nombreComanda, "ab");

    if (planilla == NULL) {
        cout << "Error: no se pudo abrir la planilla." << endl;
        return;
    }

    if (fwrite(&nuevaVenta, sizeof(Comanda), 1, planilla) != 1) {

        cout << "Error: no se pudo guardar la venta." << endl;

        fclose(planilla);
        return;
    }

    fclose(planilla);
    cout << "Venta registrada correctamente." << endl;
    cout << "Comision generada: " << nuevaVenta.comision << endl;
}

void llenarPlanilla(const char nombreComanda[]) {

    int opcion = 1;

    while (opcion == 1) {

        cout << "1 para aniadir venta, 2 para terminar cargar:" << endl << "Opcion: " << endl;
        cin >> opcion;

        if (opcion == 1) {
            añadirVenta(nombreComanda);
        } else if (opcion == 2) {
            cout << "Registro cerrado." << endl;
        }
        else {

            cout << "Error: opcion invalida." << endl;
            opcion = 1;
        }
    }
}

void ordenarPlanilla(const char nombreComanda[]) {

    FILE* planilla = fopen(nombreComanda, "rb");
    if (planilla == NULL) {
        cout << "Error: no se pudo abrir la planilla para ordenar." << endl;
        return;
    }

    fseek(planilla, 0, SEEK_END);
    long cantidadRegistros = ftell(planilla) / sizeof(Comanda);

    fseek(planilla, 0, SEEK_SET);

    if (cantidadRegistros <= 1) {
        fclose(planilla);
        return;
    }

    Comanda* ventas = new Comanda[cantidadRegistros];

    fread(ventas, sizeof(Comanda), cantidadRegistros, planilla);
    fclose(planilla);

    for (long i = 0; i < cantidadRegistros - 1; i++) {
        for (long j = 0; j < cantidadRegistros - 1 - i; j++) {

            bool hayQueIntercambiar = false;

            if (ventas[j].idMozo > ventas[j + 1].idMozo) {
                hayQueIntercambiar = true;
            } else if (ventas[j].idMozo == ventas[j + 1].idMozo &&
                       ventas[j].codigoProducto > ventas[j + 1].codigoProducto) {
                hayQueIntercambiar = true;
            }

            if (hayQueIntercambiar) {
                Comanda aux = ventas[j];
                ventas[j] = ventas[j + 1];
                ventas[j + 1] = aux;
            }
        }
    }

    planilla = fopen(nombreComanda, "wb");
    if (planilla == NULL) {
        cout << "Error: no se pudo reescribir la planilla." << endl;
        delete[] ventas;
        return;
    }

    fwrite(ventas, sizeof(Comanda), cantidadRegistros, planilla);
    fclose(planilla);

    delete[] ventas;

    cout << "Planilla ordenada correctamente." << endl;
}
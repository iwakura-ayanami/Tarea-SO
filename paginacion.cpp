#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>


const unsigned long TAMANO_BLOQUE = 512;
const unsigned long TAMANO_RAM = 2 * 1024 * 1024; // 2 MB


struct Proceso {
    std::string nombre;
    unsigned long tamano; // en bytes
    unsigned long tiempo_acceso; // en milisegundos
    unsigned long tiempo_transferencia; // en milisegundos
    unsigned long tiempo_ejecucion; // en milisegundos
};


unsigned long convertir_a_bytes(unsigned long tamano, char unidad){
    switch (unidad) {
        case 'K': return tamano * 1024;
        case 'M': return tamano * 1024 * 1024;
        case 'G': return tamano * 1024 * 1024 * 1024;
        default: return tamano;
    }
}


void ingresar_datos(std::vector<Proceso>& procesos){
    int i = 1;
    for (auto& proceso : procesos) {
        std::cout << "Proceso " << i << std::endl;
        std::cout << "Ingrese el nombre del proceso: ";
        std::cin >> proceso.nombre;
        std::cout << "Ingrese el tamano del proceso: ";
        unsigned long tamano;
        char unidad;
        std::cin >> tamano;
        std::cout << "Ingrese el la unidad de medida (K, M o G): ";
        std::cin >> unidad;
        proceso.tamano = convertir_a_bytes(tamano, unidad);
        std::cout << "Ingrese el tiempo de acceso del proceso (en milisegundos): ";
        std::cin >> proceso.tiempo_acceso;
        std::cout << "Ingrese el tiempo de transferencia del proceso (en milisegundos): ";
        std::cin >> proceso.tiempo_transferencia;
        std::cout << "Ingrese el tiempo promedio de ejecucion del proceso (en milisegundos): ";
        std::cin >> proceso.tiempo_ejecucion;
        i++;
    }
}

std::vector<std::string> split(std::string str, char pattern) {
    
    int posInit = 0;
    int posFound = 0;
    std::string splitted;
    std::vector<std::string> results;
    
    while(posFound >= 0){
        posFound = str.find(pattern, posInit);
        splitted = str.substr(posInit, posFound - posInit);
        posInit = posFound + 1;
        results.push_back(splitted);
    }
    
    return results;
}

void ingresar_desde_archivo(std::vector<Proceso>& procesos, int num){
    std::ifstream read;
    read.open("procesos.txt", std::ios::out | std::ios::in);
    char ln[100];
    if(read.is_open()){    
        read.getline(ln, 100);
        for(int i = 0; i < num; i++){
            read.getline(ln, 100);
            std::vector<std::string> resultado = split(ln,  '|');
            procesos[i].nombre = resultado[0];
            char unidad = resultado[2][0];
            unsigned long tamano = std::stoi(resultado[1]);
            procesos[i].tamano = convertir_a_bytes(tamano, unidad);
            procesos[i].tiempo_acceso = std::stoi(resultado[3]);
            procesos[i].tiempo_ejecucion = std::stoi(resultado[4]);
            procesos[i].tiempo_transferencia = std::stoi(resultado[5]);
        }
    }
    else{
        std::cout << "¡Error! El archivo no pudo ser abierto." << std::endl;
    }
    read.close();
}


unsigned long calcular_fallas_pagina(unsigned long tamano){
    return (tamano / TAMANO_BLOQUE);
}


void calcular_tiempos(const std::vector<Proceso>& procesos){
    for (const auto& proceso : procesos) {
        unsigned long fallas_pagina = calcular_fallas_pagina(proceso.tamano);
        unsigned long turnaround_time = proceso.tiempo_acceso + proceso.tiempo_transferencia + proceso.tiempo_ejecucion;
        std::cout << "Proceso " << proceso.nombre << ":\n";
        std::cout << "  Fallas de Pagina: " << fallas_pagina << "\n";
        std::cout << "  Turnaround Time: " << turnaround_time << " ms\n";
    }
}


int main(){
    int num_procesos;
    std::cout << "Ingrese el numero de procesos (maximo 6): ";
    std::cin >> num_procesos;
    if (num_procesos > 6) {
        std::cout << "El numero maximo de procesos es 6.\n";
        return 1;
    }
    std::vector<Proceso> procesos(num_procesos);

    int num = 1;
    std::cout << "Que forma desea usar para ingresar los datos?(1.escrito, 2.archivo): ";
    std::cin >> num;

    if(num == 1)
        ingresar_datos(procesos);
    else if(num == 2)
        ingresar_desde_archivo(procesos, num_procesos);
    else return 1;
    calcular_tiempos(procesos);

    return 0;
}
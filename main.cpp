#include <iostream>
#include <string>
#include <vector>
#include <cmath>

struct Proceso {
    std::string nombre;
    double tamano;
    std::string unidad;
    int tiempoAcceso;
    int tiempoTransferencia;
    int tiempoEjecucion;
    int numPaginas;
    int tiempoFallas;
    int turnaroundTime;
};

double convertirABytes(double tamano, std::string unidad) {
    if (unidad == "B") return tamano;
    if (unidad == "KB") return tamano * 1024;
    if (unidad == "MB") return tamano * 1024 * 1024;
    if (unidad == "GB") return tamano * 1024 * 1024 * 1024;
    return 0;
}

int calcularNumeroPaginas(double tamanoBytes, int tamanoBloque) {
    return std::ceil(tamanoBytes / tamanoBloque);
}

void distribucionProporcional(std::vector<Proceso>& procesos, int memoriaTotal, int tamanoBloque) {
    int totalPaginas = 0;
    for (auto& p : procesos) {
        totalPaginas += p.numPaginas;
    }
    
    int paginasDisponibles = memoriaTotal / tamanoBloque;
    
    for (auto& p : procesos) {
        double proporcion = (double)p.numPaginas / totalPaginas;
        p.numPaginas = std::round(proporcion * paginasDisponibles);
    }
}

int calcularTiempoFallas(Proceso& p, int tamanoRAM, int tamanoBloque) {
    int paginasRAM = tamanoRAM / tamanoBloque;
    double probabilidadFallo = (double)(p.numPaginas - paginasRAM) / p.numPaginas;
    if (probabilidadFallo < 0) probabilidadFallo = 0;
    return std::round(p.numPaginas * probabilidadFallo * (p.tiempoAcceso + p.tiempoTransferencia));
}

int calcularTurnaroundTime(Proceso& p) {
    return p.tiempoFallas + p.tiempoEjecucion;
}

int main() {
    const int RAM_SIZE = 2 * 1024 * 1024; // 2MB en bytes
    const int BLOCK_SIZE = 512; // bytes
    const int MAX_PROCESOS = 6;
    
    std::vector<Proceso> procesos;
    
    int numProcesos;
    std::cout << "Ingrese el número de procesos (máximo 6): ";
    std::cin >> numProcesos;
    
    if (numProcesos > MAX_PROCESOS) {
        std::cout << "Número de procesos excede el máximo permitido." << std::endl;
        return 1;
    }
    
    for (int i = 0; i < numProcesos; i++) {
        Proceso p;
        std::cout << "\nProceso " << i+1 << ":" << std::endl;
        std::cout << "Nombre: ";
        std::cin >> p.nombre;
        std::cout << "Tamaño: ";
        std::cin >> p.tamano;
        std::cout << "Unidad (B/KB/MB/GB): ";
        std::cin >> p.unidad;
        std::cout << "Tiempo de acceso (ms): ";
        std::cin >> p.tiempoAcceso;
        std::cout << "Tiempo de transferencia (ms): ";
        std::cin >> p.tiempoTransferencia;
        std::cout << "Tiempo promedio de ejecución (ms): ";
        std::cin >> p.tiempoEjecucion;
        
        double tamanoBytes = convertirABytes(p.tamano, p.unidad);
        p.numPaginas = calcularNumeroPaginas(tamanoBytes, BLOCK_SIZE);
        
        procesos.push_back(p);
    }
    
    distribucionProporcional(procesos, RAM_SIZE, BLOCK_SIZE);
    
    for (auto& p : procesos) {
        p.tiempoFallas = calcularTiempoFallas(p, RAM_SIZE, BLOCK_SIZE);
        p.turnaroundTime = calcularTurnaroundTime(p);
    }
    
    std::cout << "\nResultados:" << std::endl;
    for (const auto& p : procesos) {
        std::cout << "Proceso: " << p.nombre << std::endl;
        std::cout << "Número de páginas: " << p.numPaginas << std::endl;
        std::cout << "Tiempo de Fallas: " << p.tiempoFallas << " ms" << std::endl;
        std::cout << "TurnaRound Time: " << p.turnaroundTime << " ms" << std::endl;
        std::cout << std::endl;
    }
    
    return 0;
}
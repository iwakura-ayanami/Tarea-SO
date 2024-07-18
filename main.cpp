#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <iomanip>

struct Proceso {
    std::string nombre;
    double tamano;
    std::string unidad;
    int numPaginas;
    int marcosAsignados;
    double fragmentacionInterna;
    int tiempoAcceso;
    int tiempoTransferencia;
    int tiempoEjecucion;
    int tiempoFallas;
    int turnaroundTime;
};

// const int RAM_SIZE = 2 * 1024 * 1024; // 2MB en bytes
const int RAM_SIZE = 1 * 1024 * 1024 * 1024; // 2MB en bytes
//const int BLOCK_SIZE = 512; // bytes
const int BLOCK_SIZE = 4096; // bytes
const int MAX_PROCESOS = 6;

std::vector<Proceso> procesos;

double convertirABytes(double tamano, std::string unidad) {
    if (unidad == "b") return tamano / 8.0;
    if (unidad == "B") return tamano;
    if (unidad == "KB") return tamano * 1024;
    if (unidad == "MB") return tamano * 1024 * 1024;
    if (unidad == "GB") return tamano * 1024 * 1024 * 1024;
    return 0;
}

int calcularNumeroPaginas(double tamanoBytes, int tamanoBloque) {
    return std::ceil(tamanoBytes / tamanoBloque);
}

void distribucionProporcional() {
    int totalPaginas = 0;
    for (auto& p : procesos) {
        totalPaginas += p.numPaginas;
    }
    
    int marcosDisponibles = RAM_SIZE / BLOCK_SIZE;
    
    for (auto& p : procesos) {
        double proporcion = (double)p.numPaginas / totalPaginas;
        p.marcosAsignados = std::round(proporcion * marcosDisponibles);
    }
}

void ingresarProcesos() {
    int numProcesos;
    std::cout << "Ingrese el número de procesos (máximo " << MAX_PROCESOS << "): ";
    std::cin >> numProcesos;
    
    if (numProcesos > MAX_PROCESOS) {
        std::cout << "Número de procesos excede el máximo permitido." << std::endl;
        return;
    }
    
    procesos.clear();  // Limpiar procesos existentes
    
    for (int i = 0; i < numProcesos; i++) {
        Proceso p;
        std::cout << "\nProceso " << i+1 << ":" << std::endl;
        std::cout << "Nombre: ";
        std::cin >> p.nombre;
        std::cout << "Tamaño: ";
        std::cin >> p.tamano;
        std::cout << "Unidad (b/B/KB/MB/GB): ";
        std::cin >> p.unidad;
        
        double tamanoBytes = convertirABytes(p.tamano, p.unidad);
        p.numPaginas = calcularNumeroPaginas(tamanoBytes, BLOCK_SIZE);
        p.fragmentacionInterna = (p.numPaginas * BLOCK_SIZE) - tamanoBytes;
        
        procesos.push_back(p);
    }
    
    distribucionProporcional();
}

void mostrarPaginasYMarcos() {
    int totalPaginas = 0;
    int totalMarcos = RAM_SIZE / BLOCK_SIZE;
    double fragmentacionInternaTotal = 0;
    int marcosTotalesAsignados = 0;

    std::cout << "\nPáginas, Marcos y Fragmentación por Proceso:" << std::endl;
    std::cout << std::left << std::setw(10) << "Proceso" 
              << std::setw(20) << "Número de Páginas" 
              << std::setw(20) << "Marcos Asignados"
              << std::setw(25) << "Fragmentación Interna (B)" << std::endl;
    std::cout << std::string(75, '-') << std::endl;

    for (const auto& p : procesos) {
        std::cout << std::left << std::setw(10) << p.nombre 
                  << std::setw(20) << p.numPaginas 
                  << std::setw(20) << p.marcosAsignados
                  << std::setw(25) << std::fixed << std::setprecision(2) << p.fragmentacionInterna << std::endl;
        totalPaginas += p.numPaginas;
        fragmentacionInternaTotal += p.fragmentacionInterna;
        marcosTotalesAsignados += p.marcosAsignados;
    }

    std::cout << std::string(75, '-') << std::endl;
    std::cout << "Total de páginas: " << totalPaginas << std::endl;
    std::cout << "Total de marcos en memoria: " << totalMarcos << std::endl;
    std::cout << "Fragmentación interna total: " << std::fixed << std::setprecision(2) << fragmentacionInternaTotal << " bytes" << std::endl;
    
    int diferenciaMarcos = totalMarcos - marcosTotalesAsignados;
    double fragmentacionTotal = (diferenciaMarcos * BLOCK_SIZE) + fragmentacionInternaTotal;
    
    std::cout << "Diferencia de marcos no asignados: " << diferenciaMarcos << std::endl;
    std::cout << "Fragmentación total: " << std::fixed << std::setprecision(2) << fragmentacionTotal << " bytes" << std::endl;
}

void ingresarTiempos() {
    for (auto& p : procesos) {
        std::cout << "\nProceso: " << p.nombre << std::endl;
        std::cout << "Tiempo de acceso (ms): ";
        std::cin >> p.tiempoAcceso;
        std::cout << "Tiempo de transferencia (ms): ";
        std::cin >> p.tiempoTransferencia;
        std::cout << "Tiempo promedio de ejecución (ms): ";
        std::cin >> p.tiempoEjecucion;
    }
}

void calcularTiempos() {
    for (auto& p : procesos) {
        double probabilidadFallo = std::max(0.0, (double)(p.numPaginas - p.marcosAsignados) / p.numPaginas);
        p.tiempoFallas = std::round(p.numPaginas * probabilidadFallo * (p.tiempoAcceso + p.tiempoTransferencia));
        p.turnaroundTime = p.tiempoFallas + p.tiempoEjecucion;
    }
}

void mostrarResultados() {
    std::cout << "\nResultados:" << std::endl;
    std::cout << std::left << std::setw(10) << "Proceso" 
              << std::setw(20) << "Tiempo de Fallas (ms)" 
              << std::setw(20) << "Turnaround Time (ms)" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (const auto& p : procesos) {
        std::cout << std::left << std::setw(10) << p.nombre 
                  << std::setw(20) << p.tiempoFallas 
                  << std::setw(20) << p.turnaroundTime << std::endl;
    }
}

void limpiarBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    int opcion;
    bool procesosCargados = false;
    bool tiemposCargados = false;

    do {
        std::cout << "\nMenú:" << std::endl;
        std::cout << "1. Ingresar procesos" << std::endl;
        std::cout << "2. Mostrar páginas y marcos por proceso" << std::endl;
        std::cout << "3. Ingresar tiempos de acceso, transferencia y ejecución" << std::endl;
        std::cout << "4. Calcular y mostrar resultados" << std::endl;
        std::cout << "5. Salir" << std::endl;
        std::cout << "Elija una opción: ";
        std::cin >> opcion;

        limpiarBuffer();

        switch(opcion) {
            case 1:
                ingresarProcesos();
                procesosCargados = true;
                tiemposCargados = false;  // Reiniciar si se ingresan nuevos procesos
                break;
            case 2:
                if (!procesosCargados) {
                    std::cout << "Primero debe ingresar los procesos." << std::endl;
                } else {
                    mostrarPaginasYMarcos();
                }
                break;
            case 3:
                if (!procesosCargados) {
                    std::cout << "Primero debe ingresar los procesos." << std::endl;
                } else {
                    ingresarTiempos();
                    tiemposCargados = true;
                }
                break;
            case 4:
                if (!procesosCargados || !tiemposCargados) {
                    std::cout << "Debe ingresar los procesos y los tiempos primero." << std::endl;
                } else {
                    calcularTiempos();
                    mostrarResultados();
                }
                break;
            case 5:
                std::cout << "Saliendo del programa..." << std::endl;
                break;
            default:
                std::cout << "Opción no válida. Intente de nuevo." << std::endl;
        }
    } while (opcion != 5);

    return 0;
}
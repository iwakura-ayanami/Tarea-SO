#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>
#include <iomanip>

struct Proceso {
    std::string nombre;
    double tamano;
    std::string unidad;
    int numPaginas;
    int marcosAsignados;
    float tiempoAcceso;
    float tiempoTransferencia;
    float tiempoEjecucion;
    float tiempoFallas;
    float turnaroundTime;
};
void imprimir();

// const int RAM_SIZE = 2 * 1024 * 1024; // 2MB en bytes
const int RAM_SIZE = 20 * 1024 * 1024; // 2MB en bytes
//const int BLOCK_SIZE = 512; // bytes
const int BLOCK_SIZE = 4096; // bytes
const int MAX_PROCESOS = 6;
int numProcesos = 3;

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
    float totalPaginas = 0;
    for (auto& p : procesos) {
        totalPaginas += p.numPaginas;
    }
    
    int marcosDisponibles = RAM_SIZE / BLOCK_SIZE;
    
    for (auto& p : procesos) {
        double proporcion = (double)p.numPaginas / totalPaginas;
        if(proporcion * marcosDisponibles < 1.0)
            p.marcosAsignados = 1;
        else
            p.marcosAsignados = std::floor(proporcion * marcosDisponibles);
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
                  << std::setw(20) << p.marcosAsignados << std::endl;
        totalPaginas += p.numPaginas;
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


void ingresar_desde_archivo(){
    std::ifstream read;
    read.open("procesos.csv", std::ios::in);
    std::string line, word;
    Proceso p;
    getline(read, line);
    std::vector<std::string> fila;
     while(read.is_open()){
        fila.clear();
        getline(read, line);
        std::stringstream s(line);
        if(line == "")break;
        while (getline(s, word, ',')) {
            fila.push_back(word);
        }
        fila.push_back(word);

        p.nombre = fila[0];
        p.unidad = fila[2];
        double tamano = std::stod(fila[1]);
        p.tamano = convertirABytes(tamano, p.unidad);
        p.tiempoAcceso = std::stof(fila[3]);
        p.tiempoEjecucion = std::stof(fila[4]);
        p.tiempoTransferencia = std::stof(fila[5]);
        p.numPaginas = calcularNumeroPaginas(p.tamano, BLOCK_SIZE);
        
        procesos.push_back(p);
    }
    read.close();
}

void CalcularCargasDescargas(int pags, int marcos, int &vecesCompletas, int &sobrantes){
  int aux = pags/marcos;
  aux += (aux-1);
  vecesCompletas = aux;
  sobrantes = pags % marcos;
}
void calcularTiempos() {
    for (auto& p : procesos) {
        if(p.numPaginas < p.marcosAsignados)
          {
            p.tiempoFallas = p.numPaginas * (p.tiempoAcceso + p.tiempoTransferencia);
          }
        else{
           int a, b;
           CalcularCargasDescargas(p.numPaginas, p.marcosAsignados, a, b);
           p.tiempoFallas = ((p.marcosAsignados * a) + (b*2)) * (p.tiempoAcceso + p.tiempoTransferencia);
          }
        p.turnaroundTime = p.tiempoFallas + (p.tiempoEjecucion*p.numPaginas);
        
    }
    std::cout << "hola";
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

void imprimir(){
    for(Proceso pro : procesos){
        std::cout << pro.nombre << std::endl;
        std::cout << pro.tamano << std::endl;
        std::cout << pro.tiempoAcceso << std::endl;
        std::cout << pro.tiempoEjecucion << std::endl;
        std::cout << pro.tiempoTransferencia << std::endl;
        std::cout << "\n" << std::endl;
    }
}

int main() {
    ingresar_desde_archivo();
    distribucionProporcional();
    calcularTiempos();
    mostrarResultados();
    system("PAUSE");
    return 0;
}

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


// const int RAM_SIZE = 2 * 1024 * 1024; // 2MB en bytes
const int RAM_SIZE = 20 * 1024 * 1024; // 20MB en bytes
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
        p.tiempoEjecucion = std::stof(fila[5]);
        p.tiempoTransferencia = std::stof(fila[4]);
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
           int veces_completas, pags_sobrantes;
           CalcularCargasDescargas(p.numPaginas, p.marcosAsignados, veces_completas, pags_sobrantes);

           p.tiempoFallas = p.marcosAsignados * veces_completas;
           p.tiempoFallas += pags_sobrantes*2;
           p.tiempoFallas *= (p.tiempoAcceso + p.tiempoTransferencia);
           
          }
        p.turnaroundTime = p.tiempoFallas + (p.tiempoEjecucion*p.numPaginas);
        
    }
}

void mostrarResultados() {
    std::cout << "\nResultados:" << std::endl;
    std::cout << std::left << std::setw(10) << "Proceso" 
              << std::setw(20) << "Tiempo de Fallas (ms)" 
              << std::setw(20) << "Turn around Time(ms)"
              << std::setw(20) << "paginas " 
              << std::setw(20) << "marcos" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (const auto& p : procesos) {
        std::cout << std::left << std::setw(10) << p.nombre 
                  << std::setw(20) << p.tiempoFallas 
                  << std::setw(20) << p.turnaroundTime
                  << std::setw(20) <<p.numPaginas 
                  << std::setw(20) <<p.marcosAsignados << std::endl;
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

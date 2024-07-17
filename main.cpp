#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct Proceso {
    string nombre;
    double tamano;
    string unidad;
    int tiempoAcceso;
    int tiempoTransferencia;
    int tiempoEjecucion;
    int numPaginas;
    int tiempoFallas;
    int turnaroundTime;
};

double convertirABytes(double tamano, string unidad) {
    if (unidad == "B") return tamano;
    if (unidad == "KB") return tamano * 1024;
    if (unidad == "MB") return tamano * 1024 * 1024;
    if (unidad == "GB") return tamano * 1024 * 1024 * 1024;
    return 0;
}

int calcularNumeroPaginas(double tamanoBytes, int tamanoBloque) {
    return ceil(tamanoBytes / tamanoBloque);
}

void distribucionProporcional(vector<Proceso>& procesos, int memoriaTotal) {
    int totalPaginas = 0;
    for (auto& p : procesos) {
        totalPaginas += p.numPaginas;
    }
    
    for (auto& p : procesos) {
        double proporcion = (double)p.numPaginas / totalPaginas;
        p.numPaginas = round(proporcion * (memoriaTotal / 512));
    }
}

int calcularTiempoFallas(Proceso& p) {
    return p.numPaginas * (p.tiempoAcceso + p.tiempoTransferencia);
}

int calcularTurnaroundTime(Proceso& p) {
    return p.tiempoFallas + p.tiempoEjecucion;
}

int main() {
    const int RAM_SIZE = 2 * 1024 * 1024; // 2MB en bytes
    const int BLOCK_SIZE = 512; // bytes
    const int MAX_PROCESOS = 6;
    
    vector<Proceso> procesos;
    
    int numProcesos;
    cout << "Ingrese el número de procesos (máximo 6): ";
    cin >> numProcesos;
    
    if (numProcesos > MAX_PROCESOS) {
        cout << "Número de procesos excede el máximo permitido." << endl;
        return 1;
    }
    
    for (int i = 0; i < numProcesos; i++) {
        Proceso p;
        cout << "\nProceso " << i+1 << ":" << endl;
        cout << "Nombre: ";
        cin >> p.nombre;
        cout << "Tamaño: ";
        cin >> p.tamano;
        cout << "Unidad (B/KB/MB/GB): ";
        cin >> p.unidad;
        cout << "Tiempo de acceso (ms): ";
        cin >> p.tiempoAcceso;
        cout << "Tiempo de transferencia (ms): ";
        cin >> p.tiempoTransferencia;
        cout << "Tiempo promedio de ejecución (ms): ";
        cin >> p.tiempoEjecucion;
        
        double tamanoBytes = convertirABytes(p.tamano, p.unidad);
        p.numPaginas = calcularNumeroPaginas(tamanoBytes, BLOCK_SIZE);
        
        procesos.push_back(p);
    }
    
    distribucionProporcional(procesos, RAM_SIZE);
    
    for (auto& p : procesos) {
        p.tiempoFallas = calcularTiempoFallas(p);
        p.turnaroundTime = calcularTurnaroundTime(p);
    }
    
    cout << "\nResultados:" << endl;
    for (const auto& p : procesos) {
        cout << "Proceso: " << p.nombre << endl;
        cout << "Tiempo de Fallas: " << p.tiempoFallas << " ms" << endl;
        cout << "TurnaRound Time: " << p.turnaroundTime << " ms" << endl;
        cout << endl;
    }
    
    return 0;
}
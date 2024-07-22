#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>
#include <iomanip>

struct Proceso {
    std::string nombre;
    double      tamano;
    std::string unidad;
    int         numPaginas;
    int         marcosAsignados;
    float       tiempoAcceso;
    float       tiempoTransferencia;
    float       tiempoEjecucion;
    float       tiempoFallas;
    float       turnaroundTime;
};


const int RAM_SIZE = 20 * 1024 * 1024; // 2MB
const int BLOCK_SIZE = 4096; // 512B
const int MAX_PROCESOS = 6;

std::vector<Proceso> procesos; //Vector de procesos a analizar.

//Convierte cualquier unidad de entrada a bytes
double convertirABytes(double tamano, std::string unidad) {
    if (unidad == "b") return tamano / 8.0;
    if (unidad == "B") return tamano;
    if (unidad == "KB") return tamano * 1024;
    if (unidad == "MB") return tamano * 1024 * 1024;
    if (unidad == "GB") return tamano * 1024 * 1024 * 1024;
    return 0;
}

//Calcula el número de paginas para el tamaño de un proceso y su tamaño de bloque
int calcularNumeroPaginas(double tamanoBytes, int tamanoBloque) {
    return std::ceil(tamanoBytes / tamanoBloque);
}

//Esta función determina el número de marcos asignados a cada proceso en 
//relacion a su proporcion de páginas respecto a las páginas totales y los marcos disponibles.
void distribucionProporcional() {
    float totalPaginas = 0;
    for (auto& p : procesos) {
        totalPaginas += p.numPaginas;//Suma todos los números de páginas y los guarda.
    }
    
    int marcosDisponibles = RAM_SIZE / BLOCK_SIZE;
    
    for (auto& p : procesos) {
        double proporcion = (double)p.numPaginas / totalPaginas;
        if(proporcion * marcosDisponibles < 1.0)        //Según si la proporcion es menor a los marcos disponibles
            p.marcosAsignados = 1;                      //Asignamos 1 en caso de estar entre 0 y 1.
        else
            p.marcosAsignados = std::floor(proporcion * marcosDisponibles);
    }
}


//Muestra páginas y marcos.
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

//Toma un archivo .csv y extrae los datos de una forma específica como se puede ver en el archivo procesos.csv
void ingresar_desde_archivo(){
    std::ifstream read;
    read.open("procesos.csv", std::ios::in);
    std::string line, word;
    Proceso p;
    getline(read, line);                //Ignora la primera línea
    std::vector<std::string> fila;
    int i = 0;
     while(read.is_open() && i < MAX_PROCESOS){
        i++;
        fila.clear();
        getline(read, line);
        std::stringstream s(line);
        if(line == "")break;
        while (getline(s, word, ',')) {
            fila.push_back(word);           //Obtiene los campos
        }
        fila.push_back(word);

        p.nombre = fila[0];
        p.unidad = fila[2];
        double tamano = std::stod(fila[1]);
        p.tamano = convertirABytes(tamano, p.unidad);
        p.tiempoAcceso = std::stof(fila[3]);
        p.tiempoEjecucion = std::stof(fila[5]);             //Asigna los campos a sus variables correspondientes
        p.tiempoTransferencia = std::stof(fila[4]);
        p.numPaginas = calcularNumeroPaginas(p.tamano, BLOCK_SIZE);
        
        procesos.push_back(p);
    }
    read.close();
}

//Calcula las veces que se cargan y descargan las páginas de un proceso hasta 
//que solo queden pendientes las páginas sobrantes del mismo y así mismo calcula el número de stas últimas.
//Luego, guarda los resultados en las variables "vecesCompletas" y "sobrantes" respectivamente.
void CalcularCargasDescargas(int pags, int marcos, int &vecesCompletas, int &sobrantes){
  int aux = pags/marcos;
  aux += (aux-1);
  vecesCompletas = aux;
  sobrantes = pags % marcos;
}

//Calcula el tiempo de falla y el turnAroundTime y los guarda en el struct de cada proceso. 
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

//Muestra los resultados obtenidos por consola el tiempo de fallas, turn around time, paginas y marcos.
void mostrarResultados() {
    std::cout << "\nResultados:" << std::endl;
    std::cout << std::left << std::setw(10) << "Proceso" 
              << std::setw(20) << "Tiempo de Fallas (ms)" 
              << std::setw(20) << "Turn around Time(ms)"
              << std::setw(20) << "paginas "                //Imprime el header
              << std::setw(20) << "marcos" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (const auto& p : procesos) {
        std::cout << std::left << std::setw(10) << p.nombre 
                  << std::setw(20) << p.tiempoFallas        //Imprime cada uno de los procesos
                  << std::setw(20) << p.turnaroundTime
                  << std::setw(20) <<p.numPaginas 
                  << std::setw(20) <<p.marcosAsignados << std::endl;
    }
}

//Función principal
int main() {
    ingresar_desde_archivo();   //Ingreso el archivo y guardo los procesos
    distribucionProporcional(); //Calculo los marcos y páginas asignados de cada proceso
    calcularTiempos();          //Calculo el tiempo de fallas y el turn around time de cada proceso
    mostrarResultados();        //Muestro estas últimas 4 variables recién calculadas
    system("PAUSE");
    return 0;
}

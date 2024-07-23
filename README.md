# Enunciado Seccion 411

Dado un sistema multiprogramado monoprocesador, de un ambiente de paginación, con una RAM de
2MB, y se ejecutan procesos, determine a través de una rutina programada en C o C++, cuanto tiempo le
lleva al sistema en ejecutar los procesos (TurnaRound Time), ingresados al sistema.

Considere: 

 * Distribución Proporcional, Tamaño bloque de Memoria = 512 bytes
 * Datos de entrada: Nombre Proceso, tamaño en bytes, KB, MB o GB; Tiempo de acceso mls,
Tiempo de transferencia en mls y tiempo promedio de ejecución en mls. (máximo 6 procesos)
 * Datos de Salida: Tiempo de Fallas, TurnaRound Time por proceso

## Instrucciones de uso
Estas instrucciones corresponden a Windows. En caso de ejecutarse en linux, remitirse al informe. Seccion 4.2
1. En primer lugar, se debe acceder al directorio donde se encuentra el código fuente junto al archivo .csv. 
2. Verificar el tamaño de las variables globales dentro del archivo main.cpp, en este caso es de 512 bytes y 2 MB de Ram y se encuentra en la línea 23.
3. Debemos verificar y poner los procesos y sus variables en el archivo csv.
4. Abrir una terminal dentro del directorio
5. Compilar utilizando el comando: “ g++ main.c -o main.exe” ( main.exe es el ejecutable)
6. Ejecutamos el archivo ‘.exe’, de la manera “./main.exe”
7. Finalmente, se desplegará en consola los resultados correspondientes a los datos ingresados.

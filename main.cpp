#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <omp.h>
#include <chrono> // Necesario para std::this_thread::sleep_for
#include <thread> // Para std::this_thread::sleep_for

// Tamaño del canvas
const int canvasWidth = 640;
const int canvasHeight = 480;

// Número de elementos a renderizar
int numElements;

// Estructura para representar un elemento
struct Element {
    double x, y;     // Posición
    double dx, dy;   // Velocidad
    double radius;   // Radio
    double red, green, blue; // Color

    Element() : x(0), y(0), dx(0), dy(0), radius(0), red(0), green(0), blue(0) {}
};

// Arreglo de elementos
Element* elements;

// Función para inicializar los elementos con valores aleatorios
void InitializeElements() {
    srand(time(nullptr));

    for (int i = 0; i < numElements; i++) {
        elements[i].x = rand() % canvasWidth;
        elements[i].y = rand() % canvasHeight;
        elements[i].dx = (rand() % 1000 - 500) / 100.0; // Velocidad en X entre -5 y 5
        elements[i].dy = (rand() % 1000 - 500) / 100.0; // Velocidad en Y entre -5 y 5
        elements[i].radius = rand() % 20 + 10; // Radio entre 10 y 30
        elements[i].red = (rand() % 256) / 255.0;
        elements[i].green = (rand() % 256) / 255.0;
        elements[i].blue = (rand() % 256) / 255.0;
    }
}

// Función para actualizar la posición de los elementos
void UpdateElements() {
    #pragma omp parallel for
    for (int i = 0; i < numElements; i++) {
        elements[i].x += elements[i].dx;
        elements[i].y += elements[i].dy;

        // Detectar colisiones con las paredes
        if (elements[i].x - elements[i].radius < 0 || elements[i].x + elements[i].radius > canvasWidth) {
            elements[i].dx *= -1;
        }
        if (elements[i].y - elements[i].radius < 0 || elements[i].y + elements[i].radius > canvasHeight) {
            elements[i].dy *= -1;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <num_elements>" << std::endl;
        return 1;
    }

    numElements = std::atoi(argv[1]);
    elements = new Element[numElements];

    InitializeElements();

    // Bucle principal del screensaver
    while (true) {
        // Medir el tiempo de inicio
        double startTime = omp_get_wtime();

        // Actualizar elementos
        UpdateElements();

        // Limpiar la pantalla (simulado)
        std::cout << "\033[2J\033[1;1H"; // Limpiar pantalla en la terminal

        // Renderizar elementos (simulado)
        for (int i = 0; i < numElements; i++) {
            std::cout << "\033[" << (int)round(elements[i].y) << ";" << (int)round(elements[i].x) << "H";
            std::cout << "\033[38;2;" << (int)(elements[i].red * 255) << ";" << (int)(elements[i].green * 255) << ";" << (int)(elements[i].blue * 255) << "m";
            std::cout << "O";
        }
        std::cout << "\033[0m"; // Restaurar el color normal

        // Calcular FPS
        double endTime = omp_get_wtime();
        double fps = 1.0 / (endTime - startTime);
        std::cout << "FPS: " << fps << std::endl;

        // Esperar un breve tiempo
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10 ms
    }

    delete[] elements;
    return 0;
}

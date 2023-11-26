#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
using namespace std;
//constantes
const int CHAR_LINEA_HORIZONTAL = 205; // ˭
const int CHAR_ESQUINA_SUPERIOR_IZQUIERDA = 201; // ╔═
const int CHAR_ESQUINA_SUPERIOR_DERECHA = 187; // ═╗
const int CHAR_ESQUINA_INFERIOR_IZQUIERDA = 200; // ╚═
const int CHAR_ESQUINA_INFERIOR_DERECHA = 188; // ═╝
const int CHAR_LINEA_VERTICAL = 186; // ║
const int CHAR_COCHE = 254; // ■
const int CHAR_CLAVO = 193; // ┴
const int CHAR_SORPRESA = 63; // ?
const int CHAR_NORMAL = 32; // ‘ ‘
const int LONG_CARRETERA = 10;
const int MAX_PASOS = 3;
const int TIEMPO_PARADO = 2;
const bool DEBUG = true;

// typedef
typedef enum {NORMAL, CLAVO, SORPRESA} tTipoPosicion;
typedef tTipoPosicion tCarretera[LONG_CARRETERA];

//lee y cargar carril
bool cargaCarretera(tCarretera carretera);
tTipoPosicion stringToEnum(string s);
void iniciaCarril(tCarretera carretera);

// dibujar carril
void dibujaLineaHorizontalSuperior();
void dibujaLineaHorizontalInferior();
void dibujarLinea(int posIni, int posFin, int ascii);
void dibujarPosiciones();
void dibujaCarril(const tCarretera carretera, int posCoche);
void dibujaCarretera(const tCarretera carretera, int posCoche);

//si es sorpresa o clavo
bool esSorpresa(const tCarretera carretera, int posCoche);
bool esClavo(const tCarretera carretera, int posCoche);

// buscar siguiente sorpresa
int buscaSiguientePosSorpresa(const tCarretera carretera, int posCoche);
int buscaSorpresa(const tCarretera carretera, int desde, int incr);

// simulacion
int avanza(int posCoche);
bool haLlegado(int posCoche);
void avanzaCarril(const tCarretera carretera, int& posCoche, int& tiempoParado);
void iniciaCoche(int& posCoche, int& tiempoParado);
void simulaCarrera(const tCarretera carretera);
int pideNumPasos();

int main() {
    srand(time(NULL));
    tCarretera carretera;
    cargaCarretera(carretera);
    string respuesta;
    bool repetir = true;
    while (repetir) {
        simulaCarrera(carretera);
        cout << "FIN DE LA SIMULACION" << endl;
        cout << "¿Quieres repetir? Introduzca(S o N): ";
        cin >> respuesta;
        if (respuesta != "S" && respuesta != "s") {
            repetir = false;
        }
    }
}

//lee y cargar carril
void iniciaCarril(tCarretera carretera) {
    for (int i = 0; i < LONG_CARRETERA; i++)
    {
        carretera[i] = NORMAL;
    }
}
bool cargaCarretera(tCarretera carretera) {
    bool cargaCorrecta = false;
    int cuantos, posicion;
    string nombreArchivo, palabra;
    ifstream archivo;
    cout << "Introduce el nombre del archivo: ";  
    cin >> nombreArchivo;
    archivo.open(nombreArchivo);
     if (archivo.is_open()) {
        iniciaCarril(carretera);
        archivo >> palabra;
        while (palabra != "XX") {
            archivo >> cuantos;
            for (int i = 0; i < cuantos; i++) {
                archivo >> posicion;
                carretera[posicion] = stringToEnum (palabra);
            }
            archivo >> palabra;
        }
        cargaCorrecta = true;
        archivo.close();
    }
    return cargaCorrecta;
}   
tTipoPosicion stringToEnum(string s) {
    tTipoPosicion tipo = NORMAL;
    if (s == "CLAVO") {
        tipo = CLAVO;
    }
    if (s == "SORPRESA") {
        tipo = SORPRESA;
    }
    return tipo;
}

// dibujar carril
void dibujarLinea(int posIni, int posFin, int ascii) {
    for (int i = posIni; i < posFin; i++)
    {
        cout << char(ascii);
    }
}
void dibujaLineaHorizontalSuperior() {
    cout << char(CHAR_ESQUINA_SUPERIOR_IZQUIERDA);
    dibujarLinea(0, LONG_CARRETERA, CHAR_LINEA_HORIZONTAL);
    cout << char(CHAR_ESQUINA_SUPERIOR_DERECHA);
    cout << endl;
}
void dibujaLineaHorizontalInferior() {
    cout << char(CHAR_ESQUINA_INFERIOR_IZQUIERDA);
    dibujarLinea(0, LONG_CARRETERA, CHAR_LINEA_HORIZONTAL);
    cout << char(CHAR_ESQUINA_INFERIOR_DERECHA);
    cout << endl;
}
void dibujarPosiciones(){
    cout << char(CHAR_LINEA_VERTICAL);
    for (int i = 0; i < LONG_CARRETERA; i++)
    {
        cout << i % 10;
    }
    cout << char(CHAR_LINEA_VERTICAL);
    cout << endl;
}
void dibujaCarril(const tCarretera carretera, int posCoche) {
    cout << char(CHAR_LINEA_VERTICAL);
    for (int i = 0; i < LONG_CARRETERA; i++)
    {
        switch (carretera[i]) {
        case NORMAL:
            cout << char(CHAR_NORMAL);
            break;
        case CLAVO:
            cout << char(CHAR_CLAVO);
            break;
        case SORPRESA:
            cout << char(CHAR_SORPRESA);
            break;
        }
    }
    cout << char(CHAR_LINEA_VERTICAL);
    cout << endl;

    cout << char(CHAR_LINEA_VERTICAL);
    dibujarLinea(0, posCoche, CHAR_NORMAL);
    cout << char(CHAR_COCHE);
    dibujarLinea(posCoche + 1, LONG_CARRETERA, CHAR_NORMAL);
    cout << char(CHAR_LINEA_VERTICAL);
    cout << endl;
}
void dibujaCarretera(const tCarretera carretera, int posCoche) {
    dibujaLineaHorizontalSuperior();
    dibujarPosiciones();
    dibujaCarril(carretera, posCoche);
    dibujaLineaHorizontalInferior();
}

//si es sorpresa o clavo
bool esSorpresa(const tCarretera carretera, int posCoche) {
    return carretera[posCoche] == SORPRESA;
}
bool esClavo(const tCarretera carretera, int posCoche) {
    return carretera[posCoche] == CLAVO;
}

// buscar siguiente sorpresa
int buscaSiguientePosSorpresa(const tCarretera carretera, int posCoche) {
    int aleatorio = rand() %2, Ssorpresa;
    
    if (aleatorio == 1) {
        cout << "Hoy no es tu día de suerte, retrocedes a la sorpresa anterior" << endl;
        int incr =-1;
        Ssorpresa= buscaSorpresa(carretera, posCoche - 1, incr);
        
    }
    else {
        cout << "Felicidades avanzas a la siguiente sorpresa" << endl; 
        int incr = 1;
        Ssorpresa = buscaSorpresa(carretera, posCoche +1, incr);
    }
    return Ssorpresa;
}
int buscaSorpresa(const tCarretera carretera, int desde, int incr) {
   while (desde < LONG_CARRETERA && desde > 0 && carretera[desde] != SORPRESA) {
        desde += incr;
    }
    if (desde == LONG_CARRETERA) {
        desde = 0;
    }
    return desde;


}

// simulacion
int avanza(int posCoche) {
    if (!DEBUG) {
        cout << "Pulsa enter para continuar" << endl;
        char a;
        a = getchar();
        posCoche += 1 + rand() % MAX_PASOS;
    }
    else {
        posCoche += pideNumPasos();
    }

    if (posCoche < 0) {
        posCoche = 0;
    }
    if (posCoche > LONG_CARRETERA) {
        posCoche = LONG_CARRETERA;
    }

    return posCoche;
}
bool haLlegado(int posCoche) {
    return posCoche >= LONG_CARRETERA;
}
void avanzaCarril(const tCarretera carretera, int& posCoche, int& tiempoPinchadoRestante) {

    if (tiempoPinchadoRestante == 0) {
        posCoche = avanza(posCoche);

        if (esClavo(carretera, posCoche)) {
            tiempoPinchadoRestante = TIEMPO_PARADO;
            cout << "Has pinchado... estaras" << tiempoPinchadoRestante << "pasos sin moverte" << endl;
            char a;
            a = getchar();
        }
        if (esSorpresa(carretera, posCoche)) {
            dibujaCarretera(carretera, posCoche);
            posCoche = buscaSiguientePosSorpresa(carretera, posCoche);
        }
    }
    else {
        cout << "Te queda un paso menos..." << endl;
        char a;
        a = getchar();
        tiempoPinchadoRestante--;
    }

    dibujaCarretera(carretera, posCoche);
}
void iniciaCoche(int& posCoche, int& tiempoPinchadoRestante) {
    posCoche = 0;
    tiempoPinchadoRestante = 0;
}
void simulaCarrera(const tCarretera carretera) {
    int posCoche;
    int tiempoPinchadoRestante;
    iniciaCoche(posCoche, tiempoPinchadoRestante);
    dibujaCarretera(carretera, posCoche);

    while (not haLlegado(posCoche)) {
        avanzaCarril(carretera, posCoche, tiempoPinchadoRestante);
    }
}
int pideNumPasos() {
    int pasos;
    cout << "Dame el numero de pasos a avanzar o retroceder: ";
    cin >> pasos;
    return pasos;
}
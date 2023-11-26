#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

const int LONG_CARRETERA = 10;
const int MAX_PASOS = 3;
const int TIEMPO_PARADO = 2;
const bool DEBUG = false;


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

typedef enum {NORMAL, SORPRESA, CLAVO} tTipoPosicion;
typedef tTipoPosicion tCarretera[LONG_CARRETERA];

bool cargaCarretera(tCarretera&);
tTipoPosicion stringToEnum(string s);
void iniciaCarril(tCarretera carretera);
void iniciaCoche(int&, int&);

void dibujarLinea(int posIni, int PosFin, int ascii);
void dibujarLineaHorizontalSuperior();
void dibujarLineaHorizontalInferior();
void dibujaCarril(int posCoche, tCarretera carretera);
void dibujarCarretera(int posCoche, tCarretera carretera);

bool haLlegado(int posCoche);
int pideNumPasos();
bool esSorpresa(int posCoche, tCarretera carretera);
bool esClavo(int posCoche, tCarretera carretera);
int buscaSiguientePosSorpresa(int posCoche, const tCarretera carretera);

int avanza(int posCoche);

void avanzaCarril(tCarretera, int&, int&);
void simulaCarrera(tCarretera carretera);

tCarretera carretera;

void iniciaCarril(tCarretera carretera) {
    for (int i = 0; i < LONG_CARRETERA; i++)
    {
        carretera[i] = NORMAL;
    }
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

void cargaElementoCarretera(ifstream& archivo, string identificador, tCarretera& carretera) {
	int count;
    //la siguiente palabra es el número de veces que aparecen
	archivo >> count;
	int codigo;
	tTipoPosicion tipo = stringToEnum(identificador);
	for (int i = 0; i < count; i++) {
		archivo >> codigo;
		carretera[codigo] = tipo;
	}
}

bool cargaCarretera(tCarretera& carretera) {
    iniciaCarril(carretera);
    
    bool apertura = true;
    string nombreArchivo;
    cout << "Introduce el nombre del archivo que se va a importar: ";
    cin >> nombreArchivo;
    
	ifstream archivo;
	archivo.open(nombreArchivo);
	if (archivo.is_open()) {
		string identificador;
        //quita la primera palabra de "archivo" y la almacena en "palabra"
		archivo >> identificador;
		while (identificador != "XX") {
			cargaElementoCarretera(archivo, identificador, carretera);
			archivo >> identificador;
		}
		archivo.close();
	}
	else {
		apertura = false;
	}
	return apertura;
}

int main()
{
    srand(time(NULL));
    tCarretera carretera;
    bool cargaCorrecta = cargaCarretera(carretera);
    
    if (cargaCorrecta) {
        bool repetir = true;
        while (repetir) {
            simulaCarrera(carretera);
            cout << "FIN DE LA SIMULACION" << endl;
            cout << "¿Quieres repetir? (S o N): ";
            string respuesta;
            cin >> respuesta;
            if (respuesta != "S" && respuesta != "s") {
                repetir = false;
            }
        }
    } else {
        cout << "No se ha podido cargar el archvo." << endl;
    }
}


void iniciaCoche(int& posCoche, int& tiempoPinchadoRestante){
    posCoche = 0;
    tiempoPinchadoRestante = 0;
}

void simulaCarrera(tCarretera carretera) {
    int posCoche;
    int tiempoPinchadoRestante;
    iniciaCoche(posCoche, tiempoPinchadoRestante);
    dibujarCarretera(posCoche, carretera);

    while (not haLlegado(posCoche)) {
        avanzaCarril(carretera, posCoche, tiempoPinchadoRestante);
    }
}
void avanzaCarril(tCarretera carretera, int& posCoche, int& tiempoPinchadoRestante) {
    if (tiempoPinchadoRestante == 0) {
        posCoche = avanza(posCoche);
        dibujarCarretera(posCoche, carretera);

        if (esClavo(posCoche, carretera)) {
            tiempoPinchadoRestante = TIEMPO_PARADO;
            cout << "Has pinchado... estaras " << tiempoPinchadoRestante << " pasos sin moverte" << endl;
            char a;
            a = getchar();
        }
        if (esSorpresa(posCoche, carretera)) {
            dibujarCarretera(posCoche, carretera);
            posCoche = buscaSiguientePosSorpresa(posCoche, carretera);
        }
    } else {
        cout << "Te queda un paso menos..." << endl;
        char a;
        a = getchar();
        tiempoPinchadoRestante--;
        
        dibujarCarretera(posCoche, carretera);
    }
}

int avanza (int posCoche) {
    if (!DEBUG) {
        cout << "Pulsa enter para continuar" << endl;
        char a;
        a = getchar();
        int pasos = 1 + rand() % MAX_PASOS;
        cout << "Avanzas " << pasos << " pasos" << endl;
        posCoche += pasos;
    } else {
        posCoche += pideNumPasos();
        if (posCoche < 0) {
            posCoche = 0;
        }
        if (posCoche > LONG_CARRETERA) {
            posCoche = LONG_CARRETERA;
        }
    }
    return posCoche;
}

void dibujarLinea(int posIni, int posFin, int ascii)
{
    for (int i = posIni; i < posFin; i++)
    {
        cout << char(ascii);
    }
}
void dibujarLineaHorizontalSuperior()
{
    cout << char(CHAR_ESQUINA_SUPERIOR_IZQUIERDA);
    dibujarLinea(0, LONG_CARRETERA, CHAR_LINEA_HORIZONTAL);
    cout << char(CHAR_ESQUINA_SUPERIOR_DERECHA);
    cout << endl;
}
void dibujarPosiciones()
{
    cout << char(CHAR_LINEA_VERTICAL);
    for (int i = 0; i < LONG_CARRETERA; i++)
    {
        cout << i%10;
    }
    cout << char(CHAR_LINEA_VERTICAL);
    cout << endl;
}
void dibujarLineaHorizontalInferior()
{
    cout << char(CHAR_ESQUINA_INFERIOR_IZQUIERDA);
    dibujarLinea(0, LONG_CARRETERA, CHAR_LINEA_HORIZONTAL);
    cout << char(CHAR_ESQUINA_INFERIOR_DERECHA);
    cout << endl;
}
void dibujaCarril(int posCoche, tCarretera carretera)
{
    // --- Clavos / Sorpresas ---
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

    // --- Coche ---
    cout << char(CHAR_LINEA_VERTICAL);
    dibujarLinea(0, posCoche, CHAR_NORMAL);
    cout << char(CHAR_COCHE);
    dibujarLinea(posCoche + 1, LONG_CARRETERA , CHAR_NORMAL);
    cout << char(CHAR_LINEA_VERTICAL);
    cout << endl;
}

void dibujarCarretera(int posCoche, tCarretera carretera)
{
    dibujarLineaHorizontalSuperior();
    //dibujarPosiciones();
    dibujaCarril(posCoche, carretera);
    dibujarLineaHorizontalInferior();
}
    
bool haLlegado(int posCoche) {
    return posCoche >= LONG_CARRETERA;
}

int pideNumPasos() {
    int pasos;
    cout << "Dame el numero de pasos a avanzar o retroceder: ";
    cin >> pasos;
    return pasos;
}

bool esSorpresa(int posCoche, tCarretera carretera) {
    return carretera[posCoche] == SORPRESA;
}

bool esClavo(int posCoche, tCarretera carretera) {
    return carretera[posCoche] == CLAVO;
}

int buscaSorpresa(const tCarretera carretera, int pos, int incr) {
    pos += incr; //pos ya es una sorpresa
    while (pos < LONG_CARRETERA && pos > 0 && carretera[pos] != SORPRESA) {
        pos += incr;
    }
    if (pos >= LONG_CARRETERA || pos <= 0) {
        cout << "No habia sorpresas en tu camino" << endl;
        pos = 0;
    }
    return pos;
}

int buscaSiguientePosSorpresa(int posCoche, const tCarretera carretera) {
    int aleatorio = rand() %2;
    
    int SiguienteSorpresa;
    if (aleatorio == 1) {
        cout << "Hoy no es tu dia de suerte, retrocedes a la sorpresa anterior" << endl;
        int incr =-1;
        SiguienteSorpresa = buscaSorpresa(carretera, posCoche, incr);
        
    }
    else {
        cout << "Felicidades avanzas a la siguiente sorpresa" << endl; 
        int incr = 1;
        SiguienteSorpresa = buscaSorpresa(carretera, posCoche, incr);
    }
    return SiguienteSorpresa;
}
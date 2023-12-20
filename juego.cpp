#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <ctype.h>
using namespace std;

const int LONG_CARRETERA = 13;
const int MAX_PASOS = 3;
const int TIEMPO_PARADO = 3;
const int NUM_CARRILES = 3;
const int MAX_PODIUMS = 10;

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

typedef struct {
    int pos;
    int tiempoPinchadoRestante;
} tCoche;

typedef tTipoPosicion tArrayPosiciones[LONG_CARRETERA];
typedef struct {
    tCoche coche;
    tArrayPosiciones posiciones;
} tCarril;
typedef tCarril tCarretera[NUM_CARRILES];

typedef int tGanadores[NUM_CARRILES];
typedef struct {
    string idCarrera;
    tGanadores ganadores;
} tPodium;

typedef tPodium tArrayPodiums[MAX_PODIUMS];
typedef struct {
    int cont;
    tArrayPodiums lista;
} tListaPodiums;


bool cargaCarretera(tCarretera&);
tTipoPosicion stringToEnum(string s);
void iniciaCarril(tCarretera carretera);
void iniciaCoche(tCoche& coche);

void simulaCarrera(tCarretera carretera, tGanadores ganadores);
void avanzaCarriles(tCarretera carretera, tGanadores ganadores, int& cMeta);
bool avanzaCarril(tCarretera carretera, tCarril& carril, int nc);
int avanza(int posCoche);

void dibujarLinea(int posIni, int PosFin, int ascii);
void dibujarLineaHorizontalSuperior();
void dibujarLineaHorizontalInferior();
void dibujaCarril(int posCoche, tCarretera carretera);
void dibujarCarretera(tCarretera carretera);

bool haLlegado(tCoche coche);
int pideNumPasos();
bool esSorpresa(int pos, tArrayPosiciones posiciones);
bool esClavo(int pos, tArrayPosiciones posiciones);
int buscaSiguientePosSorpresa(int posCoche, const tCarril carril);

void iniciaListaPodiums(tListaPodiums& listaP);
void insertaPodium(tListaPodiums& listaP, const tPodium& podium);
void muestraGanadores(const tGanadores ganadores);
bool guardaListaPodiums(const tListaPodiums& podiums);

int main()
{
    srand(time(NULL));
    tCarretera carretera;
    bool cargaCorrecta = cargaCarretera(carretera);
    if (cargaCorrecta) {
        tListaPodiums podiums;
        iniciaListaPodiums(podiums);
        
        bool repetir = true;
        while (repetir) {
            string id;
            cout << "Id de la carrera: ";
            cin >> id;
            podiums.lista[podiums.cont].idCarrera = id;

            simulaCarrera(carretera, podiums.lista[podiums.cont].ganadores);

            muestraGanadores(podiums.lista[podiums.cont].ganadores);
            podiums.cont++;

            cout << "FIN DE LA SIMULACION" << endl;
            cout << "Quieres repetir? (S o N): ";
            string respuesta;
            cin >> respuesta;
            if (respuesta != "S" && respuesta != "s") {
                repetir = false;
            }
        }

        guardaListaPodiums(podiums);
    }
    else {
        cout << "No se ha podido cargar el archvo." << endl;
    }
}

void iniciaCarril(tCarril& carril) {
    for (int i = 0; i < LONG_CARRETERA; i++)
    {
        carril.posiciones[i] = NORMAL;
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

void cargaElementoCarril(ifstream& archivo, string identificador, tCarril& carril) {
	int count;
    //la siguiente palabra es el número de veces que aparecen
	archivo >> count;
	int codigo;
	tTipoPosicion tipo = stringToEnum(identificador);
	for (int i = 0; i < count; i++) {
		archivo >> codigo;
		carril.posiciones[codigo] = tipo;
	}
}

void leeCarril(ifstream& archivo, tCarril& carril) {
    string identificador;
    //quita la primera palabra de "archivo" y la almacena en "identificador"
    archivo >> identificador;

    while (identificador != "XX") {
        cargaElementoCarril(archivo, identificador, carril);
        archivo >> identificador;
    }
}

bool cargaCarretera(tCarretera& carretera) {
    
    bool apertura = true;
    string nombreArchivo;
    nombreArchivo = "carriles.txt";
    //cout << "Introduce el nombre del archivo que se va a importar: ";
    //cin >> nombreArchivo;
    
	ifstream archivo;
	archivo.open(nombreArchivo);
	if (archivo.is_open()) {
        for (int i = 0; i < NUM_CARRILES; i++)
        {
            iniciaCarril(carretera[i]);
            leeCarril(archivo, carretera[i]);
        }
        
		archivo.close();
	}
	else {
		apertura = false;
	}
	return apertura;
}


void iniciaCoche(tCoche& coche){
    coche.pos = 0;
    coche.tiempoPinchadoRestante = 0;
}

void iniciaCoches(tCarretera carretera) {
    for (int i = 0; i < NUM_CARRILES; i++)
    {
        iniciaCoche(carretera[i].coche);
    }
    
}

void simulaCarrera(tCarretera carretera, tGanadores ganadores) {
    iniciaCoches(carretera);
    dibujarCarretera(carretera);

    int numGanadores = 0;
    while (numGanadores < NUM_CARRILES) {
        avanzaCarriles(carretera, ganadores, numGanadores);
    }
}

void avanzaCarriles(tCarretera carretera, tGanadores ganadores, int& cMeta) {
    cout << "Empezando nueva vuelta..." << endl;
    
    for (int i = 0; i < NUM_CARRILES; i++)
    {
        bool habiaGanado = false;
        for (int j = 0; j < cMeta; j++)
        {
            if (ganadores[j] == i) {
                habiaGanado = true;
            }
        }
        
        if (!habiaGanado) {
            cout << "Avanzando en el carril " << i << "..." << endl;
            bool haGanado = avanzaCarril(carretera, carretera[i], i);
            dibujarCarretera(carretera);

            if (haGanado){ 
                ganadores[cMeta] = i;
                cMeta++;
            }
            
            cout << "Pulsa enter para continuar" << endl;
            char a;
            a = getchar();
        }
    }
}

bool avanzaCarril(tCarretera carretera, tCarril& carril, int nc) {
    if (!haLlegado(carril.coche)) {
        if (carril.coche.tiempoPinchadoRestante == 0) {
            carril.coche.pos = avanza(carril.coche.pos);

            if (esClavo(carril.coche.pos, carril.posiciones)) {
                carril.coche.tiempoPinchadoRestante = TIEMPO_PARADO;
                cout << "Ha pinchado... estara " << carril.coche.tiempoPinchadoRestante << " pasos sin moverte" << endl;
            }
            if (esSorpresa(carril.coche.pos, carril.posiciones)) {
                carril.coche.pos = buscaSiguientePosSorpresa(carril.coche.pos, carril);
            }
        }
        else {
            cout << "Quedan " << carril.coche.tiempoPinchadoRestante << " pasos pinchado..." << endl;
            carril.coche.tiempoPinchadoRestante--;
        }
    }

    return haLlegado(carril.coche);
}

int avanza(int pos) {
    if (!DEBUG) {
        int pasos = 1 + rand() % MAX_PASOS;
        cout << "Avanza " << pasos << " pasos" << endl;
        pos += pasos;
    }
    else {
        pos += pideNumPasos();
        if (pos < 0) {
            pos = 0;
        }
        if (pos > LONG_CARRETERA) {
            pos = LONG_CARRETERA;
        }
    }
    return pos;
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
        cout << i % 10;
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
void dibujarLineaDiscontinua()
{
    cout << char(CHAR_LINEA_VERTICAL);
    for (int i = 0; i < LONG_CARRETERA; i++)
    {
        if (i%2 == 1) {
            cout << " ";
        } else {
            cout << char(CHAR_LINEA_HORIZONTAL);
        }
    }
    cout << char(CHAR_LINEA_VERTICAL);
    cout << endl;
}
void dibujaCarril(tCarril& carril)
{
    // --- Clavos / Sorpresas ---
    cout << char(CHAR_LINEA_VERTICAL);
    for (int i = 0; i < LONG_CARRETERA; i++)
    {
        switch (carril.posiciones[i]) {
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
    dibujarLinea(0, carril.coche.pos, CHAR_NORMAL);
    cout << char(CHAR_COCHE);
    dibujarLinea(carril.coche.pos + 1, LONG_CARRETERA, CHAR_NORMAL);
    cout << char(CHAR_LINEA_VERTICAL);
    cout << endl;
}

void dibujarCarretera(tCarretera carretera)
{
    dibujarLineaHorizontalSuperior();
    dibujarPosiciones();
    for (int i = 0; i < NUM_CARRILES-1; i++)
    {
        dibujaCarril(carretera[i]);
        dibujarLineaDiscontinua();
    }
    dibujaCarril(carretera[NUM_CARRILES-1]);
    
    dibujarLineaHorizontalInferior();
}

bool haLlegado(tCoche coche) {
    return coche.pos >= LONG_CARRETERA;
}

int pideNumPasos() {
    int pasos;
    cout << "Dame el numero de pasos a avanzar o retroceder: ";
    cin >> pasos;
    return pasos;
}

bool esSorpresa(int pos, tArrayPosiciones posiciones) {
    return posiciones[pos] == SORPRESA;
}

bool esClavo(int pos, tArrayPosiciones posiciones) {
    return posiciones[pos] == CLAVO;
}

int buscaSorpresa(const tCarril carril, int pos, int incr) {
    pos += incr; //pos ya es una sorpresa
    while (pos < LONG_CARRETERA && pos > 0 && carril.posiciones[pos] != SORPRESA) {
        pos += incr;
    }
    if (pos >= LONG_CARRETERA || pos <= 0) {
        cout << "No habia sorpresas en tu camino" << endl;
        pos = 0;
    }
    return pos;
}

int buscaSiguientePosSorpresa(int posCoche, const tCarril carril) {
    int aleatorio = rand() % 2;

    int SiguienteSorpresa;
    if (aleatorio == 1) {
        cout << "Hoy no es tu dia de suerte, retrocedes a la sorpresa anterior" << endl;
        int incr = -1;
        SiguienteSorpresa = buscaSorpresa(carril, posCoche, incr);

    }
    else {
        cout << "Felicidades avanzas a la siguiente sorpresa" << endl;
        int incr = 1;
        SiguienteSorpresa = buscaSorpresa(carril, posCoche, incr);
    }
    return SiguienteSorpresa;
}

void iniciaListaPodiums(tListaPodiums& listaP) {
    listaP.cont = 0;    
}

void insertaPodium(tListaPodiums& listaP, const tPodium& podium){
    if (listaP.cont < MAX_PODIUMS) {
        listaP.lista[listaP.cont] = podium;
        listaP.cont++;
    }
}

void muestraGanadores(const tGanadores ganadores){
    cout << "-- PODIUM --" << endl;
    for (int i = 0; i < NUM_CARRILES; i++)
    {
        cout << i+1 << "- Coche numero " << ganadores[i] << endl;
    }
    
}

bool guardaListaPodiums(const tListaPodiums& podiums){
    ofstream archivo;
    archivo.open ("ganadores.txt");
    for (int i = 0; i < podiums.cont; i++)
    {
        archivo << "Podium carrera " << podiums.lista[i].idCarrera << endl;
        for (int j = 0; j < NUM_CARRILES; j++)
        {
            archivo << j+1 << "- Coche número " << podiums.lista[i].ganadores[j] << endl;
        }
        archivo << endl;
    }
    archivo.close();
}
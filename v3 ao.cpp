#include<iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <fstream>
#include <string>
#include<cctype>
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
const int TIEMPO_PARADO = 2;
const bool DEBUG = false;
const int MAX_PASOS = 3;
const int LONG_CARRETERA = 10;
const int NUM_CARRILES = 3;
const int MAX_PODIUMS = 3;



//Definiciones de datos
typedef enum { NORMAL, SORPRESA, CLAVO } tTipoPosicion;
typedef tTipoPosicion tArrayPosiciones[LONG_CARRETERA];


//Estructuras de datos
typedef struct {
	int pos;
	int tiempoParado;
} tCoche;


typedef struct {
	tCoche coche;
	tArrayPosiciones posicion;
} tCarril;



typedef tCarril tArrayPosicion[NUM_CARRILES];
typedef tCarril tCarretera[NUM_CARRILES];
tCarretera carretera;

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


//leer del archivo de texto y cargar la carretera
void iniciaCoche(tCoche& coche);
void iniciaCarril(tCarril& carril);
void leeCarril(ifstream& archivo, tCarril& carril);
bool cargaCarretera(tCarretera carretera);
//encargados de controlar la lógica de la simulación
void iniciaCoches(tCarretera& carretera);
bool esSorpresa(const tArrayPosiciones pos, int p);
bool esClavo(const tArrayPosiciones pos, int p);
int buscaSiguientePosSorpresa(const tArrayPosiciones pos, int p);
int buscaSorpresa(const tArrayPosiciones pos, int desde, int incr);
int pideNumPasos();
int avanza(int posCoche);
bool haLlegado(int posCoche);
bool avanzaCarril(const tCarretera carretera, tCarril& carril, int nc);
void avanzaCarriles(tCarretera carretera, tGanadores ganadores, int& cMeta);
void simulaCarrera(tCarretera carretera, tGanadores ganadores);
bool repetir(const char a);
char pedir(tListaPodiums& listaP);
//encargados de dibujar el estado de la carrera
void dibujaLinea(int posIni, int posFin, int ascii);
void dibujaLineaHorizontalInferior();
void dibujaCarril(const tCarril& carril);
void dibujaLineaDiscontinua();
void dibujaLineaHorizontalSuperior();
void dibujaCarretera(const tCarretera carriles);
//encargados de gestionar y mostrar los pódiums
void iniciaListaPodiums(tListaPodiums& listaP);
void insertaPodium(tListaPodiums& listaP, const tPodium& podium);
void muestraGanadores(const tGanadores ganadores);
bool guardaListaPodiums(const tListaPodiums& podiums);
void pedirId(tListaPodiums& podium);
void guardarId(ofstream& archivo, const tListaPodiums& podiums);
void guardar(ofstream& archivo, const tListaPodiums& podiums, int cont);

int main() {
	tCarretera carretera;
	tListaPodiums listaP;
	srand(time(NULL));
	iniciaListaPodiums(listaP);
	if (cargaCarretera(carretera)) {
		while (repetir(pedir(listaP)))
		{
			
			if (listaP.cont < MAX_PODIUMS) {
				pedirId(listaP);
				iniciaCoches(carretera);
				simulaCarrera(carretera, listaP.lista[listaP.cont].ganadores);
				muestraGanadores(listaP.lista[listaP.cont].ganadores);
				listaP.cont++;
			}
			guardaListaPodiums(listaP);
			
		}
		
	}
	else {
		cout << "No se ha podido abrir el archivo";
	}
	return 0;
}
tTipoPosicion stringToEnum(string s) {
	tTipoPosicion t = NORMAL;
	if (s == "CLAVO") {
		t = CLAVO;
	}
	else if (s == "SORPRESA") {
		t = SORPRESA;
	}
	return t;
}
//leer del archivo de texto y cargar la carretera
void iniciaCoche(tCoche& coche) {
	coche.pos = 0;
	coche.tiempoParado = 0;
}
void iniciaCarril(tCarril& carril) {
	for (int i = 0; i < LONG_CARRETERA; i++) {
		carril.posicion[i] = NORMAL;	
		iniciaCoche(carril.coche);
	}
}
void leeCarril(ifstream& archivo, tCarril& carril) {
	string tipo;
	int n, posicion;
	while (archivo >> tipo&&tipo != "XX")
	{
		archivo >> n; // el numero de clavo o sorpresa
		for (int i = 0; i < n; i++)
		{
			archivo >> posicion;// donde se encuentra 
			carril.posicion[posicion] = stringToEnum(tipo);
		}
	}
	
}
bool cargaCarretera(tCarretera carretera) {
	bool apertura;
	string nom_archivo, tipo;
	ifstream archivo;
//	cout << "Ingrese el nombre del archivo: ";
	//cin >> nom_archivo;

	archivo.open("carriles.txt");
	if (archivo.is_open()) {
		for (int i = 0; i < NUM_CARRILES; i++)
		{
			iniciaCarril(carretera[i]);
			leeCarril(archivo, carretera[i]);
		}
		apertura = true;
		archivo.close();
	}
	else {
		apertura = false;
	}
	return apertura;
}
//encargados de controlar la lógica de la simulación
void iniciaCoches(tCarretera& carretera) {
	for (int i = 0; i < NUM_CARRILES; i++)
	{
		iniciaCoche(carretera[i].coche);
	}
}
bool esSorpresa(const tArrayPosiciones pos, int p) {
	return (pos[p] == SORPRESA);
}
bool esClavo(const tArrayPosiciones pos, int p) {
	return (pos[p] == CLAVO);
}
int buscaSorpresa(const tArrayPosiciones pos, int desde, int incr) {
	while (desde < LONG_CARRETERA && desde > 0 && pos[desde] != SORPRESA) {
		desde += incr;
	}
	if (desde == LONG_CARRETERA) {
		desde = 0;
	}
	return desde;
}
int buscaSiguientePosSorpresa(const tArrayPosiciones pos, int p) {
	int numero;
	numero = rand() % 2;
	int incr = 0;
	int salta = 0;
	if (numero == 0) {
		cout << "Avanzas hasta la siguiente sorpresa" << endl;
		incr = 1;
		salta = buscaSorpresa( pos, p + incr, incr);
	}
	else if (numero == 1) {
		cout << "Retrocedes hasta la anterior sorpresa" << endl;
		incr = -1;
		salta = buscaSorpresa(pos, p + incr, incr);
	}
	return salta;
}
int pideNumPasos() {
	int pasos;
	cout << "Introduce el numero de pasos que deseas avanzar o retroceder: ";
	cin >> pasos;
	return pasos;
}
int avanza(int posCoche) {
	int pasos;
	char c;
	if (!DEBUG) {
		pasos = 1 + rand() % (MAX_PASOS);
		cout << "Vas a avanzar " << pasos << " pasos" << endl;
		posCoche = posCoche + pasos;
		cout << "Pulsa ENTER para continuar" << endl;
		char c;
		c = getchar();
	}
	else {
		pasos = pideNumPasos();
		posCoche += pasos;
	}

	if (posCoche < 0)posCoche = 0;
	else if (posCoche > LONG_CARRETERA)posCoche = LONG_CARRETERA;
	return posCoche;
}
bool haLlegado(int posCoche) {
	return (posCoche >= LONG_CARRETERA);
}
bool avanzaCarril(const tCarretera carretera, tCarril& carril, int nc) {
	bool enMeta = false;
	dibujaCarretera(carretera);
	cout << "El carril " << nc << " va a avanzar." << endl;
	if (carril.coche.tiempoParado == 0) {
		carril.coche.pos = avanza(carril.coche.pos);
		if (haLlegado(carril.coche.pos)) {
			dibujaCarretera(carretera);
			enMeta = true;	
		}
		else {
			if (esSorpresa(carril.posicion, carril.coche.pos)) {
				dibujaCarretera(carretera);
				cout << "HAS CAIDO EN UNA SORPRESA. PASAS A LA SIGUIENTE SORPRESA" << endl;
				int salta = buscaSiguientePosSorpresa(carril.posicion, carril.coche.pos);
				carril.coche.pos = salta;
			}
			else if (esClavo(carril.posicion, carril.coche.pos)) {
				cout << "Has pinchado...estaras " << TIEMPO_PARADO << " pasos sin moverte" << endl;
				carril.coche.tiempoParado = TIEMPO_PARADO;
			}
		}
	}
	else {
		cout << "Te quedan " << carril.coche.tiempoParado << " pasos sin moverte..." << endl;
		carril.coche.tiempoParado--;
	}
	return enMeta;
}
void avanzaCarriles(tCarretera carretera, tGanadores ganadores, int& cMeta) {
	for (int i = 0; i < NUM_CARRILES; i++)
	{
		if (!haLlegado(carretera[i].coche.pos) && avanzaCarril(carretera, carretera[i], i))
		{
			ganadores[cMeta] = i;
			cMeta++;
		}
	}
}
void simulaCarrera(tCarretera carretera, tGanadores ganadores) {
	int cMeta = 0;
	while (cMeta != NUM_CARRILES) {
		avanzaCarriles(carretera, ganadores, cMeta);
	}
	cout << "FIN DE LA CARRERA" << endl;
}
bool repetir(const char a) {
	bool rep = false;
	if (a == 'N') {
		rep = false;
	}
	else if (a == 'S') {
		rep = true;
	}
	return rep;
}
char pedir(tListaPodiums& listaP) {
	char a = 'a';
	while (a != 'S' && a != 'N') {
		cout << "Deseas realizar una simulacion? (S/N): ";
		cin >> a;
		a = toupper(a);
	}
	return a;
}
//encargados de dibujar el estado de la carrera



void dibujaLinea(int posIni, int posFin, int ascii) {
	for (int i = posIni; i < posFin; i++)
	{
		cout << char(ascii);
	}
}


void dibujaLineaHorizontalSuperior() {
	cout << char(CHAR_ESQUINA_SUPERIOR_IZQUIERDA);
	dibujaLinea(0, LONG_CARRETERA, CHAR_LINEA_HORIZONTAL);
	cout << char(CHAR_ESQUINA_SUPERIOR_DERECHA) << endl;
}


void dibujaLineaHorizontalInferior() {
	cout << char(CHAR_ESQUINA_INFERIOR_IZQUIERDA);
	dibujaLinea(0, LONG_CARRETERA, CHAR_LINEA_HORIZONTAL);
	cout << char(CHAR_ESQUINA_INFERIOR_DERECHA) << endl;
}

void dibujaCarril(const tCarril& carril) {
	cout << char(CHAR_LINEA_VERTICAL);
	for (int i = 0; i < LONG_CARRETERA; i++) {
		cout << i;
	}
	cout << char(CHAR_LINEA_VERTICAL) << endl;
	cout << char(CHAR_LINEA_VERTICAL);
	for (int i = 0; i < LONG_CARRETERA; i++) {
		if (carril.posicion[i] == NORMAL) cout << char(CHAR_NORMAL);
		if (carril.posicion[i] == SORPRESA) cout << char(CHAR_SORPRESA);
		if (carril.posicion[i] == CLAVO) cout << char(CHAR_CLAVO);
	}
	cout << char(CHAR_LINEA_VERTICAL) << endl;

	cout << char(CHAR_LINEA_VERTICAL);
	int j = 0;
	while (j< carril.coche.pos)
	{
		cout << char(CHAR_NORMAL);
		j++;
	}
	cout << char(CHAR_COCHE);
	while (j < LONG_CARRETERA -1)
	{
		cout << char(CHAR_NORMAL);
		j++;
	}
	cout << char(CHAR_LINEA_VERTICAL) << endl;
}

void dibujaLineaDiscontinua() {

	cout << char(CHAR_LINEA_VERTICAL);

	for (int i = 0; i < LONG_CARRETERA; i++)
	{
		if (i % 2 == 0)
		{
			cout << char(CHAR_NORMAL);
		}
		else
		{
			cout << char(CHAR_LINEA_HORIZONTAL);
		}
	}
	cout << char(CHAR_LINEA_VERTICAL) << endl;

}


void dibujaCarretera(const tCarretera carriles) {

	dibujaLineaHorizontalSuperior();

	for (int i = 0; i < NUM_CARRILES; i++) {

		dibujaCarril(carriles[i]);
		if (i < NUM_CARRILES - 1) dibujaLineaDiscontinua();
	}
	dibujaLineaHorizontalInferior();
}

//encargados de gestionar y mostrar los pódiums
void iniciaListaPodiums(tListaPodiums& listaP) {
	listaP.cont = 0;
}
void insertaPodium(tListaPodiums& listaP, const tPodium& podium) {
	listaP.lista[listaP.cont] = podium;
	listaP.cont++;
}
void muestraGanadores(const tGanadores ganadores) {
	for (int i = 0; i < NUM_CARRILES; i++)
	{
		cout <<i+1<< " puesto es el carril " << ganadores[i] << endl;
	}
}
bool guardaListaPodiums(const tListaPodiums& podium) {
	bool apertura = true;
	ofstream archivo;
	archivo.open("ganadores.txt");
	if (archivo.is_open()) {
		archivo << "Lista de ganadores" << endl;
		guardarId(archivo, podium);
		archivo.close();
	}
	else {
		apertura = false;
	}
	return apertura;
}
void pedirId(tListaPodiums& podium) {
	cout << "Dime un identificador para la carrera: ";
	cin >> podium.lista[podium.cont].idCarrera;
}
void guardar(ofstream& archivo, const tListaPodiums& podiums, int cont) {
	for (int j = 0; j < NUM_CARRILES; j++)
	{
		archivo << "Puesto " << j + 1 << " es el carril: " << podiums.lista[cont].ganadores[j] << endl;
	}
}
void guardarId(ofstream& archivo, const tListaPodiums& podiums) {
	for (int i = 0; i < podiums.cont; i++)
	{
		archivo << "id de la carrera: " << podiums.lista[i].idCarrera << endl;
		guardar(archivo, podiums, i);
	}
}
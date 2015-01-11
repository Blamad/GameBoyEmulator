
#include "Z80.h"

/*
Procek.
Prosty schemat dzia�ania: 
- pobierz	(funkcja dispatch pobiera pierwszy bajt pod adresem (czyli sam rozkaz)
- dekoduj	(w tablicy _map trzymane s� wszystkie funkcje pod indeksami odpowiadaj�cymi ich reprezentacji binarnej (np. x00h = rozkaz NOP)
			dlatego odwo�anie si� do tablicy _map[numer_rozkazu] wywo�a po��dan� funkcj� (na razie w teorii)..)
- wykonaj	(wywo�anie odpowiedniej funkcji w dekodowaniu spowoduje jej wykonanie. Je�eli funkcja posiada dodatkowe parametry 
			(rozkaz ma rozmiar od 1 do 3 bajt�w w��cznie z danymi, dlatego same dane mog� nie wyst�pi� lub mie� od 1 do 2 bajt�w)
			to sama sobie je pobiera i analizuje. Tutaj trzeba zaimplementowa� wszystkie funkcje, na tym na pocz�tku si� skupiam.)
*/

Z80::Z80()
{
	init();
}

Z80::~Z80()
{
}

void Z80::reset()
{
	_r.a = 0;
	_r.b = 0;
	_r.c = 0;
	_r.d = 0;
	_r.e = 0;
	_r.h = 0;
	_r.l = 0;
	_r.pc = 0;
	_r.sp = 0;
	_r.m = 0;
	_r.t = 0;
	_r.r = 0;

	_clock.m = 0;
	_clock.t = 0;
}

void Z80::dispatch()
{
	_r.r = (_r.r + 1) & 127; //tlumaczenie pobranego polecenia na kod. Nie wiem czy to dziala jak powinno, by� mo�e jest zb�dne.
	*this.*_map[mmu.rb(_r.pc++)]();
	_r.pc &= 65535;
	_clock.m += _r.m;
}

void Z80::init()
{
	R _r = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Clock _clock = {0, 0};
	OpCodeMap _map[] = 
	{ 
		//00
		NOP,		LDBCnn,		LDBCmA,		INCBC,
		INCr_b,		DECr_b,		LDrn_b,		RLCA,
		LDmmSP,		ADDHLBC,	LDABCm,		DECBC,
		INCr_c,		DECr_c,		LDrn_c,		RRCA,
		//10

	};
}

/*
Tu nie mo�e by� taki burdel.. Posortuj� to sobie wed�ug jakiej� kolejno�ci, mo�e jak ma ten �ebek z tutorialu od javascriptu.
*/

//00
void Z80::NOP() { _r.m = 1; }
void Z80::LDBCnn() { }
void Z80::LDBCmA() { }
void Z80::INCBC() { }
void Z80::INCr_b() { }
void Z80::DECr_b() { }
void Z80::LDrn_b() { }
void Z80::RLCA() { }
void Z80::LDmmSP() { }
void Z80::ADDHLBC() { }
void Z80::LDABCm() { }
void Z80::DECBC() { }
void Z80::INCr_c() { }
void Z80::DECr_c() { }
void Z80::LDrn_c() { }
void Z80::RRCA() { }
//10
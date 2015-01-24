#include <vector>
#include <iostream>
#include "GPU.h"

using namespace std;

#ifndef MMU_H
#define MMU_H

#include <vector>
#include <iostream>
#include "GPU.h"

using namespace std;

#ifndef MMU_H
#define MMU_H

class MMU
{
public:

	struct MBC
	{
	
		int rombank;
		int rambank;
		int ramon; 
		bool mode;
	
	};
	
	static MBC _mbc;

	MMU();
	void init();
	void reset();
	char rb(int addr);				//Odczytaj bajt z adresu
	short rw(int addr);				//Odczytaj slowo z adresu
	void wb(char byte, int addr);	//Zapisz bajt pod adres
	void ww(short word, int addr);	//Zapisz slowo pod adres
	vector<char> load(char* filename);	//Zaladowanie Romu 
	
	GPU* _gpu;
	
private:
	vector<char> _rom;				
	vector<char> _eram;
	vector<char> _bios;
	vector<char> _wram; 
	vector<char> _zram;

	char _carttype;				
	int _inbios;
	int _ie;					//Interrupt enable (Lokacja w pamieci: FFFF) 
	int _if;					//Interrupt flags (Lokacja w pamieci: FF0F) 
	char _romoffs;
	char _ramoffs;
};
#endif



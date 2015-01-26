#include <vector>
#include <iostream>
#include "GPU.h"
#include "Input.h"

using namespace std;

#ifndef MMU_H
#define MMU_H

class GPU;
class Input;

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
	
	MBC _mbc;

	MMU();
	void init();
	void reset();
	unsigned char rb(unsigned short addr);				//Odczytaj bajt z adresu
	unsigned short rw(unsigned short addr);				//Odczytaj slowo z adresu
	void wb(unsigned char byte, unsigned short addr);	//Zapisz bajt pod adres
	void ww(unsigned short word, unsigned short addr);	//Zapisz slowo pod adres
	
	vector<char> load(char* filename);					//Zaladowanie Romu 
	
	unsigned char _ie;					//Interrupt enable (Lokacja w pamieci: FFFF) 
	unsigned char _if;					//Interrupt flags (Lokacja w pamieci: FF0F) 
	
	vector<char> _bios;
	

	GPU *_gpu;
	Input *_input;
	
	
private:
	vector<char> _rom;				
	vector<char> _eram;
	//vector<char> _bios;
	vector<char> _wram; 
	vector<char> _zram;

	char _carttype;				
	int _inbios;
	unsigned short _romoffs;
	unsigned short _ramoffs;
};
#endif



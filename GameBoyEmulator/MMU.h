#ifndef MMU_H
#define MMU_H

#include <vector>
#include <iostream>
using namespace std;

class MMU
{
public:
	MMU();
	void init();
	unsigned char rb(unsigned short addr);				//Odczytaj bajt z adresu
	short rw(unsigned short addr);				//Odczytaj slowo z adresu
	void wb(unsigned char byte, unsigned short addr);	//Zapisz bajt pod adres
	void ww(unsigned short word, unsigned short addr);	//Zapisz slowo pod adres
	vector<char> load(char* filename);	//Zaladowanie Romu 

private:
	vector<char> _rom;				
	vector<char> _eram;
	vector<char> _bios;
	vector<char> _wram; 
	vector<char> _zram;

	char _carttype;				
	int _inbios;
};
#endif

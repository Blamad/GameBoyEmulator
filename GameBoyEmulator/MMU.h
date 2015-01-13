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
	char rb(int addr);				//Odczytaj bajt z adresu
	short rw(int addr);				//Odczytaj slowo z adresu
	void wb(char byte, int addr);	//Zapisz bajt pod adres
	void ww(short word, int addr);	//Zapisz slowo pod adres
	vector<char> load(char* filename);	//Zaladowanie Romu 

private:
	vector<char> _rom;				
	vector<char> _eram;
	vector<char> _bios;
	vector<char> _wram; 
	vector<char> _zram;

	char _carttype;					//Typ kartidza
	int _inbios;
};
#endif

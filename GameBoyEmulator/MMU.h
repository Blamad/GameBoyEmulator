#ifndef MMU_H
#define MMU_H

class MMU
{
public:
	MMU();
	char rb(int addr);				//Odczytaj bajt z adresu
	short rw(int addr);				//Odczytaj slowo z adresu
	void wb(char byte, int addr);	//Zapisz bajt pod adres
	void ww(short word, int addr);	//Zapisz slowo pod adres
}
#endif
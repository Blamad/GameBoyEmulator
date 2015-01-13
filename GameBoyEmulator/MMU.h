#ifndef MMU_H
#define MMU_H

class MMU
{
public:
	MMU();
	char rb(int addr);				//Odczytaj bajt z adresu
	short rw(int addr);				//Odczytaj slowo z adresu
	void wb(int addr, char byte);	//Zapisz bajt pod adres
	void ww(int addr, short word);	//Zapisz slowo pod adres
}
#endif
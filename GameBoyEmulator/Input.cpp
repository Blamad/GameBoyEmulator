#include "Input.h"

#include <iostream>

/*
Pamiêtaj, 0 to przycisk aktywny, 1 to puszczony

Odczytywanie przycisków jest tu nieco pokiækane.

		   --------------|5
		   |   /---------|4
Down	---o--o--Start---|3	JOYP
Up		---o--o--Select--|2
Left	---o--o----B-----|1
Right	---o--o----A-----|0

Sprawdzenie który przycisk zosta³ wciœniêty odbywa siê przez aktywacjê jednej z kolumn (4 lub 5),
odczekanie chwili na rozpropagowanie sygna³u do JOYP i sprawdzenie która kolumna (0-3) jest aktywna.
(Je¿eli jest wciœniêty przycisk to sygna³ po zadaniu napiêcia wróci do JOYP pod portem (0-3).
Wtedy wiemy który przycisk zosta³ uruchomiony.
*/

Input::Input()
{
	std::cout<<"Input wstaje"<<std::endl;
}

void Input::init()
{
	reset();
}


void Input::reset() 
{
	_keyRow[0]=0x0F;
	_keyRow[1]=0x0F;
	_column=0;  
}

unsigned char Input::rb() 
{
	switch(_column)
	{
		case 0x00: return 0x00; break;
		case 0x10: return _keyRow[0]; break;
		case 0x20: return _keyRow[1]; break;
		default: return 0x00; break;
	}
}

void Input::wb(unsigned char byte) 
{
	_column=byte&0x30;
}

void Input::keyDown(unsigned char key, int x, int y) 
{    
	switch(key)
	{
		case KEY_RIGHT:	_keyRow[1] &= 0xE; break;
		case KEY_LEFT:	_keyRow[1] &= 0xD; break;
		case KEY_UP:	_keyRow[1] &= 0xB; break;
		case KEY_DOWN:	_keyRow[1] &= 0x7; break;
		case KEY_A:		_keyRow[0] &= 0xE; break;
		case KEY_B:		_keyRow[0] &= 0xD; break;
		case KEY_SELECT:_keyRow[0] &= 0xB; break;
		case KEY_START:	_keyRow[0] &= 0x7; break;
	}
}

void Input::keyUp(unsigned char key, int x, int y) 
{
	switch(key)
	{
		case KEY_RIGHT:	_keyRow[1] |= 0x1; break;
		case KEY_LEFT:	_keyRow[1] |= 0x2; break;
		case KEY_UP:	_keyRow[1] |= 0x4; break;
		case KEY_DOWN:	_keyRow[1] |= 0x8; break;
		case KEY_A:		_keyRow[0] |= 0x1; break;
		case KEY_B:		_keyRow[0] |= 0x2; break;
		case KEY_SELECT:_keyRow[0] |= 0x5; break;
		case KEY_START:	_keyRow[0] |= 0x8; break;
	}
}
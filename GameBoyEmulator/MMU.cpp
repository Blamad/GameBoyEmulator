#include "MMU.h"
#include "Z80.h"

#include <vector>
#include <fstream>
using namespace std;

/*

Odpowiada za wczytywanie do i z pamiÄ™ci o podanym adresie danych z romu lub nie wiem czego..
Nie interesowaÅ‚em siÄ™ tym jeszcze. Na razie tylko uÅ¼ywane w poleceniach procesora, nie wiem czy nie bÄ™dzie uÅ¼ywane gdzieÅ› jeszcze.
TODO.

*/


MMU::MMU()
{
	init();
}

void MMU::init()
{
	 _rom = load("file.GB");		  
	 _carttype = _rom[0x0147];		
	 _inbios = 1;

	 _bios = { };
}

char MMU::rb(unsigned short addr)
{
	switch(addr & 0xF000) 
		{  
			 // BIOS (256b) i ROM0 
			 case 0x0000: 
				if(_inbios) 
				{ 
					if(addr < 0x0100)
						return _bios[addr]; 
					else if(Z80::_r.pc == 0x0100) 
						_inbios = 0; 
				} 
				return _rom[addr];

			 // ROM0 
			 case 0x1000: 
			 case 0x2000: 
			 case 0x3000: 
				return _rom[addr]; 
			 
			//ROM(switched bank) 
			case 0x4000: 
			case 0x5000: 
			case 0x6000: 
			case 0x7000: 
				return _rom[0x4000 + (addr & 0x3FFF)]; 
				
			//Zewnetrzny RAM 
			case 0xA000: 
			case 0xB000: 
				return _eram[0x0000 + (addr & 0x1FFF)]; 

			//VRAM 
			case 0x8000: 
			case 0x9000: 
				//return _vram[addr & 0x1FFF]; 
			
			// Working RAM
			case 0xC000: 
			case 0xD000: 
				return _wram[addr & 0x1FFF]; 
			
			// Working RAM shadow 
			case 0xE000: 
				return _wram[addr & 0x1FFF]; 
			
			// Working RAM shadow, I/O, Zero-page RAM 
			case 0xF000: 
				switch(addr & 0x0F00) 
				{ 
					// Working RAM shadow 
					case 0x000: case 0x100: case 0x200: 
					case 0x300: case 0x400: case 0x500: 
					case 0x600: case 0x700: case 0x800: 
					case 0x900: case 0xA00: case 0xB00: 
					case 0xC00: case 0xD00: 
						return _wram[addr & 0x1FFF]; 
					
					// OAM
					case 0xE00: 
						//if(addr < 0xFEA0) return _oam[addr & 0xFF]; 
						//else return 0; 
					
					// Zero-page 
					case 0xF00: 
						if(addr >= 0xFF80) { 
							return _zram[addr & 0x7F]; 
						}
						else { 
							// I/O
							return 0; 
						} 
				
				}
		} 
}

//Zapisz bajt pod adresem
void MMU::wb(unsigned char byte, unsigned short addr)
{
	switch(addr & 0xF000)
	{
		/*
		//Musi byæ tutaj ten case, ¿eby updateowaæ moje tile z gpu!
        case 0x9000:
		_vram[addr & 0x1FFF] = byte;	//tablica z gpu
		updatetile(addr); //Metoda z gpu
		break;
		*/
	}
}
//Odczytaj s³owo z adresu
short MMU::rw(unsigned short addr)
{
	return rb(addr) + (rb(addr+1) << 8);
}
//Zapisz s³owo pod adresem
void MMU::ww(unsigned short word, unsigned short addr)
{

}

vector<char> MMU::load(char* filename) 
{ 	 
	ifstream ifs(filename, ios::binary|ios::ate);
	ifstream::pos_type pos = ifs.tellg();
 
	vector<char> result(pos);
 
	ifs.seekg(0, ios::beg);
	ifs.read(&result[0], pos);
	 
	return result;	
}

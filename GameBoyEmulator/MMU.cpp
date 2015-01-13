#include "MMU.h"
#include "Z80.h"

#include <vector>
#include <fstream>
using namespace std;

/*

Odpowiada za wczytywanie do i z pamięci o podanym adresie danych z romu lub nie wiem czego..
Nie interesowałem się tym jeszcze. Na razie tylko używane w poleceniach procesora, nie wiem czy nie będzie używane gdzieś jeszcze.
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

char MMU::rb(int addr)
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
				
			//External RAM (8k)
			case 0xA000: 
			case 0xB000: 
				return _eram[0x0000 + (addr & 0x1FFF)]; 

			// Graphics: VRAM (8k) 
			case 0x8000: 
			case 0x9000: 
				//return GPU._vram[addr & 0x1FFF]; 
			
			// Working RAM (8k) 
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
					case 0x000: 
					case 0x100: 
					case 0x200: 
					case 0x300: 
					case 0x400: 
					case 0x500: 
					case 0x600: 
					case 0x700: 
					case 0x800: 
					case 0x900: 
					case 0xA00: 
					case 0xB00: 
					case 0xC00: 
					case 0xD00: 
						return _wram[addr & 0x1FFF]; 
					
					// Graphics: object attribute memory 
					// OAM is 160 bytes, remaining bytes read as 0 
					case 0xE00: 
						//if(addr < 0xFEA0) return GPU._oam[addr & 0xFF]; 
						//else return 0; 
					
					// Zero-page 
					case 0xF00: 
						if(addr >= 0xFF80) { 
							return _zram[addr & 0x7F]; 
						}
						else { 
							// I/O control handling 
							// Currently unhandled 
							return 0; 
						} 
				
				}
		} 
}

void MMU::wb(char byte, int addr)
{

}

short MMU::rw(int addr)
{
	return rb(addr) + (rb(addr+1) << 8);
}

void MMU::ww(short word, int addr)
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

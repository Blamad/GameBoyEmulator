#include "MMU.h"
#include "Z80.h"
#include "GPU.h"

#include <vector>
#include <fstream>
using namespace std;

/*

Odpowiada za wczytywanie do i z pamięci o podanym adresie danych z romu lub nie wiem czego..
Nie interesowałem się tym jeszcze. Na razie tylko używane w poleceniach procesora, nie wiem czy nie będzie używane gdzieś jeszcze.
TODO.

*/

//-------------------------------------------------------------------------------------
MMU::MMU()
{

}
//-------------------------------------------------------------------------------------
void MMU::init()
{
	 _rom = load("GAME.GB");			
	 _bios = load("BIOS.BIN");

	 reset();
}
//-------------------------------------------------------------------------------------
void MMU::reset() 
{

	for(int i=0; i<8192; i++) 
		_wram[i] = 0;
	
	for(int i=0; i<32768; i++) 
		_eram[i] = 0;
	
	for(int i=0; i<127; i++) 
		_zram[i] = 0;
	
	_inbios = 1;
	
	_ie = 0;
	_if = 0;
	
	_carttype = 0;

	_mbc.rombank = 0;
	_mbc.rambank = 0;
	_mbc.ramon = 0;
	_mbc.mode = 0;

	_romoffs = 0x4000;
	_ramoffs = 0;

}
//-------------------------------------------------------------------------------------
unsigned char MMU::rb(unsigned short addr)
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
				else
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
				return _rom[_romoffs + (addr & 0x3FFF)]; 
				
			//External RAM 
			case 0xA000: 
			case 0xB000: 
				return _eram[_ramoffs + (addr & 0x1FFF)]; 

			// Graphics: VRAM 
			case 0x8000: 
			case 0x9000: 
				return _gpu->_vram[addr & 0x1FFF]; 
			
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
					
					// Grafika -> OAM (Object Attribute Memory) 
					case 0xE00: 
						if(addr < 0xFEA0) return _gpu->_oam[addr & 0xFF]; 
						else return 0; 
					
					// Zero-page 
					case 0xF00:
						if(addr == 0xFFFF) 
						{ 
							return _ie; //Interrupt enable 
						}
						else if(addr > 0xFF7F) 
						{ 
							return _zram[addr & 0x7F]; 
						}
						else switch(addr & 0xF0)
						{
							case 0x00:
								switch(addr & 0xF)
								{
									case 0: 
										return _input->rb();		// JOYP
									case 4: case 5: case 6: case 7:
										//return TIMER::rb(addr);
									case 15: 
										return _if;					// Interrupt flags
									default: 
										return 0;
								}
							
							case 0x10: case 0x20: case 0x30:
								return 0;
							
							case 0x40: case 0x50: case 0x60: case 0x70:
								return _gpu->rb(addr);
						}
				
				}
		} 
}
//-------------------------------------------------------------------------------------
void MMU::wb(unsigned char byte, unsigned short addr)
{
	switch(addr & 0xF000)
	{
	// ROM bank 0
	// MBC1 -> wlaczony external RAM
	case 0x0000: 
	case 0x1000:
		switch(_carttype)
		{
			case 1:
			_mbc.ramon = ((byte & 0xF) == 0xA) ? 1 : 0;
			break;
		}
		break;

	// MBC1: ROM bank switch
	case 0x2000: 
	case 0x3000:
		switch(_carttype)
		{
			case 1:
				byte &= 0x1F;
				if(!byte) 
					byte = 1;
				_mbc.rombank = (_mbc.rombank & 0x60) + byte;
				
				//Obliczenie offsetu ROMu
				_romoffs = _mbc.rombank * 0x4000;
				break;
		}
		break;

	// ROM bank 1
	// MBC1 -> RAM bank switch
	case 0x4000: 
	case 0x5000:
		switch(_carttype)
		{
			case 1:
				if(_mbc.mode)
				{
					_mbc.rambank = ( byte & 3 );
					_ramoffs = _mbc.rambank * 0x2000;
				}
				else
				{
					_mbc.rombank = (_mbc.rombank & 0x1F) + ((byte & 3) << 5);
					_romoffs = _mbc.rombank * 0x4000;
				}
		}
		break;
	
	case 0x6000: 
	case 0x7000:
		switch(_carttype)
		{
			case 1:
			_mbc.mode = byte & 1;
			break;
		}
		break;
	
	// VRAM
	case 0x8000: 
	case 0x9000:
		_gpu->_vram[addr & 0x1FFF] = byte;
		_gpu->updateTile( addr & 0x1FFF );
		break;
	
	// External RAM
	case 0xA000: 
	case 0xB000:
		_eram[_ramoffs + ( addr & 0x1FFF )] = byte;
		break;

	// Work RAM i echo
	case 0xC000: 
	case 0xD000: 
	case 0xE000:
		_wram[addr & 0x1FFF] = byte;
		break;

	case 0xF000:
		switch(addr & 0x0F00)
		{
			// Echo RAM
			case 0x000: case 0x100: case 0x200: case 0x300:
			case 0x400: case 0x500: case 0x600: case 0x700:
			case 0x800: case 0x900: case 0xA00: case 0xB00:
			case 0xC00: case 0xD00:
				_wram[addr & 0x1FFF] = byte;
				break;
		
			// OAM
			case 0xE00:
				if((addr & 0xFF) < 0xA0) _gpu->_oam[addr & 0xFF] = byte;
				_gpu->updateObjData(addr, byte); //tak ma zostać
				break;
		
			// Zeropage RAM, I/O, interrupts
			case 0xF00:
				if(addr == 0xFFFF) 
				{ 
					_ie = byte; 
				}
				else if(addr > 0xFF7F) 
				{ 
					_zram[addr & 0x7F] = byte; 
				}
				else switch(addr & 0xF0) //IO
				{
					case 0x00:
					switch(addr & 0xF)
					{
						case 0: 
							_input->wb(byte); 
							break;
						
						case 4: case 5: case 6: case 7: 
							//TIMER.wb(addr, byte); 
							break;
						
						case 15: 
							_if = byte; 
							break;
					}
					break;

					case 0x10: case 0x20: case 0x30:
					break;
					
					//GPU
					case 0x40: case 0x50: case 0x60: case 0x70:
					_gpu->wb(addr, byte);
					break;
				}
		}
		break;
	}
}
//-------------------------------------------------------------------------------------
unsigned short MMU::rw(unsigned short addr)
{
	return rb(addr) + (rb(addr+1) << 8);
}
//-------------------------------------------------------------------------------------
void MMU::ww(unsigned short word, unsigned short addr)
{
	 wb(addr, word & 255); 
	 wb(addr + 1, word >> 8);
}
//-------------------------------------------------------------------------------------
vector<char> MMU::load(char* filename) 
{ 	 
	ifstream ifs(filename, ios::binary|ios::ate);
	ifstream::pos_type pos = ifs.tellg();
 
	vector<char> result(pos);
 
	ifs.seekg(0, ios::beg);
	ifs.read(&result[0], pos);
	
	_carttype = result[0x0147];	

	return result;	
}
//-------------------------------------------------------------------------------------

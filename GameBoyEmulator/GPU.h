#include "Input.h"

#ifndef GPU_H
#define GPU_H

class GPU
{
public:
	GPU();
	~GPU();

	Input* _input;
	MMU* _mmu;

	static int pixelSize;
	unsigned char _mode;
	unsigned short _sCX, _sCY;
	unsigned short _modeClock;
	unsigned short _line; //Line i scan s¹ tam rozdzielone. Czemu?
	
								//GPU control register	0	1
	bool _bgOn,					//0	Background: on/off		Off	On
		_objOn,					//1	Sprites: on/off			Off	On
		_objSize;				//2	Sprites: size (pixels)	8x8	8x16
	unsigned short	_bgMapSet,	//3	Background: tile map	#0	#1
					_bgTileSet;	//4	Background: tile set	#0	#1
								//5	Window: on/off			Off	On
								//6	Window: tile map		#0	#1
	bool _lcdOn;				//7	Display: on/off			Off	On

	struct Palette
	{
		unsigned char bg[3];
		unsigned char obj0[3];
		unsigned char obj1[3];

		unsigned char mapThrough(unsigned char* pal, unsigned char color);
	};

	Palette _palette;

	/*Kazdy obiekt (jest ich 40) jest opisywany czterema bajtami
		0.pozycja y lewego górnego rogu
		1.pozycja x lewego górnego rogu
		2.numer opisywanego kafelka
		3.bity:
			4. paleta (0: pal0, 1: pal1)
			5. czy obraz powinien byæ odwrócony wg. osi x (0 - nie)
			6. czy obraz powinien byæ odwrócony wg. osi y (0 - nie)
			7. priorytet wzglêdem t³a (0: nad t³em, 1: pod t³em)
	*/
	struct ObjData
	{
		short y,
			x;
		unsigned short tileNum;
		bool priority,
			yFlip,
			xFlip,
			palette;
	};

	ObjData _objData[40];

	unsigned char _tileSet[512][8][8];

	unsigned char _vram[8192]; //TO NIE IWEM CZY JEST DOBRZE
	unsigned char _oam[160];

	bool _scanRow[160];

	void init();
	void draw();
	void reset();
	void step();
	void scanLine();
	void updateTile(short addr);
	void updateObjData(unsigned short addr, unsigned char byte);
	unsigned char rb(unsigned short addr);
	void wb(unsigned char byte, unsigned short addr);

private:
	//Bufor trzyma wartoœæ koloru jak¹ ma wyœwietliæ w zadanym miejscu funkcja draw
	unsigned char _bufor[160][144*3];
	double lastTime;

	void initGlut();
	void writeColorToBufor(unsigned char color, int x, int y);
};
#endif
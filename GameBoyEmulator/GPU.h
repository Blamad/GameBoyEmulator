
#ifndef GPU_H
#define GPU_H

class GPU
{
public:
	GPU();
	~GPU();
	void init();
	void draw();
	void reset();
	void step();
	void scanLine();
	void updateTile(short addr);
	unsigned char rb(unsigned short addr);
	void wb(unsigned char byte, unsigned short addr);

	static int pixelSize;
	unsigned char _mode;
	unsigned short _sCX, _sCY;
	unsigned short _modeClock;
	unsigned short _line; //Line i scan s¹ tam rozdzielone. Czemu?
	
								//GPU control register	0	1
	bool _bgOn;					//0	Background: on/off		Off	On
								//1	Sprites: on/off			Off	On
								//2	Sprites: size (pixels)	8x8	8x16
	unsigned short	_bgMapSet,	//3	Background: tile map	#0	#1
					_bgTileSet;	//4	Background: tile set	#0	#1
								//5	Window: on/off			Off	On
								//6	Window: tile map		#0	#1
	bool _lcdOn;				//7	Display: on/off			Off	On

	struct Palette
	{
		unsigned char bg[3];
	};

	Palette _palette;
	unsigned char _tileSet[384][8][8];
	//unsigned char _tileMap[];

	static unsigned char _vram[8192]; //TO NIE IWEM CZY JEST DOBRZE

private:
	//Bufor trzyma wartoœæ koloru jak¹ ma wyœwietliæ w zadanym miejscu funkcja draw
	unsigned char _bufor[160][144*3];
	double lastTime;

	void initGlut();
	void mapColorThroughPalette(unsigned char color, int x, int y);
};
#endif
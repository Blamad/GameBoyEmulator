
#ifndef GPU_H
#define GPU_H

class GPU
{
public:
	GPU();
	~GPU();
	void draw();
	void reset();
	void step();
	void scanLine();

	static int pixelSize;
	unsigned char _mode;
	unsigned short _sCX, _sCY;
	unsigned short _modeClock;
	unsigned short _line;
	bool _bgmap;
	bool _bgTile;
	unsigned char _tileSet[384][8][8];

	static unsigned char _vram[8192]; //TO NIE IWEM CZY JEST DOBRZE
	void updateTile(short addr);

private:
	unsigned char _bufor[160][144*3];
	double lastTime;
};
#endif
#ifndef INPUT_H
#define INPUT_H

class Input
{
public:

	Input();
	//~Input();

	static const unsigned char KEY_UP = 'w',
		KEY_DOWN='s',
		KEY_LEFT='a', 
		KEY_RIGHT='d', 
		KEY_A='k',
		KEY_B='l',
		KEY_SELECT='i',
		KEY_START='o';

	void init();
	void reset();

	void keyUp(unsigned char key, int x, int y);
	void keyDown(unsigned char key, int x, int y);
	
	void wb(unsigned char byte);
	unsigned char rb();

private:
	unsigned char _keyRow[2];
	unsigned char _column;

};
#endif
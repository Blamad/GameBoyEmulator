#include "GPU.h"
#include "Z80.h"
#include "Libs\freeglut\freeglut.h"

void GPU::step()
{
	_modeClock += Z80::_r.t;

	switch(_mode)
	{
	    // OAM read mode, scanline active
	    case 2:
	        if(_modeClock >= 80)
			{
				// Enter scanline mode 3
				_modeClock = 0;
				_mode = 3;
			}
			break;

	    // VRAM read mode, scanline active
	    // Treat end of mode 3 as end of scanline
	    case 3:
	        if(_modeClock >= 172)
			{
				// Enter hblank
				_modeClock = 0;
				_mode = 0;

				// Wyrysuj linie do bufora obrazu
				scanLine();
			}
			break;

	    // Hblank
	    // After the last hblank, push the screen data to canvas
	    case 0:
	        if(_modeClock >= 204)
			{
				_modeClock = 0;
				_line++;

				if(_line == 143)
				{
					// Enter vblank
					_mode = 1;
					//Wyrysuj powstaly obraz
					glutPostRedisplay();
				}
				else
				{
		    		_mode = 2;
				}
		}
		break;

	    // Vblank (10 lines)
	    case 1:
	        if(_modeClock >= 456)
			{
				_modeClock = 0;
				_line++;

				if(_line > 153) ///Czemu?
				{
					// Restart scanning modes
					_mode = 2;
					_line = 0;
				}
			}
		break;
	}
}

void GPU::draw()
{
	glBegin(GL_QUADS);
	for(int xPos = 0; xPos < 160; xPos++ )
		for(int yPos = 0; yPos < 144; yPos++ )
		{
			glColor3f((float)_bufor[xPos][3*yPos]/255, (float)_bufor[xPos][3*yPos+1]/255, (float)_bufor[xPos][3*yPos+2]/255); 
			glVertex2f(xPos*pixelSize, yPos*pixelSize);
			glVertex2f(xPos*pixelSize+pixelSize, yPos*pixelSize);
			glVertex2f(xPos*pixelSize+pixelSize, yPos*pixelSize+pixelSize);
			glVertex2f(xPos*pixelSize, yPos*pixelSize+pixelSize);
		}

	glEnd();
	glutSwapBuffers();
	glFlush();
}

void GPU::initGlut()
{
	//Ukrycie okna konsoli
	/*HWND hWnd = GetConsoleWindow();
	ShowWindow( hWnd, SW_HIDE );*/

	char *myargv [1];
	int myargc=1;
	myargv [0]=_strdup("GB Emulator");
	glutInit(&myargc, myargv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 100);//pozycja
	glutInitWindowSize(160*pixelSize, 144*pixelSize); //rozmiar
	glutCreateWindow("GB Emulator");
 
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION | GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0, 160*pixelSize, 144*pixelSize, 0);

	
	glutIdleFunc(this->step);
	glutDisplayFunc(this->draw);
	
	glutMainLoop();
}

GPU::GPU()
{
	
}

void GPU::init()
{
	pixelSize = 2;
	_line = 0;
	_mode = 0;
	_modeClock = 0;
	_bgMapSet=0x1800;
	_bgTileSet=0x0000;
	_lcdOn=0;
	_bgOn=0;
	_sCX=0;
	_sCY=0;

	initGlut();
}

void GPU::reset()
{
	//Takie tam na teraz..
	_bgMapSet=0x1800;
	_bgTileSet=0x0000;
	_lcdOn=0;
	_bgOn=0;
	_sCX=0;
	_sCY=0;
	_line=0;
	_mode=0;
	_modeClock=0;

	//Bufor od grafiki sk³ada siê z pamiêci 160x144 pikseli z których ka¿dy ma okreœlony kolor RGB.
	//x to R, x+1 to G i x+2 to B
	//wyczyszczenie t³a..
	for(int xPos = 0; xPos < 160; xPos++ )
	{
		for(int yPos = 0; yPos < 144; yPos++ )
		{
			_bufor[xPos][yPos*4]=255;
			_bufor[xPos][yPos*4+1]=255;
			_bufor[xPos][yPos*4+2]=255;
		}
	}

	//zainicjowanie pamiêci tileset (mamy 384 teksturki, z czego ka¿da ma rozmiar 8x8 pikseli.
	//Ka¿dy piksel to dwa bity pamiêci(w gb) opisuj¹ce kolor bitu od 0 do 3.
	for(int tile = 0; tile < 384; tile++)
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 8; j++)
				_tileSet[tile][i][j] = 0;
}

void GPU::updateTile(short addr)
{
	int vramIndex = addr &0x1FFE;

	int tile = vramIndex >> 4 & 511;
	int y = vramIndex >> 1 & 7;

	short pxIndex;
	for(int x = 0; x < 8; x++)
	{
		pxIndex = 1 << (7-x);
		_tileSet[tile][x][y] = ((_vram[addr]&pxIndex)? 1:0) + ((_vram[addr+1] & pxIndex)? 2:0);
	}
}

void GPU::scanLine()
{
	
	if(_lcdOn)
    {
        if(_bgOn)
        {

			int linebase = _line;
			//int mapOffs = _bgMapSet + ((_line + _sCY) & 255)>>3;
			int mapBase = _bgMapSet + ((((_line+_sCY)&255)>>3)<<5);
			int y = (_line+_sCY)&7;	//ktore piksele z tile pobierac
			int x = _sCX&7;			//gdzie zaczyna sie linia tile'a
			int tileOffs = (_sCX>>3)&31;	//Ktory tile bede obrabial
		
			if(_bgTileSet)
			{
				int tilePos = _vram[mapBase + tileOffs];
				if(tilePos<128) tilePos+=256;
				for(int xPos = 0; xPos < 160; xPos++)
				{
					mapColorThroughPalette(_tileSet[tilePos][x][y], xPos, _line);
					x++;
			
					if(x == 8)
					{
						x = 0;
						tileOffs = (tileOffs + 1) & 31;
						tilePos = _vram[mapBase + tileOffs];
						if(tilePos < 128) 
							tilePos += 256;
					}
				}
			}
			/*else
			{
				int tileRow=_tileMap[_vram[mapBase+tileOffs]][y];
				for(int xPos = 0; xPos < 160; xPos++)
				{
					mapColorThroughPalette(_tileSet[tileRow][x][y], xPos, _line);
					x++;
					if(x==8) 
					{ 
						tileOffs=(tileOffs+1)&31; 
						x=0; 
					}
				}
			}*/
		}
	}
}

void GPU::mapColorThroughPalette(unsigned char color, int x, int y)
{
		//Pociagnij kolor z tego szajsu i ustaw tu odpowiedni do wyœwietlania
		switch(_palette.bg[color])
		{
			case 0:
				_bufor[x][y]=255;
				_bufor[x][y+1]=255;
				_bufor[x][y+2]=255;
				break;
			case 1:
				_bufor[x][y]=192;
				_bufor[x][y+1]=192;
				_bufor[x][y+2]=192;
				break;
			case 2:
				_bufor[x][y]=96;
				_bufor[x][y+1]=96;
				_bufor[x][y+2]=96;
				break;
			case 3:
				_bufor[x][y]=0;
				_bufor[x][y+1]=0;
				_bufor[x][y+2]=0;
				break;
		}
}

unsigned char GPU::rb(unsigned short addr)
{
	char addrOff = addr-0xFF40;
	switch(addrOff)
	{
	    // LCD Control
	    case 0:
	        return 
				(_lcdOn?0x80:0)|
				((_bgTileSet==0x0000)?0x10:0)|
				((_bgMapSet==0x1C00)?0x08:0)|
				//(_objSize?0x04:0)|
				//(_objOn?0x02:0)|
				(_bgOn?0x01:0);
	    // Scroll Y
	    case 2:
	        return _sCY;

	    // Scroll X
	    case 3:
	        return _sCX;

	    // Current scanline
	    case 4:
	        return _line;
	}
}

void GPU::wb(unsigned char byte, unsigned short addr)
{
	char addrOff = addr-0xFF40;
	switch(addrOff)
	{
	    // LCD Control
	    case 0:
	        _lcdOn = (byte&0x80)?1:0;
			_bgTileSet = (byte&0x10)?0x0000:0x0800;
			_bgMapSet = (byte&0x08)?0x1C00:0x1800;
			//_objsize = (byte&0x04)?1:0;
			//_objon = (byte&0x02)?1:0;
	        _bgOn = (byte&0x01)?1:0;

		break;

	    // Scroll Y
	    case 2:
	        _sCY = byte;
		break;

	    // Scroll X
	    case 3:
	        _sCX = byte;
		break;

	    // Background palette
	    case 7:
	        for(int i = 0; i < 3; i++)
			{
				switch((byte>>(i*2))&3)
				{
					case 0: _palette.bg[i] = 255; break;
					case 1: _palette.bg[i] = 192; break;
					case 2: _palette.bg[i] = 96; break;
					case 3: _palette.bg[i] = 0; break;
				}
			}
		break;
	}
}
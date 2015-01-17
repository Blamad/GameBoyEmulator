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

				// Write a scanline to the framebuffer
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
					draw();
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

GPU::GPU()
{
	pixelSize = 2;
	_line = 0;
	_mode = 0;
	_modeClock = 0;

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

void GPU::reset()
{
	//Takie tam na teraz..

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
	// VRAM offset for the tile map
	int mapOffs = _bgmap ? 0x1C00 : 0x1800;

	// Which line of tiles to use in the map
	//mapoffs += ((GPU._line + GPU._scy) & 255) >> 3;
	mapOffs += ((_line + _sCY) & 255)>>3;

	// Which tile to start with in the map line
	//var lineoffs = (GPU._scx >> 3);
	int lineOffs = _sCX>>3;

	// Which line of pixels to use in the tiles
	//var y = (GPU._line + GPU._scy) & 7;
	int y = (_line + _sCY)&7;

	// Where in the tileline to start
	//var x = GPU._scx & 7;
	int x = _sCX&7;

    // Read tile index from the background map
	unsigned char color;
	
	//var tile
	int tilePos = _vram[mapOffs + lineOffs];

	// If the tile data set in use is #1, the
	// indices are signed; calculate a real tile offset
	if(_bgTile == 1 && tilePos < 128) tilePos += 256;

	for(int xPos = 0; xPos < 160; xPos++)
	{
	    // Re-map the tile pixel through the palette
	    color=_tileSet[tilePos][x][y];
		//Pociagnij kolor z tego szajsu i ustaw tu odpowiedni do wyœwietlania

		switch(color)
		{
			case 0:
				_bufor[xPos][_line*3]=255;
				_bufor[xPos][_line*3+1]=255;
				_bufor[xPos][_line*3+2]=255;
				break;
			case 1:
				_bufor[xPos][_line*3]=192;
				_bufor[xPos][_line*3+1]=192;
				_bufor[xPos][_line*3+2]=192;
				break;
			case 2:
				_bufor[xPos][_line*3]=96;
				_bufor[xPos][_line*3+1]=96;
				_bufor[xPos][_line*3+2]=96;
				break;
			case 3:
				_bufor[xPos][_line*3]=0;
				_bufor[xPos][_line*3+1]=0;
				_bufor[xPos][_line*3+2]=0;
				break;
		}

		x++;
	    if(x == 8)
	    {
			x = 0;
			lineOffs = (lineOffs + 1) & 31;
			tilePos = _vram[mapOffs + lineOffs];
			if(_bgTile && tilePos < 128) tilePos += 256;
	    }
	}
}

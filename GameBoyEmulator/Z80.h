#include "MMU.h"

#ifndef Z80_H
#define Z80_H

class Z80
{
public:
	
	Z80();
	~Z80();
	void init();
	void reset();
	void dispatch();
private:
	struct R 
	{
		int a, b, c, d, e, h, l,	//Rejestry 8 bitowe
		pc, sp,						//Rejestry 16b
		m, t,						//Rejestry zegara
		r;							//Na wykonywana aktualnie procedure
	};

	struct Clock
	{
		int m, t;
	};

	typedef void (Z80::*OpCodeMap)();

	R _r;
	Clock _clock;
	OpCodeMap _map;
	MMU mmu;

	//Lista instrukcji
	void NOP();
	void LDBCnn();
	void LDBCmA();
	void INCBC();
	void INCr_b();
	void DECr_b();
	void LDrn_b();
	void RLCA();
	void LDmmSP();
	void ADDHLBC();
	void LDABCm();
	void DECBC();
	void INCr_c();
	void DECr_c();
	void LDrn_c();
	void RRCA();
	
	/*
	();
	
	*/
};
#endif
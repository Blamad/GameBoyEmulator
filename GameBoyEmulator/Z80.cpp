
#include "Z80.h"

Z80::Z80()
{
	init();
}

Z80::~Z80()
{
}

void Z80::reset()
{
	_r.a = 0;
	_r.b = 0;
	_r.c = 0;
	_r.d = 0;
	_r.e = 0;
	_r.h = 0;
	_r.l = 0;
	_r.pc = 0;
	_r.sp = 0;
	_r.m = 0;
	_r.t = 0;
	_r.r = 0;

	_clock.m = 0;
	_clock.t = 0;
}

void Z80::dispatch()
{
	_r.r = (_r.r + 1) & 127; //tlumaczenie pobranego polecenia na kod. Nie wiem czy to dziala jak powinno, byæ mo¿e jest zbêdne.
	_map[mmu.rb(_r.pc++)]();
	_r.pc &= 65535;
	_clock.m += _r.m;
}

void Z80::init()
{
	R _r = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Clock _clock = {0, 0};
	OpCodeMap _map[] = 
	{ 
		//00
		NOP,		LDBCnn,		LDBCmA,		INCBC,
		INCr_b,		DECr_b,		LDrn_b,		RLCA,
		LDmmSP,		ADDHLBC,	LDABCm,		DECBC,
		INCr_c,		DECr_c,		LDrn_c,		RRCA,
		//10

	};
}

//00
void Z80::NOP() { _r.m = 1; }
void Z80::LDBCnn() { }
void Z80::LDBCmA() { }
void Z80::INCBC() { }
void Z80::INCr_b() { }
void Z80::DECr_b() { }
void Z80::LDrn_b() { }
void Z80::RLCA() { }
void Z80::LDmmSP() { }
void Z80::ADDHLBC() { }
void Z80::LDABCm() { }
void Z80::DECBC() { }
void Z80::INCr_c() { }
void Z80::DECr_c() { }
void Z80::LDrn_c() { }
void Z80::RRCA() { }
//10
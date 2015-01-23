
#include "Z80.h"
#include "MMU.h"
#include <iostream>
/*
Procek.
Prosty schemat dzia³ania: 
- pobierz	(funkcja dispatch pobiera pierwszy bajt pod adresem (czyli sam rozkaz)
- dekoduj	(w tablicy _map trzymane s¹ wszystkie funkcje pod indeksami odpowiadaj¹cymi ich reprezentacji binarnej (np. x00h = rozkaz NOP)
			dlatego odwo³anie siê do tablicy _map[numer_rozkazu] wywo³a po¿¹dan¹ funkcjê (na razie w teorii)..)
- wykonaj	(wywo³anie odpowiedniej funkcji w dekodowaniu spowoduje jej wykonanie. Je¿eli funkcja posiada dodatkowe parametry 
			(rozkaz ma rozmiar od 1 do 3 bajtów w³¹cznie z danymi, dlatego same dane mog¹ nie wyst¹piæ lub mieæ od 1 do 2 bajtów)
			to sama sobie je pobiera i analizuje. Tutaj trzeba zaimplementowaæ wszystkie funkcje, na tym na pocz¹tku siê skupiam.)
*/

Z80::Z80()
{
	
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
	_r.f = 0;
	_r.h = 0;
	_r.l = 0;
	_r.pc = 0;
	_r.sp = 0;
	_r.m = 0;
	_r.t = 0;
	_r.r = 0;
	_r.ime = 1;

	_stop = 0;
	_halt = 0;

	_clock.m = 0;
	_clock.t = 0;
}

void Z80::dispatch()
{
	while(!_stop)
	{
		_r.r++; //Licznik poleceñ? Po kij mi to tutaj? Mo¿e jakiœ debag czy cuœ..
		(this->*_map[_mmu->rb(_r.pc++)])();
		_clock.m += _r.m;
		_clock.t += _r.t;

		_gpu->step();
	}
}

void Z80::init()
{
	R _r = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Clock _clock = {0, 0};

	reset();
}

Z80::OpCodeMap Z80::_map[] = 
	{ 
		// 00
		&Z80::NOP,		&Z80::LDBCnn,	&Z80::LDBCnA,	&Z80::INCBC,
		&Z80::INCr_b,	&Z80::DECr_b,	&Z80::LDrn_b,	&Z80::RLCA,
		&Z80::LDnnSP,	&Z80::ADDHLBC,	&Z80::LDABCn,	&Z80::DECBC,
		&Z80::INCr_c,	&Z80::DECr_c,	&Z80::LDrn_c,	&Z80::RRCA,
		// 10
		&Z80::STOP,		&Z80::LDDEnn,	&Z80::LDDEnA,	&Z80::INCDE,
		&Z80::INCr_d,	&Z80::DECr_d,	&Z80::LDrn_d,	&Z80::RLA,
		&Z80::JRn,		&Z80::ADDHLDE,	&Z80::LDADEn,	&Z80::DECDE,
		&Z80::INCr_e,	&Z80::DECr_e,	&Z80::LDrn_e,	&Z80::RRA,
		// 20
		&Z80::JRNZn,	&Z80::LDHLnn,	&Z80::LDHLIA,	&Z80::INCHL,
		&Z80::INCr_h,	&Z80::DECr_h,	&Z80::LDrn_h,	&Z80::DAA,
		&Z80::JRZn,		&Z80::ADDHLHL,	&Z80::LDAHLI,	&Z80::DECHL,
		&Z80::INCr_l,	&Z80::DECr_l,	&Z80::LDrn_l,	&Z80::CPL,
		// 30
		&Z80::JRNCn,	&Z80::LDSPnn,	&Z80::LDHLDA,	&Z80::INCSP,
		&Z80::INCHLn,	&Z80::DECHLn,	&Z80::LDHLnn,	&Z80::SCF,
		&Z80::JRCn,		&Z80::ADDHLSP,	&Z80::LDAHLD,	&Z80::DECSP,
		&Z80::INCr_a,	&Z80::DECr_a,	&Z80::LDrn_a,	&Z80::CCF,
		// 40
		&Z80::LDrr_bb,	&Z80::LDrr_bc,	&Z80::LDrr_bd,	&Z80::LDrr_be,
		&Z80::LDrr_bh,	&Z80::LDrr_bl,	&Z80::LDrHLn_b,	&Z80::LDrr_ba,
		&Z80::LDrr_cb,	&Z80::LDrr_cc,	&Z80::LDrr_cd,	&Z80::LDrr_ce,
		&Z80::LDrr_ch,	&Z80::LDrr_cl,	&Z80::LDrHLn_c,	&Z80::LDrr_ca,
		// 50
		&Z80::LDrr_db,	&Z80::LDrr_dc,	&Z80::LDrr_dd,	&Z80::LDrr_de,
		&Z80::LDrr_dh,	&Z80::LDrr_dl,	&Z80::LDrHLn_d,	&Z80::LDrr_da,
		&Z80::LDrr_eb,	&Z80::LDrr_ec,	&Z80::LDrr_ed,	&Z80::LDrr_ee,
		&Z80::LDrr_eh,	&Z80::LDrr_el,	&Z80::LDrHLn_e,	&Z80::LDrr_ea,
		// 60
		&Z80::LDrr_hb,	&Z80::LDrr_hc,	&Z80::LDrr_hd,	&Z80::LDrr_he,
		&Z80::LDrr_hh,	&Z80::LDrr_hl,	&Z80::LDrHLn_h,	&Z80::LDrr_ha,
		&Z80::LDrr_lb,	&Z80::LDrr_lc,	&Z80::LDrr_ld,	&Z80::LDrr_le,
		&Z80::LDrr_lh,	&Z80::LDrr_ll,	&Z80::LDrHLn_l,	&Z80::LDrr_la,
		// 70
		&Z80::LDHLnr_b,	&Z80::LDHLnr_c,	&Z80::LDHLnr_d,	&Z80::LDHLnr_e,
		&Z80::LDHLnr_h,	&Z80::LDHLnr_l,	&Z80::HALT,		&Z80::LDHLnr_a,
		&Z80::LDrr_ab,	&Z80::LDrr_ac,	&Z80::LDrr_ad,	&Z80::LDrr_ae,
		&Z80::LDrr_ah,	&Z80::LDrr_al,	&Z80::LDrHLn_a,	&Z80::LDrr_aa,
		// 80
		&Z80::ADDr_b,	&Z80::ADDr_c,	&Z80::ADDr_d,	&Z80::ADDr_e,
		&Z80::ADDr_h,	&Z80::ADDr_l,	&Z80::ADDHL,	&Z80::ADDr_a,
		&Z80::ADCr_b,	&Z80::ADCr_c,	&Z80::ADCr_d,	&Z80::ADCr_e,
		&Z80::ADCr_h,	&Z80::ADCr_l,	&Z80::ADCHL,	&Z80::ADCr_a,
		// 90
		&Z80::SUBr_b,	&Z80::SUBr_c,	&Z80::SUBr_d,	&Z80::SUBr_e,
		&Z80::SUBr_h,	&Z80::SUBr_l,	&Z80::SUBHL,	&Z80::SUBr_a,
		&Z80::SBCr_b,	&Z80::SBCr_c,	&Z80::SBCr_d,	&Z80::SBCr_e,
		&Z80::SBCr_h,	&Z80::SBCr_l,	&Z80::SBCHL,	&Z80::SBCr_a,
		// A0
		&Z80::ANDr_b,	&Z80::ANDr_c,	&Z80::ANDr_d,	&Z80::ANDr_e,
		&Z80::ANDr_h,	&Z80::ANDr_l,	&Z80::ANDHL,	&Z80::ANDr_a,
		&Z80::XORr_b,	&Z80::XORr_c,	&Z80::XORr_d,	&Z80::XORr_e,
		&Z80::XORr_h,	&Z80::XORr_l,	&Z80::XORHL,	&Z80::XORr_a,
		// B0
		&Z80::ORr_b,	&Z80::ORr_c,	&Z80::ORr_d,	&Z80::ORr_e,
		&Z80::ORr_h,	&Z80::ORr_l,	&Z80::ORHL,		&Z80::ORr_a,
		&Z80::CPr_b,	&Z80::CPr_c,	&Z80::CPr_d,	&Z80::CPr_e,
		&Z80::CPr_h,	&Z80::CPr_l,	&Z80::CPHL,		&Z80::CPr_a,
		// C0
		&Z80::RETNZ,	&Z80::POPBC,	&Z80::JPNZnn,	&Z80::JPnn,
		&Z80::CALLNZnn,	&Z80::PUSHBC,	&Z80::ADDn,		&Z80::RST00,
		&Z80::RETZ,		&Z80::RET,		&Z80::JPZnn,	&Z80::MAPcb,
		&Z80::CALLZnn,	&Z80::CALLnn,	&Z80::ADCn,		&Z80::RST08,
		// D0
		&Z80::RETNC,	&Z80::POPDE,	&Z80::JPNCnn,	&Z80::XX,
		&Z80::CALLNCnn,	&Z80::PUSHDE,	&Z80::SUBn,		&Z80::RST10,
		&Z80::RETC,		&Z80::RETI,		&Z80::JPCnn,	&Z80::XX,
		&Z80::CALLCnn,	&Z80::XX,		&Z80::SBCn,		&Z80::RST18,
		// E0
		&Z80::LDIOnA,	&Z80::POPHL,	&Z80::LDIOCA,	&Z80::XX,
		&Z80::XX,		&Z80::PUSHHL,	&Z80::ANDn,		&Z80::RST20,
		&Z80::ADDSPn,	&Z80::JPHL,		&Z80::LDnnA,	&Z80::XX,
		&Z80::XX,		&Z80::XX,		&Z80::XORn,		&Z80::RST28,
		// F0
		&Z80::LDAIOn,	&Z80::POPAF,	&Z80::LDAIOC,	&Z80::DI,
		&Z80::XX,		&Z80::PUSHAF,	&Z80::ORn,		&Z80::RST30,
		&Z80::LDHLSPn,	&Z80::XX,		&Z80::LDAnn,	&Z80::EI,
		&Z80::XX,		&Z80::XX,		&Z80::CPn,		&Z80::RST38
	};

Z80::OpCodeMap Z80::_CBmap[] = 
	{
		// CB00
		&Z80::RLCr_b,	&Z80::RLCr_c,	&Z80::RLCr_d,	&Z80::RLCr_e,
		&Z80::RLCr_h,	&Z80::RLCr_l,	&Z80::RLCHL,	&Z80::RLCr_a,
		&Z80::RRCr_b,	&Z80::RRCr_c,	&Z80::RRCr_d,	&Z80::RRCr_e,
		&Z80::RRCr_h,	&Z80::RRCr_l,	&Z80::RRCHL,	&Z80::RRCr_a,
		// CB10
		&Z80::RLr_b,	&Z80::RLr_c,	&Z80::RLr_d,	&Z80::RLr_e,
		&Z80::RLr_h,	&Z80::RLr_l,	&Z80::RLHL,		&Z80::RLr_a,
		&Z80::RRr_b,	&Z80::RRr_c,	&Z80::RRr_d,	&Z80::RRr_e,
		&Z80::RRr_h,	&Z80::RRr_l,	&Z80::RRHL,		&Z80::RRr_a,
		// CB20
		&Z80::SLAr_b,	&Z80::SLAr_c,	&Z80::SLAr_d,	&Z80::SLAr_e,
		&Z80::SLAr_h,	&Z80::SLAr_l,	&Z80::SLAHL,	&Z80::SLAr_a,
		&Z80::SRAr_b,	&Z80::SRAr_c,	&Z80::SRAr_d,	&Z80::SRAr_e,
		&Z80::SRAr_h,	&Z80::SRAr_l,	&Z80::SRAHL,	&Z80::SRAr_a,
		// CB30
		&Z80::SWAPr_b,	&Z80::SWAPr_c,	&Z80::SWAPr_d,	&Z80::SWAPr_e,
		&Z80::SWAPr_h,	&Z80::SWAPr_l,	&Z80::SWAPHL,	&Z80::SWAPr_a,
		&Z80::SRLr_b,	&Z80::SRLr_c,	&Z80::SRLr_d,	&Z80::SRLr_e,
		&Z80::SRLr_h,	&Z80::SRLr_l,	&Z80::SRLHL,		&Z80::SRLr_a,
		// CB40
		&Z80::BIT0b,	&Z80::BIT0c,	&Z80::BIT0d,	&Z80::BIT0e,
		&Z80::BIT0h,	&Z80::BIT0l,	&Z80::BIT0n,	&Z80::BIT0a,
		&Z80::BIT1b,	&Z80::BIT1c,	&Z80::BIT1d,	&Z80::BIT1e,
		&Z80::BIT1h,	&Z80::BIT1l,	&Z80::BIT1n,	&Z80::BIT1a,
		// CB50
		&Z80::BIT2b,	&Z80::BIT2c,	&Z80::BIT2d,	&Z80::BIT2e,
		&Z80::BIT2h,	&Z80::BIT2l,	&Z80::BIT2n,	&Z80::BIT2a,
		&Z80::BIT3b,	&Z80::BIT3c,	&Z80::BIT3d,	&Z80::BIT3e,
		&Z80::BIT3h,	&Z80::BIT3l,	&Z80::BIT3n,	&Z80::BIT3a,
		// CB60
		&Z80::BIT4b,	&Z80::BIT4c,	&Z80::BIT4d,	&Z80::BIT4e,
		&Z80::BIT4h,	&Z80::BIT4l,	&Z80::BIT4n,	&Z80::BIT4a,
		&Z80::BIT5b,	&Z80::BIT5c,	&Z80::BIT5d,	&Z80::BIT5e,
		&Z80::BIT5h,	&Z80::BIT5l,	&Z80::BIT5n,	&Z80::BIT5a,
		// CB70
		&Z80::BIT6b,	&Z80::BIT6c,	&Z80::BIT6d,	&Z80::BIT6e,
		&Z80::BIT6h,	&Z80::BIT6l,	&Z80::BIT6n,	&Z80::BIT6a,
		&Z80::BIT7b,	&Z80::BIT7c,	&Z80::BIT7d,	&Z80::BIT7e,
		&Z80::BIT7h,	&Z80::BIT7l,	&Z80::BIT7n,	&Z80::BIT7a,
		// CB80
		&Z80::RES0b,	&Z80::RES0c,	&Z80::RES0d,	&Z80::RES0e,
		&Z80::RES0h,	&Z80::RES0l,	&Z80::RES0n,	&Z80::RES0a,
		&Z80::RES1b,	&Z80::RES1c,	&Z80::RES1d,	&Z80::RES1e,
		&Z80::RES1h,	&Z80::RES1l,	&Z80::RES1n,	&Z80::RES1a,
		// CB90
		&Z80::RES2b,	&Z80::RES2c,	&Z80::RES2d,	&Z80::RES2e,
		&Z80::RES2h,	&Z80::RES2l,	&Z80::RES2n,	&Z80::RES2a,
		&Z80::RES3b,	&Z80::RES3c,	&Z80::RES3d,	&Z80::RES3e,
		&Z80::RES3h,	&Z80::RES3l,	&Z80::RES3n,	&Z80::RES3a,
		// CBA0
		&Z80::RES4b,	&Z80::RES4c,	&Z80::RES4d,	&Z80::RES4e,
		&Z80::RES4h,	&Z80::RES4l,	&Z80::RES4n,	&Z80::RES4a,
		&Z80::RES5b,	&Z80::RES5c,	&Z80::RES5d,	&Z80::RES5e,
		&Z80::RES5h,	&Z80::RES5l,	&Z80::RES5n,	&Z80::RES5a,
		// CBB0
		&Z80::RES6b,	&Z80::RES6c,	&Z80::RES6d,	&Z80::RES6e,
		&Z80::RES6h,	&Z80::RES6l,	&Z80::RES6n,	&Z80::RES6a,
		&Z80::RES7b,	&Z80::RES7c,	&Z80::RES7d,	&Z80::RES7e,
		&Z80::RES7h,	&Z80::RES7l,	&Z80::RES7n,	&Z80::RES7a,
		// CBC0
		&Z80::SET0b,	&Z80::SET0c,	&Z80::SET0d,	&Z80::SET0e,
		&Z80::SET0h,	&Z80::SET0l,	&Z80::SET0n,	&Z80::SET0a,
		&Z80::SET1b,	&Z80::SET1c,	&Z80::SET1d,	&Z80::SET1e,
		&Z80::SET1h,	&Z80::SET1l,	&Z80::SET1n,	&Z80::SET1a,
		// CBD0
		&Z80::SET2b,	&Z80::SET2c,	&Z80::SET2d,	&Z80::SET2e,
		&Z80::SET2h,	&Z80::SET2l,	&Z80::SET2n,	&Z80::SET2a,
		&Z80::SET3b,	&Z80::SET3c,	&Z80::SET3d,	&Z80::SET3e,
		&Z80::SET3h,	&Z80::SET3l,	&Z80::SET3n,	&Z80::SET3a,
		// CBE0
		&Z80::SET4b,	&Z80::SET4c,	&Z80::SET4d,	&Z80::SET4e,
		&Z80::SET4h,	&Z80::SET4l,	&Z80::SET4n,	&Z80::SET4a,
		&Z80::SET5b,	&Z80::SET5c,	&Z80::SET5d,	&Z80::SET5e,
		&Z80::SET5h,	&Z80::SET5l,	&Z80::SET5n,	&Z80::SET5a,
		// CBF0
		&Z80::SET6b,	&Z80::SET6c,	&Z80::SET6d,	&Z80::SET6e,
		&Z80::SET6h,	&Z80::SET6l,	&Z80::SET6n,	&Z80::SET6a,
		&Z80::SET7b,	&Z80::SET7c,	&Z80::SET7d,	&Z80::SET7e,
		&Z80::SET7h,	&Z80::SET7l,	&Z80::SET7n,	&Z80::SET7a,
	};


//////////////////////////////////////////////////
//												//
//				Lista instrukcji				//
// http://gameboy.mongenel.com/dmg/opcodes.html	//
//////////////////////////////////////////////////

//r - rejestr, n - adres pamieci
//r - 8b, rr - 16b, tak samo n.

/* Operacje wczytywania i zapisu */
//Z rejestru do rejestru
void Z80::LDrr_bb() { _r.b = _r.b; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_bc() { _r.b = _r.c; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_bd() { _r.b = _r.d; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_be() { _r.b = _r.e; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_bh() { _r.b = _r.h; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_bl() { _r.b = _r.l; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ba() { _r.b = _r.a; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_cb() { _r.c = _r.b; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_cc() { _r.c = _r.c; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_cd() { _r.c = _r.d; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ce() { _r.c = _r.e; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ch() { _r.c = _r.h; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_cl() { _r.c = _r.l; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ca() { _r.c = _r.a; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_db() { _r.d = _r.b; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_dc() { _r.d = _r.c; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_dd() { _r.d = _r.d; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_de() { _r.d = _r.e; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_dh() { _r.d = _r.h; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_dl() { _r.d = _r.l; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_da() { _r.d = _r.a; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_eb() { _r.e = _r.b; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ec() { _r.e = _r.c; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ed() { _r.e = _r.d; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ee() { _r.e = _r.e; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_eh() { _r.e = _r.h; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_el() { _r.e = _r.l; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ea() { _r.e = _r.a; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_hb() { _r.h = _r.b; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_hc() { _r.h = _r.c; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_hd() { _r.h = _r.d; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_he() { _r.h = _r.e; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_hh() { _r.h = _r.h; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_hl() { _r.h = _r.l; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ha() { _r.h = _r.a; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_lb() { _r.l = _r.b; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_lc() { _r.l = _r.c; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ld() { _r.l = _r.d; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_le() { _r.l = _r.e; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_lh() { _r.l = _r.h; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ll() { _r.l = _r.l; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_la() { _r.l = _r.a; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ab() { _r.a = _r.b; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ac() { _r.a = _r.c; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ad() { _r.a = _r.d; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ae() { _r.a = _r.e; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_ah() { _r.a = _r.h; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_al() { _r.a = _r.l; _r.m = 1;  _r.t = 4; }
void Z80::LDrr_aa() { _r.a = _r.a; _r.m = 1;  _r.t = 4; }

//Kopiuj dane spod adresu w HL do rejestru
void Z80::LDrHLn_b() { _r.b=_mmu->rb((_r.h<<8)+_r.l); _r.m=1; _r.t=8; }
void Z80::LDrHLn_c() { _r.c=_mmu->rb((_r.h<<8)+_r.l); _r.m=1; _r.t=8; }
void Z80::LDrHLn_d() { _r.d=_mmu->rb((_r.h<<8)+_r.l); _r.m=1; _r.t=8; }
void Z80::LDrHLn_e() { _r.e=_mmu->rb((_r.h<<8)+_r.l); _r.m=1; _r.t=8; }
void Z80::LDrHLn_h() { _r.h=_mmu->rb((_r.h<<8)+_r.l); _r.m=1; _r.t=8; }
void Z80::LDrHLn_l() { _r.l=_mmu->rb((_r.h<<8)+_r.l); _r.m=1; _r.t=8; }
void Z80::LDrHLn_a() { _r.a=_mmu->rb((_r.h<<8)+_r.l); _r.m=1; _r.t=8; }

//Kopiuj z rejestru do pamieci pod adresem w HL
void Z80::LDHLnr_b() { _mmu->wb((_r.h<<8)+_r.l, _r.b); _r.m=1; _r.t=8; }
void Z80::LDHLnr_c() { _mmu->wb((_r.h<<8)+_r.l, _r.c); _r.m=1; _r.t=8; }
void Z80::LDHLnr_d() { _mmu->wb((_r.h<<8)+_r.l, _r.d); _r.m=1; _r.t=8; }
void Z80::LDHLnr_e() { _mmu->wb((_r.h<<8)+_r.l, _r.e); _r.m=1; _r.t=8; }
void Z80::LDHLnr_h() { _mmu->wb((_r.h<<8)+_r.l, _r.h); _r.m=1; _r.t=8; }
void Z80::LDHLnr_l() { _mmu->wb((_r.h<<8)+_r.l, _r.l); _r.m=1; _r.t=8; }
void Z80::LDHLnr_a() { _mmu->wb((_r.h<<8)+_r.l, _r.a); _r.m=1; _r.t=8; }

//Pociagnij dane bezpoœrednio spod PC (program counter) do rejestru
void Z80::LDrn_b() { _r.b=_mmu->rb(_r.pc); _r.pc++; _r.m=2; _r.t=8; }
void Z80::LDrn_c() { _r.c=_mmu->rb(_r.pc); _r.pc++; _r.m=2; _r.t=8; }
void Z80::LDrn_d() { _r.d=_mmu->rb(_r.pc); _r.pc++; _r.m=2; _r.t=8; }
void Z80::LDrn_e() { _r.e=_mmu->rb(_r.pc); _r.pc++; _r.m=2; _r.t=8; }
void Z80::LDrn_h() { _r.h=_mmu->rb(_r.pc); _r.pc++; _r.m=2; _r.t=8; }
void Z80::LDrn_l() { _r.l=_mmu->rb(_r.pc); _r.pc++; _r.m=2; _r.t=8; }
void Z80::LDrn_a() { _r.a=_mmu->rb(_r.pc); _r.pc++; _r.m=2; _r.t=8; }

//Skopiuj bajt spod PC do pamiêci wskazywanej przez HL.
void Z80::LDHLn() { _mmu->wb((_r.h<<8)+_r.l, _mmu->rb(_r.pc)); _r.pc++; _r.m=2; _r.t=12; }

//Zapisz rejestr A pod adres wskazywany przez 16b rejestr
void Z80::LDBCnA() { _mmu->wb((_r.b<<8)+_r.c, _r.a); _r.m=1; _r.t=8; }
void Z80::LDDEnA() { _mmu->wb((_r.d<<8)+_r.e, _r.a); _r.m=1; _r.t=8; }

//Zapisz A pod adres wskazywany przez 16b adresu PC
void Z80::LDnnA() { _mmu->wb(_mmu->rw(_r.pc), _r.a); _r.pc+=2; _r.m=3; _r.t=16; }

//Zaladuj do A z adresu wskazywanego przez 16b rejestr
void Z80::LDABCn() { _r.a=_mmu->rb((_r.b<<8)+_r.c); _r.m=1; _r.t=8; }
void Z80::LDADEn() { _r.a=_mmu->rb((_r.d<<8)+_r.e); _r.m=1; _r.t=8; }

//Zaladuj do A z pamieci wskazywanej przez 16b adresu PC
void Z80::LDAnn() { _r.a=_mmu->rb(_mmu->rw(_r.pc)); _r.pc+=2; _r.m=3; _r.t=16; }

//Zaladuj 16 bitów spod PC do rejestru
void Z80::LDBCnn() { _r.c=_mmu->rb(_r.pc); _r.b=_mmu->rb(_r.pc+1); _r.pc+=2; _r.m=3; _r.t=12; }
void Z80::LDDEnn() { _r.e=_mmu->rb(_r.pc); _r.d=_mmu->rb(_r.pc+1); _r.pc+=2; _r.m=3; _r.t=12; }
void Z80::LDHLnn() { _r.l=_mmu->rb(_r.pc); _r.h=_mmu->rb(_r.pc+1); _r.pc+=2; _r.m=3; _r.t=12; }
void Z80::LDSPnn() { _r.sp=_mmu->rw(_r.pc); _r.pc+=2; _r.m=3; _r.t=12; }

//Zaladuj z/do rejestru A z/do adresu wskazywanego przez HL i inkrementuj HL
void Z80::LDHLIA() { _mmu->wb((_r.h<<8)+_r.l, _r.a); _r.l=(_r.l+1)&255; if(!_r.l) _r.h=(_r.h+1)&255; _r.m=1; _r.t=8; }
void Z80::LDAHLI() { _r.a=_mmu->rb((_r.h<<8)+_r.l); _r.l=(_r.l+1)&255; if(!_r.l) _r.h=(_r.h+1)&255; _r.m=1; _r.t=8; }

//jw. z dekrementacja
void Z80::LDHLDA() { _mmu->wb((_r.h<<8)+_r.l, _r.a); _r.l=(_r.l-1)&255; if(_r.l==255) _r.h=(_r.h-1)&255; _r.m=1; _r.t = 8; }
void Z80::LDAHLD() { _r.a=_mmu->rb((_r.h<<8)+_r.l); _r.l=(_r.l-1)&255; if(_r.l==255) _r.h=(_r.h-1)&255; _r.m=1; _r.t = 8; }

//Za³aduj A z adresu wskazywanego przez (FF00 + bajt na który wskazuje PC)
void Z80::LDAIOn() { _r.a=_mmu->rb(0xFF00+_mmu->rb(_r.pc)); _r.pc++; _r.m=2; _r.t=12; }
//zapisz A do adresu wskazywanego przez (FF00 + bajt na który wskazuje PC)
void Z80::LDIOnA() { _mmu->wb(0xFF00+_mmu->rb(_r.pc),_r.a); _r.pc++; _r.m=2; _r.t=12; }
//jw, zamiast bajtu na który wskazuje PC u¿ywamy po prostu rejestru C
void Z80::LDAIOC() { _r.a=_mmu->rb(0xFF00+_r.c); _r.m=2; _r.t=8; }
void Z80::LDIOCA() { _mmu->wb(0xFF00+_r.c,_r.a); _r.m=2; _r.t=8; }

//Dodaj bajt spod adresu w PC do wartoœci SP i wpisz wynik do HL 
void Z80::LDHLSPn() { unsigned char i=_mmu->rb(_r.pc); if(i>127) i=-((~i+1)&255); _r.pc++; i+=_r.sp; _r.h=(i>>8)&255; _r.l=i&255; _r.m=2; _r.t=12; }
void Z80::LDnnSP() { _mmu->wb(_mmu->rw(_r.pc), _r.sp); _r.pc+=2; _r.m=3; _r.t=20; }

//Zamiana miejscami czesci starszej i mlodszej
void Z80::SWAPr_b() { _r.b=((_r.b&0xF)<<4)|((_r.b&0xF0)>>4); _r.f=_r.b?0:0x80; _r.m=2; _r.t=8; }
void Z80::SWAPr_c() { _r.c=((_r.c&0xF)<<4)|((_r.c&0xF0)>>4); _r.f=_r.c?0:0x80; _r.m=2; _r.t=8; }
void Z80::SWAPr_d() { _r.d=((_r.d&0xF)<<4)|((_r.d&0xF0)>>4); _r.f=_r.d?0:0x80; _r.m=2; _r.t=8; }
void Z80::SWAPr_e() { _r.e=((_r.e&0xF)<<4)|((_r.e&0xF0)>>4); _r.f=_r.e?0:0x80; _r.m=2; _r.t=8; }
void Z80::SWAPr_h() { _r.h=((_r.h&0xF)<<4)|((_r.h&0xF0)>>4); _r.f=_r.h?0:0x80; _r.m=2; _r.t=8; }
void Z80::SWAPr_l() { _r.l=((_r.l&0xF)<<4)|((_r.l&0xF0)>>4); _r.f=_r.l?0:0x80; _r.m=2; _r.t=8; }
void Z80::SWAPr_a() { _r.a=((_r.a&0xF)<<4)|((_r.a&0xF0)>>4); _r.f=_r.a?0:0x80; _r.m=2; _r.t=8; }
void Z80::SWAPHL() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var=((var&0xF)<<4)|((var&0xF0)>>4); _r.f=var?0:0x80; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

/* Operacje przetwarzania danych */
//Wszystkie operacje logiczne i arytmetyczne sa wykonywane na rejestrze A i podanym drugim rejestrze.
//Flagi: zero flag - 0x80, flaga po¿yczki - 0x40, flaga nadmiaru w po³owie rejestru 0x20, flaga nadmiaru - 0x10
void Z80::ADDr_b() { unsigned short tmp=_r.a; tmp+=_r.b; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.b^_r.a)&0x10) _r.f|=0x20; _r.a+=_r.b; _r.m=1; _r.t=4; }
void Z80::ADDr_c() { unsigned short tmp=_r.a; tmp+=_r.c; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.c^_r.a)&0x10) _r.f|=0x20; _r.a+=_r.c; _r.m=1; _r.t=4; }
void Z80::ADDr_d() { unsigned short tmp=_r.a; tmp+=_r.d; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.d^_r.a)&0x10) _r.f|=0x20; _r.a+=_r.d; _r.m=1; _r.t=4; }
void Z80::ADDr_e() { unsigned short tmp=_r.a; tmp+=_r.e; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.e^_r.a)&0x10) _r.f|=0x20; _r.a+=_r.e; _r.m=1; _r.t=4; }
void Z80::ADDr_h() { unsigned short tmp=_r.a; tmp+=_r.h; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.h^_r.a)&0x10) _r.f|=0x20; _r.a+=_r.h; _r.m=1; _r.t=4; }
void Z80::ADDr_l() { unsigned short tmp=_r.a; tmp+=_r.l; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.l^_r.a)&0x10) _r.f|=0x20; _r.a+=_r.l; _r.m=1; _r.t=4; }
void Z80::ADDr_a() { unsigned short tmp=_r.a; tmp+=_r.a; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.a^_r.a)&0x10) _r.f|=0x20; _r.a+=_r.a; _r.m=1; _r.t=4; }
void Z80::ADDHL() { unsigned short tmp=_r.a; unsigned char var=_mmu->rb((_r.h<<8)+_r.l); tmp+=var; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.a^var)&0x10) _r.f|=0x20; _r.a+=var; _r.m=1; _r.t = 8; }
void Z80::ADDn() { unsigned short tmp=_r.a; unsigned char var=_mmu->rb(_r.pc); _r.pc++; tmp+=var; _r.f=(tmp>255)?0x10:0; tmp&=255; if(!tmp) _r.f|=0x80; if((tmp^_r.a^var)&0x10) _r.f|=0x20; _r.a+=var; _r.m=1; _r.t = 8; }
void Z80::ADDHLBC() { unsigned int hl=(_r.h<<8)+_r.l; unsigned short var =(_r.b<<8)+_r.c; hl+=var; if(hl>65535) _r.f|=0x10; else _r.f&=0xEF; _r.f^=0x40; hl&=65535; if((hl^((_r.h<<8)+_r.l)^var)&0x0100) _r.f|=0x20; _r.h=(hl>>8)&255; _r.l=hl&255; _r.m=1; _r.t=8; }
void Z80::ADDHLDE() { unsigned int hl=(_r.h<<8)+_r.l; unsigned short var =(_r.d<<8)+_r.e; hl+=var; if(hl>65535) _r.f|=0x10; else _r.f&=0xEF; _r.f^=0x40; hl&=65535; if((hl^((_r.h<<8)+_r.l)^var)&0x0100) _r.f|=0x20; _r.h=(hl>>8)&255; _r.l=hl&255; _r.m=1; _r.t=8; }
void Z80::ADDHLHL() { unsigned int hl=(_r.h<<8)+_r.l; unsigned short var =(_r.h<<8)+_r.l; hl+=var; if(hl>65535) _r.f|=0x10; else _r.f&=0xEF; _r.f^=0x40; hl&=65535; if((hl^((_r.h<<8)+_r.l)^var)&0x0100) _r.f|=0x20; _r.h=(hl>>8)&255; _r.l=hl&255; _r.m=1; _r.t=8; }
void Z80::ADDHLSP() { unsigned int hl=(_r.h<<8)+_r.l; unsigned short var =_r.sp; hl+=var; if(hl>65535) _r.f|=0x10; else _r.f&=0xEF; _r.f^=0x40; hl&=65535; if((hl^((_r.h<<8)+_r.l)^var)&0x0100) _r.f|=0x20; _r.h=(hl>>8)&255; _r.l=hl&255; _r.m=1; _r.t=8; }
void Z80::ADDSPn() {  unsigned int sp = _r.sp; unsigned char var=_mmu->rb(_r.pc); _r.pc++; if(var>127) var=-((~var+1)&255); sp+=var; _r.f=(sp>65535)?0x10:0;  sp&=65535; if((sp^_r.sp^var)&0x0100) _r.f|=0x20; _r.sp=sp; _r.m=2; _r.t=16; }

void Z80::ADCr_b() { unsigned short a=_r.a; a+=_r.b; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.b^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::ADCr_c() { unsigned short a=_r.a; a+=_r.c; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.c^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::ADCr_d() { unsigned short a=_r.a; a+=_r.d; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.d^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::ADCr_e() { unsigned short a=_r.a; a+=_r.e; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.e^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::ADCr_h() { unsigned short a=_r.a; a+=_r.h; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.h^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::ADCr_l() { unsigned short a=_r.a; a+=_r.l; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.l^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::ADCr_a() { unsigned short a=_r.a; a+=_r.a; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.a^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::ADCHL() { unsigned short a=_r.a; unsigned char var=_mmu->rb((_r.h<<8)+_r.l); a+=var; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^var^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=8; }
void Z80::ADCn() { unsigned short a=_r.a; unsigned char var=_mmu->rb(_r.pc); _r.pc++; a+=var; a+=(_r.f&0x10)?1:0; _r.f=(a>255)?0x10:0; a&=255; if(!a) _r.f|=0x80; if((_r.a^var^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=2; _r.t=8; }

void Z80::SUBr_b() { short a=_r.a; a-=_r.b; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^_r.b^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SUBr_c() { short a=_r.a; a-=_r.c; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^_r.c^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SUBr_d() { short a=_r.a; a-=_r.d; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^_r.d^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SUBr_e() { short a=_r.a; a-=_r.e; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^_r.e^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SUBr_h() { short a=_r.a; a-=_r.h; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^_r.h^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SUBr_l() { short a=_r.a; a-=_r.l; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^_r.l^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SUBr_a() { short a=_r.a; a-=_r.a; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^_r.a^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SUBHL() { short a=_r.a; unsigned char var=_mmu->rb((_r.h<<8)+_r.l); a-=var; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^var^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=8; }
void Z80::SUBn() { short a=_r.a; unsigned char var=_mmu->rb(_r.pc); _r.pc++; a-=var; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^var^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=2; _r.t=8; }

void Z80::SBCr_b() { short a=_r.a; a-=_r.b; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.b^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SBCr_c() { short a=_r.a; a-=_r.c; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.c^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SBCr_d() { short a=_r.a; a-=_r.d; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.d^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SBCr_e() { short a=_r.a; a-=_r.e; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.e^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SBCr_h() { short a=_r.a; a-=_r.h; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.h^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SBCr_l() { short a=_r.a; a-=_r.l; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.l^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SBCr_a() { short a=_r.a; a-=_r.a; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.a^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=4; }
void Z80::SBCHL() { short a=_r.a; unsigned char var=_mmu->rb((_r.h<<8)+_r.l); a-=var; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^var^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=1; _r.t=8; }
void Z80::SBCn() { short a=_r.a; unsigned char var=_mmu->rb(_r.pc); _r.pc++; a-=var; a-=(_r.f&0x10)?1:0; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^var^a)&0x10) _r.f|=0x20; _r.a=a; _r.m=2; _r.t=8; }

void Z80::CPr_b() { short a=_r.a; a-=_r.b; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.b^a)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::CPr_c() { short a=_r.a; a-=_r.c; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.c^a)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::CPr_d() { short a=_r.a; a-=_r.d; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.d^a)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::CPr_e() { short a=_r.a; a-=_r.e; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.e^a)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::CPr_h() { short a=_r.a; a-=_r.h; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.h^a)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::CPr_l() { short a=_r.a; a-=_r.l; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.l^a)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::CPr_a() { short a=_r.a; a-=_r.a; _r.f=(a<0)?0x50:0x40; a&=255; if(!a) _r.f|=0x80; if((_r.a^_r.a^a)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::CPHL() { short a=_r.a; unsigned char var=_mmu->rb((_r.h<<8)+_r.l); a-=var; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^var^a)&0x10) _r.f|=0x20; _r.m=1; _r.t=8; }
void Z80::CPn() { short a=_r.a; unsigned char var=_mmu->rb(_r.pc); _r.pc++; a-=var; _r.f=(a<0)?0x50:0x40; a&=255; if(a) _r.f|=0x80; if((_r.a^var^a)&0x10) _r.f|=0x20; _r.m=2; _r.t=8; }

void Z80::DAA() { unsigned char a=_r.a; if((_r.f&0x20)||((_r.a&15)>9)) _r.a+=6; _r.f&=0xEF; if((_r.f&0x20)||(a>0x99)) { _r.a+=0x60; _r.f|=0x10; } _r.m=1; _r.t=4; }

void Z80::ANDr_b() { _r.a&=_r.b; _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=4; }
void Z80::ANDr_c() { _r.a&=_r.c; _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=4; }
void Z80::ANDr_d() { _r.a&=_r.d; _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=4; }
void Z80::ANDr_e() { _r.a&=_r.e; _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=4; }
void Z80::ANDr_h() { _r.a&=_r.h; _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=4; }
void Z80::ANDr_l() { _r.a&=_r.l; _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=4; }
void Z80::ANDr_a() { _r.a&=_r.a; _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=4; }
void Z80::ANDHL() { _r.a&=_mmu->rb((_r.h<<8)+_r.l); _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=8; }
void Z80::ANDn() { _r.a&=_mmu->rb(_r.pc); _r.pc++; _r.f=_r.a?0x20:0xA0; _r.m=2; _r.t=8; }

void Z80::ORr_b() { _r.a|=_r.b; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::ORr_c() { _r.a|=_r.c; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::ORr_d() { _r.a|=_r.d; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::ORr_e() { _r.a|=_r.e; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::ORr_h() { _r.a|=_r.h; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::ORr_l() { _r.a|=_r.l; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::ORr_a() { _r.a|=_r.a; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::ORHL() { _r.a|=_mmu->rb((_r.h<<8)+_r.l); _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=8; }
void Z80::ORn() {  _r.a|=_mmu->rb(_r.pc); _r.pc++; _r.f=_r.a?0x20:0xA0; _r.m=2; _r.t=8; }

void Z80::XORr_b() { _r.a^=_r.b; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::XORr_c() { _r.a^=_r.c; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::XORr_d() { _r.a^=_r.d; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::XORr_e() { _r.a^=_r.e; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::XORr_h() { _r.a^=_r.h; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::XORr_l() { _r.a^=_r.l; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::XORr_a() { _r.a^=_r.a; _r.f=_r.a?0x00:0x80; _r.m=1; _r.t=4; }
void Z80::XORHL() { _r.a^=_mmu->rb((_r.h<<8)+_r.l); _r.f=_r.a?0x20:0xA0; _r.m=1; _r.t=8; }
void Z80::XORn() {  _r.a^=_mmu->rb(_r.pc); _r.pc++; _r.f=_r.a?0x20:0xA0; _r.m=2; _r.t=8; }

void Z80::INCr_b() { unsigned char var=_r.b+1; _r.f&=0x1F; _r.f|var?0:0x80; if((_r.b^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::INCr_c() { unsigned char var=_r.c+1; _r.f&=0x1F; _r.f|var?0:0x80; if((_r.c^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::INCr_d() { unsigned char var=_r.d+1; _r.f&=0x1F; _r.f|var?0:0x80; if((_r.d^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::INCr_e() { unsigned char var=_r.e+1; _r.f&=0x1F; _r.f|var?0:0x80; if((_r.e^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::INCr_h() { unsigned char var=_r.h+1; _r.f&=0x1F; _r.f|var?0:0x80; if((_r.h^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::INCr_l() { unsigned char var=_r.l+1; _r.f&=0x1F; _r.f|var?0:0x80; if((_r.l^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::INCr_a() { unsigned char var=_r.a+1; _r.f&=0x1F; _r.f|var?0:0x80; if((_r.a^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::INCHLn() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); _mmu->wb((_r.h<<8)+_r.l, var+1); _r.f&=0x1F; _r.f|(var+1)?0:0x80; if((var^0x01^(var+1))&0x10) _r.f|=0x20; _r.m=1; _r.t=12; }

void Z80::DECr_b() { unsigned char var=_r.b-1; _r.f|var?0x40:0xC0; if((_r.b^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::DECr_c() { unsigned char var=_r.c-1; _r.f|var?0x40:0xC0; if((_r.c^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::DECr_d() { unsigned char var=_r.d-1; _r.f|var?0x40:0xC0; if((_r.d^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::DECr_e() { unsigned char var=_r.e-1; _r.f|var?0x40:0xC0; if((_r.e^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::DECr_h() { unsigned char var=_r.h-1; _r.f|var?0x40:0xC0; if((_r.h^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::DECr_l() { unsigned char var=_r.l-1; _r.f|var?0x40:0xC0; if((_r.l^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::DECr_a() { unsigned char var=_r.a-1; _r.f=var?0x40:0xC0; if((_r.a^0x01^var)&0x10) _r.f|=0x20; _r.m=1; _r.t=4; }
void Z80::DECHLn() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); _mmu->wb((_r.h<<8)+_r.l, var-1); _r.f|(var-1)?0:0x80; if((var^0x01^(var-1))&0x10) _r.f|=0x20; _r.m=1; _r.t=12; }

void Z80::INCBC() { _r.c++; if(!_r.c) _r.b++; _r.m=1; _r.t=8; }
void Z80::INCDE() { _r.e++; if(!_r.e) _r.d++; _r.m=1; _r.t=8; }
void Z80::INCHL() { _r.l++; if(!_r.l) _r.h++; _r.m=1; _r.t=8; }
void Z80::INCSP() { _r.sp++; _r.m=1; _r.t=8; }

void Z80::DECBC() { _r.c--; if(_r.c == 255) _r.b--; _r.m=1; _r.t=8; }
void Z80::DECDE() { _r.e--; if(_r.e == 255) _r.d--; _r.m=1; _r.t=8; }
void Z80::DECHL() { _r.l--; if(_r.l == 255) _r.h--; _r.m=1; _r.t=8; }
void Z80::DECSP() { _r.sp--; _r.m=1; _r.t=8; }

/* Operacje manipulacji bitami */
void Z80::BIT0b() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.b&0x01)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT0c() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.c&0x01)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT0d() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.d&0x01)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT0e() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.e&0x01)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT0h() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.h&0x01)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT0l() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.l&0x01)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT0a() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.a&0x01)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT0n() { _r.f&=0x1F; _r.f|=0x20; _r.f=((_mmu->rb((_r.h<<8)+_r.l))&0x01)?0:0x80; _r.m=2; _r.t=16; }

void Z80::RES0b() { _r.b&=0xFE; _r.m=2; _r.t=8; }
void Z80::RES0c() { _r.c&=0xFE; _r.m=2; _r.t=8; }
void Z80::RES0d() { _r.d&=0xFE; _r.m=2; _r.t=8; }
void Z80::RES0e() { _r.e&=0xFE; _r.m=2; _r.t=8; }
void Z80::RES0h() { _r.h&=0xFE; _r.m=2; _r.t=8; }
void Z80::RES0l() { _r.l&=0xFE; _r.m=2; _r.t=8; }
void Z80::RES0a() { _r.a&=0xFE; _r.m=2; _r.t=8; }
void Z80::RES0n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var&=0xFE; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SET0b() { _r.b|=0x01; _r.m=2; _r.t=8; }
void Z80::SET0c() { _r.c|=0x01; _r.m=2; _r.t=8; }
void Z80::SET0d() { _r.d|=0x01; _r.m=2; _r.t=8; }
void Z80::SET0e() { _r.e|=0x01; _r.m=2; _r.t=8; }
void Z80::SET0h() { _r.h|=0x01; _r.m=2; _r.t=8; }
void Z80::SET0l() { _r.l|=0x01; _r.m=2; _r.t=8; }
void Z80::SET0a() { _r.a|=0x01; _r.m=2; _r.t=8; }
void Z80::SET0n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var|=0x01; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::BIT1b() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.b&0x02)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT1c() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.c&0x02)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT1d() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.d&0x02)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT1e() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.e&0x02)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT1h() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.h&0x02)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT1l() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.l&0x02)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT1a() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.a&0x02)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT1n() { _r.f&=0x1F; _r.f|=0x20; _r.f=((_mmu->rb((_r.h<<8)+_r.l))&0x02)?0:0x80; _r.m=2; _r.t=16; }

void Z80::RES1b() { _r.b&=0xFD; _r.m=2; _r.t=8; }
void Z80::RES1c() { _r.c&=0xFD; _r.m=2; _r.t=8; }
void Z80::RES1d() { _r.d&=0xFD; _r.m=2; _r.t=8; }
void Z80::RES1e() { _r.e&=0xFD; _r.m=2; _r.t=8; }
void Z80::RES1h() { _r.h&=0xFD; _r.m=2; _r.t=8; }
void Z80::RES1l() { _r.l&=0xFD; _r.m=2; _r.t=8; }
void Z80::RES1a() { _r.a&=0xFD; _r.m=2; _r.t=8; }
void Z80::RES1n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var&=0xFD; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SET1b() { _r.b|=0x02; _r.m=2; _r.t=8; }
void Z80::SET1c() { _r.c|=0x02; _r.m=2; _r.t=8; }
void Z80::SET1d() { _r.d|=0x02; _r.m=2; _r.t=8; }
void Z80::SET1e() { _r.e|=0x02; _r.m=2; _r.t=8; }
void Z80::SET1h() { _r.h|=0x02; _r.m=2; _r.t=8; }
void Z80::SET1l() { _r.l|=0x02; _r.m=2; _r.t=8; }
void Z80::SET1a() { _r.a|=0x02; _r.m=2; _r.t=8; }
void Z80::SET1n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var|=0x02; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::BIT2b() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.b&0x04)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT2c() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.c&0x04)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT2d() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.d&0x04)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT2e() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.e&0x04)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT2h() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.h&0x04)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT2l() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.l&0x04)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT2a() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.a&0x04)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT2n() { _r.f&=0x1F; _r.f|=0x20; _r.f=((_mmu->rb((_r.h<<8)+_r.l))&0x04)?0:0x80; _r.m=2; _r.t=16; }

void Z80::RES2b() { _r.b&=0xFB; _r.m=2; _r.t=8; }
void Z80::RES2c() { _r.c&=0xFB; _r.m=2; _r.t=8; }
void Z80::RES2d() { _r.d&=0xFB; _r.m=2; _r.t=8; }
void Z80::RES2e() { _r.e&=0xFB; _r.m=2; _r.t=8; }
void Z80::RES2h() { _r.h&=0xFB; _r.m=2; _r.t=8; }
void Z80::RES2l() { _r.l&=0xFB; _r.m=2; _r.t=8; }
void Z80::RES2a() { _r.a&=0xFB; _r.m=2; _r.t=8; }
void Z80::RES2n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var&=0xFB; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SET2b() { _r.b|=0x04; _r.m=2; _r.t=8; }
void Z80::SET2c() { _r.c|=0x04; _r.m=2; _r.t=8; }
void Z80::SET2d() { _r.d|=0x04; _r.m=2; _r.t=8; }
void Z80::SET2e() { _r.e|=0x04; _r.m=2; _r.t=8; }
void Z80::SET2h() { _r.h|=0x04; _r.m=2; _r.t=8; }
void Z80::SET2l() { _r.l|=0x04; _r.m=2; _r.t=8; }
void Z80::SET2a() { _r.a|=0x04; _r.m=2; _r.t=8; }
void Z80::SET2n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var|=0x04; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::BIT3b() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.b&0x08)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT3c() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.c&0x08)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT3d() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.d&0x08)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT3e() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.e&0x08)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT3h() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.h&0x08)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT3l() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.l&0x08)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT3a() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.a&0x08)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT3n() { _r.f&=0x1F; _r.f|=0x20; _r.f=((_mmu->rb((_r.h<<8)+_r.l))&0x08)?0:0x80; _r.m=2; _r.t=16; }

void Z80::RES3b() { _r.b&=0xF7; _r.m=2; _r.t=8; }
void Z80::RES3c() { _r.c&=0xF7; _r.m=2; _r.t=8; }
void Z80::RES3d() { _r.d&=0xF7; _r.m=2; _r.t=8; }
void Z80::RES3e() { _r.e&=0xF7; _r.m=2; _r.t=8; }
void Z80::RES3h() { _r.h&=0xF7; _r.m=2; _r.t=8; }
void Z80::RES3l() { _r.l&=0xF7; _r.m=2; _r.t=8; }
void Z80::RES3a() { _r.a&=0xF7; _r.m=2; _r.t=8; }
void Z80::RES3n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var&=0xF7; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SET3b() { _r.b|=0x08; _r.m=2; _r.t=8; }
void Z80::SET3c() { _r.c|=0x08; _r.m=2; _r.t=8; }
void Z80::SET3d() { _r.d|=0x08; _r.m=2; _r.t=8; }
void Z80::SET3e() { _r.e|=0x08; _r.m=2; _r.t=8; }
void Z80::SET3h() { _r.h|=0x08; _r.m=2; _r.t=8; }
void Z80::SET3l() { _r.l|=0x08; _r.m=2; _r.t=8; }
void Z80::SET3a() { _r.a|=0x08; _r.m=2; _r.t=8; }
void Z80::SET3n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var|=0x08; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::BIT4b() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.b&0x10)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT4c() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.c&0x10)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT4d() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.d&0x10)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT4e() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.e&0x10)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT4h() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.h&0x10)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT4l() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.l&0x10)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT4a() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.a&0x10)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT4n() { _r.f&=0x1F; _r.f|=0x20; _r.f=((_mmu->rb((_r.h<<8)+_r.l))&0x10)?0:0x80; _r.m=2; _r.t=16; }

void Z80::RES4b() { _r.b&=0xEF; _r.m=2; _r.t=8; }
void Z80::RES4c() { _r.c&=0xEF; _r.m=2; _r.t=8; }
void Z80::RES4d() { _r.d&=0xEF; _r.m=2; _r.t=8; }
void Z80::RES4e() { _r.e&=0xEF; _r.m=2; _r.t=8; }
void Z80::RES4h() { _r.h&=0xEF; _r.m=2; _r.t=8; }
void Z80::RES4l() { _r.l&=0xEF; _r.m=2; _r.t=8; }
void Z80::RES4a() { _r.a&=0xEF; _r.m=2; _r.t=8; }
void Z80::RES4n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var&=0xEF; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SET4b() { _r.b|=0x10; _r.m=2; _r.t=8; }
void Z80::SET4c() { _r.c|=0x10; _r.m=2; _r.t=8; }
void Z80::SET4d() { _r.d|=0x10; _r.m=2; _r.t=8; }
void Z80::SET4e() { _r.e|=0x10; _r.m=2; _r.t=8; }
void Z80::SET4h() { _r.h|=0x10; _r.m=2; _r.t=8; }
void Z80::SET4l() { _r.l|=0x10; _r.m=2; _r.t=8; }
void Z80::SET4a() { _r.a|=0x10; _r.m=2; _r.t=8; }
void Z80::SET4n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var|=0x10; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::BIT5b() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.b&0x20)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT5c() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.c&0x20)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT5d() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.d&0x20)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT5e() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.e&0x20)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT5h() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.h&0x20)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT5l() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.l&0x20)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT5a() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.a&0x20)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT5n() { _r.f&=0x1F; _r.f|=0x20; _r.f=((_mmu->rb((_r.h<<8)+_r.l))&0x20)?0:0x80; _r.m=2; _r.t=16; }

void Z80::RES5b() { _r.b&=0xDF; _r.m=2; _r.t=8; }
void Z80::RES5c() { _r.c&=0xDF; _r.m=2; _r.t=8; }
void Z80::RES5d() { _r.d&=0xDF; _r.m=2; _r.t=8; }
void Z80::RES5e() { _r.e&=0xDF; _r.m=2; _r.t=8; }
void Z80::RES5h() { _r.h&=0xDF; _r.m=2; _r.t=8; }
void Z80::RES5l() { _r.l&=0xDF; _r.m=2; _r.t=8; }
void Z80::RES5a() { _r.a&=0xDF; _r.m=2; _r.t=8; }
void Z80::RES5n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var&=0xDF; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SET5b() { _r.b|=0x20; _r.m=2; _r.t=8; }
void Z80::SET5c() { _r.c|=0x20; _r.m=2; _r.t=8; }
void Z80::SET5d() { _r.d|=0x20; _r.m=2; _r.t=8; }
void Z80::SET5e() { _r.e|=0x20; _r.m=2; _r.t=8; }
void Z80::SET5h() { _r.h|=0x20; _r.m=2; _r.t=8; }
void Z80::SET5l() { _r.l|=0x20; _r.m=2; _r.t=8; }
void Z80::SET5a() { _r.a|=0x20; _r.m=2; _r.t=8; }
void Z80::SET5n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var|=0x20; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::BIT6b() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.b&0x40)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT6c() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.c&0x40)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT6d() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.d&0x40)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT6e() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.e&0x40)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT6h() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.h&0x40)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT6l() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.l&0x40)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT6a() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.a&0x40)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT6n() { _r.f&=0x1F; _r.f|=0x20; _r.f=((_mmu->rb((_r.h<<8)+_r.l))&0x40)?0:0x80; _r.m=2; _r.t=16; }

void Z80::RES6b() { _r.b&=0xBF; _r.m=2; _r.t=8; }
void Z80::RES6c() { _r.c&=0xBF; _r.m=2; _r.t=8; }
void Z80::RES6d() { _r.d&=0xBF; _r.m=2; _r.t=8; }
void Z80::RES6e() { _r.e&=0xBF; _r.m=2; _r.t=8; }
void Z80::RES6h() { _r.h&=0xBF; _r.m=2; _r.t=8; }
void Z80::RES6l() { _r.l&=0xBF; _r.m=2; _r.t=8; }
void Z80::RES6a() { _r.a&=0xBF; _r.m=2; _r.t=8; }
void Z80::RES6n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var&=0xBF; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SET6b() { _r.b|=0x40; _r.m=2; _r.t=8; }
void Z80::SET6c() { _r.c|=0x40; _r.m=2; _r.t=8; }
void Z80::SET6d() { _r.d|=0x40; _r.m=2; _r.t=8; }
void Z80::SET6e() { _r.e|=0x40; _r.m=2; _r.t=8; }
void Z80::SET6h() { _r.h|=0x40; _r.m=2; _r.t=8; }
void Z80::SET6l() { _r.l|=0x40; _r.m=2; _r.t=8; }
void Z80::SET6a() { _r.a|=0x40; _r.m=2; _r.t=8; }
void Z80::SET6n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var|=0x40; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::BIT7b() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.b&0x80)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT7c() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.c&0x80)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT7d() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.d&0x80)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT7e() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.e&0x80)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT7h() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.h&0x80)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT7l() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.l&0x80)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT7a() { _r.f&=0x1F; _r.f|=0x20; _r.f=(_r.a&0x80)?0:0x80; _r.m=2; _r.t=8; }
void Z80::BIT7n() { _r.f&=0x1F; _r.f|=0x20; _r.f=((_mmu->rb((_r.h<<8)+_r.l))&0x80)?0:0x80; _r.m=2; _r.t=16; }

void Z80::RES7b() { _r.b&=0x7F; _r.m=2; _r.t=8; }
void Z80::RES7c() { _r.c&=0x7F; _r.m=2; _r.t=8; }
void Z80::RES7d() { _r.d&=0x7F; _r.m=2; _r.t=8; }
void Z80::RES7e() { _r.e&=0x7F; _r.m=2; _r.t=8; }
void Z80::RES7h() { _r.h&=0x7F; _r.m=2; _r.t=8; }
void Z80::RES7l() { _r.l&=0x7F; _r.m=2; _r.t=8; }
void Z80::RES7a() { _r.a&=0x7F; _r.m=2; _r.t=8; }
void Z80::RES7n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var&=0x7F; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SET7b() { _r.b|=0x80; _r.m=2; _r.t=8; }
void Z80::SET7c() { _r.c|=0x80; _r.m=2; _r.t=8; }
void Z80::SET7d() { _r.d|=0x80; _r.m=2; _r.t=8; }
void Z80::SET7e() { _r.e|=0x80; _r.m=2; _r.t=8; }
void Z80::SET7h() { _r.h|=0x80; _r.m=2; _r.t=8; }
void Z80::SET7l() { _r.l|=0x80; _r.m=2; _r.t=8; }
void Z80::SET7a() { _r.a|=0x80; _r.m=2; _r.t=8; }
void Z80::SET7n() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); var|=0x80; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::RLA() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.a&0x80?0x10:0; _r.a=(_r.a<<1)+cf; _r.f=(_r.f&0xEF)+overflow; _r.m=1; _r.t=4; }
void Z80::RLCA() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.a&0x80?0x10:0; _r.a=(_r.a<<1)+cf; _r.f=(_r.f&0xEF)+overflow; _r.m=1; _r.t=4; }
void Z80::RRA() { unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=_r.a&1?0x10:0; _r.a=(_r.a>>1)+cf; _r.f=(_r.f&0xEF)+overflow; _r.m=1; _r.t=4; }
void Z80::RRCA() { unsigned char cf=_r.a&1?0x80:0; unsigned char overflow=_r.a&1?0x10:0; _r.a=(_r.a>>1)+cf; _r.f=(_r.f&0xEF)+overflow; _r.m=1; _r.t=4; }

void Z80::RLr_b() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.b&0x80?0x10:0; _r.b=(_r.b<<1)+cf; _r.f=(_r.b)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLr_c() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.c&0x80?0x10:0; _r.c=(_r.c<<1)+cf; _r.f=(_r.c)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLr_d() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.d&0x80?0x10:0; _r.d=(_r.d<<1)+cf; _r.f=(_r.d)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLr_e() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.e&0x80?0x10:0; _r.e=(_r.e<<1)+cf; _r.f=(_r.e)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLr_h() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.h&0x80?0x10:0; _r.h=(_r.h<<1)+cf; _r.f=(_r.h)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLr_l() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.l&0x80?0x10:0; _r.l=(_r.l<<1)+cf; _r.f=(_r.l)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLr_a() { unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=_r.a&0x80?0x10:0; _r.a=(_r.a<<1)+cf; _r.f=(_r.a)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLHL() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); unsigned char cf=_r.f&0x10?1:0; unsigned char overflow=var&0x80?0x10:0; var=(var<<1)+cf; _r.f=(var)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::RLCr_b() { unsigned char cf=_r.b&0x80?1:0; unsigned char overflow=_r.b&0x80?0x10:0; _r.b=(_r.b<<1)+cf; _r.f=(_r.b)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLCr_c() { unsigned char cf=_r.c&0x80?1:0; unsigned char overflow=_r.c&0x80?0x10:0; _r.c=(_r.c<<1)+cf; _r.f=(_r.c)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLCr_d() { unsigned char cf=_r.d&0x80?1:0; unsigned char overflow=_r.d&0x80?0x10:0; _r.d=(_r.d<<1)+cf; _r.f=(_r.d)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLCr_e() { unsigned char cf=_r.e&0x80?1:0; unsigned char overflow=_r.e&0x80?0x10:0; _r.e=(_r.e<<1)+cf; _r.f=(_r.e)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLCr_h() { unsigned char cf=_r.h&0x80?1:0; unsigned char overflow=_r.h&0x80?0x10:0; _r.h=(_r.h<<1)+cf; _r.f=(_r.h)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLCr_l() { unsigned char cf=_r.l&0x80?1:0; unsigned char overflow=_r.l&0x80?0x10:0; _r.l=(_r.l<<1)+cf; _r.f=(_r.l)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLCr_a() { unsigned char cf=_r.a&0x80?1:0; unsigned char overflow=_r.a&0x80?0x10:0; _r.a=(_r.a<<1)+cf; _r.f=(_r.a)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RLCHL() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); unsigned char cf=var&0x80?1:0; unsigned char overflow=var&0x80?0x10:0; var=(var<<1)+cf; _r.f=(var)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::RRr_b() { unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=_r.b&1?0x10:0; _r.b=(_r.b>>1)+cf; _r.f=(_r.b)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRr_c() { unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=_r.c&1?0x10:0; _r.c=(_r.c>>1)+cf; _r.f=(_r.c)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRr_d() { unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=_r.d&1?0x10:0; _r.d=(_r.d>>1)+cf; _r.f=(_r.d)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRr_e() { unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=_r.e&1?0x10:0; _r.e=(_r.e>>1)+cf; _r.f=(_r.e)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRr_h() { unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=_r.h&1?0x10:0; _r.h=(_r.h>>1)+cf; _r.f=(_r.h)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRr_l() { unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=_r.l&1?0x10:0; _r.l=(_r.l>>1)+cf; _r.f=(_r.l)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRr_a() { unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=_r.a&1?0x10:0; _r.a=(_r.a>>1)+cf; _r.f=(_r.a)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRHL() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); unsigned char cf=_r.f&0x10?0x80:0; unsigned char overflow=var&1?0x10:0; var=(var>>1)+cf; _r.f=(var)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::RRCr_b() { unsigned char cf=_r.b&1?0x80:0; unsigned char overflow=_r.b&1?0x10:0; _r.b=(_r.b>>1)+cf; _r.f=(_r.b)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRCr_c() { unsigned char cf=_r.c&1?0x80:0; unsigned char overflow=_r.c&1?0x10:0; _r.c=(_r.c>>1)+cf; _r.f=(_r.c)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRCr_d() { unsigned char cf=_r.d&1?0x80:0; unsigned char overflow=_r.d&1?0x10:0; _r.d=(_r.d>>1)+cf; _r.f=(_r.d)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRCr_e() { unsigned char cf=_r.e&1?0x80:0; unsigned char overflow=_r.e&1?0x10:0; _r.e=(_r.e>>1)+cf; _r.f=(_r.e)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRCr_h() { unsigned char cf=_r.h&1?0x80:0; unsigned char overflow=_r.h&1?0x10:0; _r.h=(_r.h>>1)+cf; _r.f=(_r.h)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRCr_l() { unsigned char cf=_r.l&1?0x80:0; unsigned char overflow=_r.l&1?0x10:0; _r.l=(_r.l>>1)+cf; _r.f=(_r.l)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRCr_a() { unsigned char cf=_r.a&1?0x80:0; unsigned char overflow=_r.a&1?0x10:0; _r.a=(_r.a>>1)+cf; _r.f=(_r.a)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::RRCHL() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); unsigned char cf=var&1?0x80:0; unsigned char overflow=var&1?0x10:0; var=(var>>1)+cf; _r.f=(var)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SLAr_b() { unsigned char overflow=_r.b&0x80?0x10:0; _r.b=_r.b<<1; _r.f=_r.b?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SLAr_c() { unsigned char overflow=_r.c&0x80?0x10:0; _r.c=_r.c<<1; _r.f=_r.c?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SLAr_d() { unsigned char overflow=_r.d&0x80?0x10:0; _r.d=_r.d<<1; _r.f=_r.d?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SLAr_e() { unsigned char overflow=_r.e&0x80?0x10:0; _r.e=_r.e<<1; _r.f=_r.e?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SLAr_h() { unsigned char overflow=_r.h&0x80?0x10:0; _r.h=_r.h<<1; _r.f=_r.h?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SLAr_l() { unsigned char overflow=_r.l&0x80?0x10:0; _r.l=_r.l<<1; _r.f=_r.l?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SLAr_a() { unsigned char overflow=_r.a&0x80?0x10:0; _r.a=_r.a<<1; _r.f=_r.a?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SLAHL() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); unsigned char overflow=var&0x80?0x10:0; var=var<<1; _r.f=var?0:0x80; _r.f=(_r.f&0xEF)+overflow; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SRAr_b() { unsigned char cf=_r.b&0x80; unsigned char overflow=_r.b&1?0x10:0; _r.b=(_r.b>>1)+cf; _r.f=_r.b?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRAr_c() { unsigned char cf=_r.c&0x80; unsigned char overflow=_r.c&1?0x10:0; _r.c=(_r.c>>1)+cf; _r.f=_r.c?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRAr_d() { unsigned char cf=_r.d&0x80; unsigned char overflow=_r.d&1?0x10:0; _r.d=(_r.d>>1)+cf; _r.f=_r.d?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRAr_e() { unsigned char cf=_r.e&0x80; unsigned char overflow=_r.e&1?0x10:0; _r.e=(_r.e>>1)+cf; _r.f=_r.e?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRAr_h() { unsigned char cf=_r.h&0x80; unsigned char overflow=_r.h&1?0x10:0; _r.h=(_r.h>>1)+cf; _r.f=_r.h?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRAr_l() { unsigned char cf=_r.l&0x80; unsigned char overflow=_r.l&1?0x10:0; _r.l=(_r.l>>1)+cf; _r.f=_r.l?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRAr_a() { unsigned char cf=_r.a&0x80; unsigned char overflow=_r.a&1?0x10:0; _r.a=(_r.a>>1)+cf; _r.f=_r.a?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRAHL() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); unsigned char cf=var&0x80; unsigned char overflow=var&1?0x10:0; var=(var>>1)+cf; _r.f=var?0:0x80; _r.f=(_r.f&0xEF)+overflow; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

void Z80::SRLr_b() { unsigned char overflow=_r.b&1?0x10:0; _r.b=(_r.b>>1); _r.f=(_r.b)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRLr_c() { unsigned char overflow=_r.c&1?0x10:0; _r.c=(_r.c>>1); _r.f=(_r.c)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRLr_d() { unsigned char overflow=_r.d&1?0x10:0; _r.d=(_r.d>>1); _r.f=(_r.d)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRLr_e() { unsigned char overflow=_r.e&1?0x10:0; _r.e=(_r.e>>1); _r.f=(_r.e)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRLr_h() { unsigned char overflow=_r.h&1?0x10:0; _r.h=(_r.h>>1); _r.f=(_r.h)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRLr_l() { unsigned char overflow=_r.l&1?0x10:0; _r.l=(_r.l>>1); _r.f=(_r.l)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRLr_a() { unsigned char overflow=_r.a&1?0x10:0; _r.a=(_r.a>>1); _r.f=(_r.a)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _r.m=2; _r.t=8; }
void Z80::SRLHL() { unsigned char var=_mmu->rb((_r.h<<8)+_r.l); unsigned char overflow=var&1?0x10:0; var=(var>>1); _r.f=(var)?0:0x80; _r.f=(_r.f&0xEF)+overflow; _mmu->wb((_r.h<<8)+_r.l, var); _r.m=2; _r.t=16; }

//Logical not (Complement) na a
void Z80::CPL() { _r.a ^= 255; _r.f&=0x60; _r.m=1; _r.t=4; }
//Opuœæ, podnieœ CF
void Z80::CCF() { unsigned char cf=_r.f&0x10?0:0x10; _r.f=(_r.f&0xEF)+cf; _r.m=1; _r.t=4; }
void Z80::SCF() { _r.f&=(!_r.f&0x80)?0x90:0x10; _r.m=1; _r.t=4; }

/* Operacje stosowe */
void Z80::PUSHBC() { _r.sp--; _mmu->wb(_r.sp,_r.b); _r.sp--; _mmu->wb(_r.sp,_r.c); _r.m=1; _r.t=16; }
void Z80::PUSHDE() { _r.sp--; _mmu->wb(_r.sp,_r.d); _r.sp--; _mmu->wb(_r.sp,_r.e); _r.m=1; _r.t=16; }
void Z80::PUSHHL() { _r.sp--; _mmu->wb(_r.sp,_r.h); _r.sp--; _mmu->wb(_r.sp,_r.l); _r.m=1; _r.t=16; }
void Z80::PUSHAF() { _r.sp--; _mmu->wb(_r.sp,_r.a); _r.sp--; _mmu->wb(_r.sp,_r.f); _r.m=1; _r.t=16; }

void Z80::POPBC() { _r.c=_mmu->rb(_r.sp); _r.sp++; _r.b=_mmu->rb(_r.sp); _r.sp++; _r.m=1; _r.t=16; }
void Z80::POPDE() { _r.e=_mmu->rb(_r.sp); _r.sp++; _r.d=_mmu->rb(_r.sp); _r.sp++; _r.m=1; _r.t=16; }
void Z80::POPHL() { _r.l=_mmu->rb(_r.sp); _r.sp++; _r.h=_mmu->rb(_r.sp); _r.sp++; _r.m=1; _r.t=16; }
void Z80::POPAF() { _r.a=_mmu->rb(_r.sp); _r.sp++; _r.f=_mmu->rb(_r.sp); _r.sp++; _r.m=1; _r.t=16; }

/* Operacje skokowe */
void Z80::JPnn() { _r.pc = _mmu->rw(_r.pc); _r.m=3; _r.t=16; }	
void Z80::JPHL() { _r.pc=(_r.h<<8)+_r.l; _r.m=1; _r.t=4; }
//Skok do lokacji gdy ostatni wynik nie by³ zerem
void Z80::JPNZnn() { _r.t=12; if((_r.f&0x80)==0x00) { _r.pc=_mmu->rw(_r.pc); _r.t+=4; } else _r.pc+=2; _r.m=3; }
void Z80::JPZnn() { _r.t=12; if((_r.f&0x80)==0x80) { _r.pc=_mmu->rw(_r.pc); _r.t+=4; } else _r.pc+=2; _r.m=3; }
void Z80::JPNCnn() { _r.t=12; if((_r.f&0x10)==0x00) { _r.pc=_mmu->rw(_r.pc); _r.t+=4; } else _r.pc+=2; _r.m=3; }
void Z80::JPCnn() { _r.t=12; if((_r.f&0x10)==0x10) { _r.pc=_mmu->rw(_r.pc); _r.t+=4; } else _r.pc+=2; _r.m=3;}

void Z80::JRn() { char var=_mmu->rb(_r.pc); _r.pc++; _r.pc+=var; _r.m=3; _r.t=12; }
void Z80::JRNZn() { char var=_mmu->rb(_r.pc); _r.pc++; _r.t=8; if((_r.f&0x80)==0x00) { _r.pc+=var; _r.t+=4; } _r.m=2; }
void Z80::JRZn() { char var=_mmu->rb(_r.pc); _r.pc++; _r.t=8; if((_r.f&0x80)==0x80) { _r.pc+=var; _r.t+=4; } _r.m=2; }
void Z80::JRNCn() { char var=_mmu->rb(_r.pc); _r.pc++; _r.t=8; if((_r.f&0x10)==0x00) { _r.pc+=var; _r.t+=4; } _r.m=2; }
void Z80::JRCn() { char var=_mmu->rb(_r.pc); _r.pc++; _r.t=8; if((_r.f&0x10)==0x10) { _r.pc+=var; _r.t+=4; } _r.m=2; }

void Z80::STOP() { _stop=1; _r.pc++; _r.m=2; _r.t=4; }

void Z80::CALLnn() { _r.sp-=2; _mmu->ww(_r.sp,_r.pc+2); _r.pc=_mmu->rw(_r.pc); _r.m=3; _r.t=24; }
void Z80::CALLNZnn() { _r.t=12; if((_r.f&0x80)==0x00) { _r.sp-=2; _mmu->ww(_r.sp,_r.pc+2); _r.pc=_mmu->rw(_r.pc); _r.t+=12; } else _r.pc+=2; _r.m=3; }
void Z80::CALLZnn() { _r.t=12; if((_r.f&0x80)==0x80) { _r.sp-=2; _mmu->ww(_r.sp,_r.pc+2); _r.pc=_mmu->rw(_r.pc); _r.t+=12; } else _r.pc+=2; _r.m=3; }
void Z80::CALLNCnn() { _r.t=12; if((_r.f&0x10)==0x00) { _r.sp-=2; _mmu->ww(_r.sp,_r.pc+2); _r.pc=_mmu->rw(_r.pc); _r.t+=12; } else _r.pc+=2; _r.m=3; }
void Z80::CALLCnn() {_r.t=12; if((_r.f&0x10)==0x10) { _r.sp-=2; _mmu->ww(_r.sp,_r.pc+2); _r.pc=_mmu->rw(_r.pc); _r.t+=12; } else _r.pc+=2; _r.m=3; }

void Z80::RET() { _r.pc=_mmu->rw(_r.sp); _r.sp+=2; _r.m=1; _r.t=16; }
void Z80::RETI() { _r.ime=1; wczytajRejestry(); _r.pc=_mmu->rw(_r.sp); _r.sp+=2; _r.m=1; _r.t=16; }
void Z80::RETNZ() { _r.t=8; if((_r.f&0x80)==0x00) { _r.pc=_mmu->rw(_r.sp); _r.sp+=2; _r.t+=12; } _r.m=1; }
void Z80::RETZ() { _r.t=8; if((_r.f&0x80)==0x80) { _r.pc=_mmu->rw(_r.sp); _r.sp+=2; _r.t+=12; } _r.m=1; }
void Z80::RETNC() { _r.t=8; if((_r.f&0x10)==0x00) { _r.pc=_mmu->rw(_r.sp); _r.sp+=2; _r.t+=12; } _r.m=1; }
void Z80::RETC() { _r.t=8; if((_r.f&0x10)==0x10) { _r.pc=_mmu->rw(_r.sp); _r.sp+=2; _r.t+=12; } _r.m=1; }

void Z80::RST00() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x00; _r.m=1; _r.t=16; }
void Z80::RST08() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x08; _r.m=1; _r.t=16; }
void Z80::RST10() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x10; _r.m=1; _r.t=16; }
void Z80::RST18() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x18; _r.m=1; _r.t=16; }
void Z80::RST20() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x20; _r.m=1; _r.t=16; }
void Z80::RST28() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x28; _r.m=1; _r.t=16; }
void Z80::RST30() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x30; _r.m=1; _r.t=16; }
void Z80::RST38() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x38; _r.m=1; _r.t=16; }
void Z80::RST40() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x40; _r.m=1; _r.t=16; }
void Z80::RST48() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x48; _r.m=1; _r.t=16; }
void Z80::RST50() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x50; _r.m=1; _r.t=16; }
void Z80::RST58() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x58; _r.m=1; _r.t=16; }
void Z80::RST60() { zapiszRejestry(); _r.sp-=2; _mmu->ww(_r.sp,_r.pc); _r.pc=0x60; _r.m=1; _r.t=16; }

void Z80::NOP() { _r.m=1; _r.t=4; }
void Z80::HALT() { _halt=1; _r.m=1; _r.t=4; }

//Zablokuj/odblokuj przerwania
void Z80::DI() { _r.ime=0; _r.m=1; _r.t=4; }
void Z80::EI() { _r.ime=1; _r.m=1; _r.t=4; }

/* Domniemane puste miejsce */
void Z80::XX() {std::cout<<"Strzal w puste miejsce!"<<std::endl; }

/* Prefiks polecenia dwubajtowego, tu powinno byæ wywo³anie funkcji z tablicy dwubajtowej. */
void Z80::MAPcb() { unsigned char var=_mmu->rb(_r.pc); _r.pc++; if(_CBmap[var]) (this->*_CBmap[var])(); else std::cout<<"Blad w wejsciu CB!"<<std::endl; }

/* Pomocnicze */

//Zapis rejestrów w zapas
void Z80::zapiszRejestry()
{
	_rsv.a = _r.a; _rsv.b = _r.b;
	_rsv.c = _r.c; _rsv.d = _r.d;
	_rsv.e = _r.e; _rsv.f = _r.f;
	_rsv.h = _r.h; _rsv.l = _r.l;
}

//Wczytanie z zapasu do rejestrów
void Z80::wczytajRejestry()
{
	_r.a = _rsv.a; _r.b = _rsv.b;
	_r.c = _rsv.c; _r.d = _rsv.d;
	_r.e = _rsv.e; _r.f = _rsv.f;
	_r.h = _rsv.h; _r.l = _rsv.l;
}
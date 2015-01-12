
#include "Z80.h"

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
	(this->*_map[mmu.rb(_r.pc++)])();
	_r.pc &= 65535;
	_clock.m += _r.m;
}

void Z80::init()
{
	R _r = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	Clock _clock = {0, 0};
}

Z80::OpCodeMap Z80::_map[] = 
	{ 
		// 00
		&Z80::NOP,		&Z80::LDBCnn,	&Z80::LDBCnA,	&Z80::INCBC,
		&Z80::INCr_b,	&Z80::DECr_b,	&Z80::LDrn_b,	&Z80::RLCA,
		&Z80::LDnnSP,	&Z80::ADDHLBC,	&Z80::LDABCn,	&Z80::DECBC,
		&Z80::INCr_c,	&Z80::DECr_c,	&Z80::LDrn_c,	&Z80::RRCA,
		// 10
		&Z80::DJNZn,	&Z80::LDDEnn,	&Z80::LDDEnA,	&Z80::INCDE,
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

//////////////////////////////////////////////////
//												//
//				Lista instrukcji				//
// http://gameboy.mongenel.com/dmg/opcodes.html	//
//////////////////////////////////////////////////

//r - rejestr, n - adres pamieci
//r - 8b, rr - 16b, tak samo n.

/* Operacje wczytywania i zapisu */
//Z rejestru do rejestru
void Z80::LDrr_bb() { Z80::_r.b = Z80::_r.b; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_bc() { Z80::_r.b = Z80::_r.c; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_bd() { Z80::_r.b = Z80::_r.d; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_be() { Z80::_r.b = Z80::_r.e; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_bh() { Z80::_r.b = Z80::_r.h; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_bl() { Z80::_r.b = Z80::_r.l; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ba() { Z80::_r.b = Z80::_r.a; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_cb() { Z80::_r.c = Z80::_r.b; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_cc() { Z80::_r.c = Z80::_r.c; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_cd() { Z80::_r.c = Z80::_r.d; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ce() { Z80::_r.c = Z80::_r.e; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ch() { Z80::_r.c = Z80::_r.h; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_cl() { Z80::_r.c = Z80::_r.l; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ca() { Z80::_r.c = Z80::_r.a; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_db() { Z80::_r.d = Z80::_r.b; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_dc() { Z80::_r.d = Z80::_r.c; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_dd() { Z80::_r.d = Z80::_r.d; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_de() { Z80::_r.d = Z80::_r.e; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_dh() { Z80::_r.d = Z80::_r.h; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_dl() { Z80::_r.d = Z80::_r.l; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_da() { Z80::_r.d = Z80::_r.a; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_eb() { Z80::_r.e = Z80::_r.b; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ec() { Z80::_r.e = Z80::_r.c; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ed() { Z80::_r.e = Z80::_r.d; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ee() { Z80::_r.e = Z80::_r.e; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_eh() { Z80::_r.e = Z80::_r.h; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_el() { Z80::_r.e = Z80::_r.l; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ea() { Z80::_r.e = Z80::_r.a; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_hb() { Z80::_r.h = Z80::_r.b; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_hc() { Z80::_r.h = Z80::_r.c; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_hd() { Z80::_r.h = Z80::_r.d; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_he() { Z80::_r.h = Z80::_r.e; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_hh() { Z80::_r.h = Z80::_r.h; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_hl() { Z80::_r.h = Z80::_r.l; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ha() { Z80::_r.h = Z80::_r.a; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_lb() { Z80::_r.l = Z80::_r.b; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_lc() { Z80::_r.l = Z80::_r.c; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ld() { Z80::_r.l = Z80::_r.d; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_le() { Z80::_r.l = Z80::_r.e; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_lh() { Z80::_r.l = Z80::_r.h; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ll() { Z80::_r.l = Z80::_r.l; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_la() { Z80::_r.l = Z80::_r.a; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ab() { Z80::_r.a = Z80::_r.b; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ac() { Z80::_r.a = Z80::_r.c; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ad() { Z80::_r.a = Z80::_r.d; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ae() { Z80::_r.a = Z80::_r.e; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_ah() { Z80::_r.a = Z80::_r.h; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_al() { Z80::_r.a = Z80::_r.l; Z80::_r.m = 1;  Z80::_r.t = 4; }
void Z80::LDrr_aa() { Z80::_r.a = Z80::_r.a; Z80::_r.m = 1;  Z80::_r.t = 4; }

//Z rejestru do adresu
void Z80::LDrHLn_b() { }
void Z80::LDrHLn_c() { }
void Z80::LDrHLn_d() { }
void Z80::LDrHLn_e() { }
void Z80::LDrHLn_h() { }
void Z80::LDrHLn_l() { }
void Z80::LDrHLn_a() { }

void Z80::LDHLnr_b() { }
void Z80::LDHLnr_c() { }
void Z80::LDHLnr_d() { }
void Z80::LDHLnr_e() { }
void Z80::LDHLnr_h() { }
void Z80::LDHLnr_l() { }
void Z80::LDHLnr_a() { }

void Z80::LDrn_b() { }
void Z80::LDrn_c() { }
void Z80::LDrn_d() { }
void Z80::LDrn_e() { }
void Z80::LDrn_h() { }
void Z80::LDrn_l() { }
void Z80::LDrn_a() { }

void Z80::LDHLnn() { }

void Z80::LDBCnA() { }
void Z80::LDDEnA() { }

void Z80::LDnnA() { }

void Z80::LDABCn() { }
void Z80::LDADEn() { }

void Z80::LDAnn() { }

void Z80::LDBCnn() { }
void Z80::LDDEnn() { }
void Z80::LDHLnn() { }
void Z80::LDSPnn() { }

void Z80::LDHLnn() { }
void Z80::LDnnHL() { }

void Z80::LDHLIA() { }
void Z80::LDAHLI() { }

void Z80::LDHLDA() { }
void Z80::LDAHLD() { }

void Z80::LDAIOn() { }
void Z80::LDIOnA() { }
void Z80::LDAIOC() { }
void Z80::LDIOCA() { }

void Z80::LDHLSPn() { }
void Z80::LDnnSP() { }

void Z80::SWAPr_b() { }
void Z80::SWAPr_c() { }
void Z80::SWAPr_d() { }
void Z80::SWAPr_e() { }
void Z80::SWAPr_h() { }
void Z80::SWAPr_l() { }
void Z80::SWAPr_a() { }

/* Operacje przetwarzania danych */
//Wszystkie operacje logiczne i arytmetyczne sa wykonywane na rejestrze A i podanym drugim rejestrze.
void Z80::ADDr_b() { }
void Z80::ADDr_c() { }
void Z80::ADDr_d() { }
void Z80::ADDr_e() { }
void Z80::ADDr_h() { }
void Z80::ADDr_l() { }
void Z80::ADDr_a() { }
void Z80::ADDHL() { }
void Z80::ADDn() { }
void Z80::ADDHLBC() { }
void Z80::ADDHLDE() { }
void Z80::ADDHLHL() { }
void Z80::ADDHLSP() { }
void Z80::ADDSPn() { }

void Z80::ADCr_b() { }
void Z80::ADCr_c() { }
void Z80::ADCr_d() { }
void Z80::ADCr_e() { }
void Z80::ADCr_h() { }
void Z80::ADCr_l() { }
void Z80::ADCr_a() { }
void Z80::ADCHL() { }
void Z80::ADCn() { }

void Z80::SUBr_b() { }
void Z80::SUBr_c() { }
void Z80::SUBr_d() { }
void Z80::SUBr_e() { }
void Z80::SUBr_h() { }
void Z80::SUBr_l() { }
void Z80::SUBr_a() { }
void Z80::SUBHL() { }
void Z80::SUBn() { }

void Z80::SBCr_b() { }
void Z80::SBCr_c() { }
void Z80::SBCr_d() { }
void Z80::SBCr_e() { }
void Z80::SBCr_h() { }
void Z80::SBCr_l() { }
void Z80::SBCr_a() { }
void Z80::SBCHL() { }
void Z80::SBCn() { }

void Z80::CPr_b() { }
void Z80::CPr_c() { }
void Z80::CPr_d() { }
void Z80::CPr_e() { }
void Z80::CPr_h() { }
void Z80::CPr_l() { }
void Z80::CPr_a() { }
void Z80::CPHL() { }
void Z80::CPn() { }

void Z80::DAA() { }

void Z80::ANDr_b() { }
void Z80::ANDr_c() { }
void Z80::ANDr_d() { }
void Z80::ANDr_e() { }
void Z80::ANDr_h() { }
void Z80::ANDr_l() { }
void Z80::ANDr_a() { }
void Z80::ANDHL() { }
void Z80::ANDn() { }

void Z80::ORr_b() { }
void Z80::ORr_c() { }
void Z80::ORr_d() { }
void Z80::ORr_e() { }
void Z80::ORr_h() { }
void Z80::ORr_l() { }
void Z80::ORr_a() { }
void Z80::ORHL() { }
void Z80::ORn() { }

void Z80::XORr_b() { }
void Z80::XORr_c() { }
void Z80::XORr_d() { }
void Z80::XORr_e() { }
void Z80::XORr_h() { }
void Z80::XORr_l() { }
void Z80::XORr_a() { }
void Z80::XORHL() { }
void Z80::XORn() { }

void Z80::INCr_b() { }
void Z80::INCr_c() { }
void Z80::INCr_d() { }
void Z80::INCr_e() { }
void Z80::INCr_h() { }
void Z80::INCr_l() { }
void Z80::INCr_a() { }
void Z80::INCHLn() { }

void Z80::DECr_b() { }
void Z80::DECr_c() { }
void Z80::DECr_d() { }
void Z80::DECr_e() { }
void Z80::DECr_h() { }
void Z80::DECr_l() { }
void Z80::DECr_a() { }
void Z80::DECHLn() { }

void Z80::INCBC() { }
void Z80::INCDE() { }
void Z80::INCHL() { }
void Z80::INCSP() { }

void Z80::DECBC() { }
void Z80::DECDE() { }
void Z80::DECHL() { }
void Z80::DECSP() { }

/* Operacje manipulacji bitami */
void Z80::BIT0b() { }
void Z80::BIT0c() { }
void Z80::BIT0d() { }
void Z80::BIT0e() { }
void Z80::BIT0h() { }
void Z80::BIT0l() { }
void Z80::BIT0a() { }
void Z80::BIT0n() { }

void Z80::RES0b() { }
void Z80::RES0c() { }
void Z80::RES0d() { }
void Z80::RES0e() { }
void Z80::RES0h() { }
void Z80::RES0l() { }
void Z80::RES0a() { }
void Z80::RES0n() { }

void Z80::SET0b() { }
void Z80::SET0c() { }
void Z80::SET0d() { }
void Z80::SET0e() { }
void Z80::SET0h() { }
void Z80::SET0l() { }
void Z80::SET0a() { }
void Z80::SET0n() { }

void Z80::BIT1b() { }
void Z80::BIT1c() { }
void Z80::BIT1d() { }
void Z80::BIT1e() { }
void Z80::BIT1h() { }
void Z80::BIT1l() { }
void Z80::BIT1a() { }
void Z80::BIT1n() { }

void Z80::RES1b() { }
void Z80::RES1c() { }
void Z80::RES1d() { }
void Z80::RES1e() { }
void Z80::RES1h() { }
void Z80::RES1l() { }
void Z80::RES1a() { }
void Z80::RES1n() { }

void Z80::SET1b() { }
void Z80::SET1c() { }
void Z80::SET1d() { }
void Z80::SET1e() { }
void Z80::SET1h() { }
void Z80::SET1l() { }
void Z80::SET1a() { }
void Z80::SET1n() { }

void Z80::BIT2b() { }
void Z80::BIT2c() { }
void Z80::BIT2d() { }
void Z80::BIT2e() { }
void Z80::BIT2h() { }
void Z80::BIT2l() { }
void Z80::BIT2a() { }
void Z80::BIT2n() { }

void Z80::RES2b() { }
void Z80::RES2c() { }
void Z80::RES2d() { }
void Z80::RES2e() { }
void Z80::RES2h() { }
void Z80::RES2l() { }
void Z80::RES2a() { }
void Z80::RES2n() { }

void Z80::SET2b() { }
void Z80::SET2c() { }
void Z80::SET2d() { }
void Z80::SET2e() { }
void Z80::SET2h() { }
void Z80::SET2l() { }
void Z80::SET2a() { }
void Z80::SET2n() { }

void Z80::BIT3b() { }
void Z80::BIT3c() { }
void Z80::BIT3d() { }
void Z80::BIT3e() { }
void Z80::BIT3h() { }
void Z80::BIT3l() { }
void Z80::BIT3a() { }
void Z80::BIT3n() { }

void Z80::RES3b() { }
void Z80::RES3c() { }
void Z80::RES3d() { }
void Z80::RES3e() { }
void Z80::RES3h() { }
void Z80::RES3l() { }
void Z80::RES3a() { }
void Z80::RES3n() { }

void Z80::SET3b() { }
void Z80::SET3c() { }
void Z80::SET3d() { }
void Z80::SET3e() { }
void Z80::SET3h() { }
void Z80::SET3l() { }
void Z80::SET3a() { }
void Z80::SET3n() { }

void Z80::BIT4b() { }
void Z80::BIT4c() { }
void Z80::BIT4d() { }
void Z80::BIT4e() { }
void Z80::BIT4h() { }
void Z80::BIT4l() { }
void Z80::BIT4a() { }
void Z80::BIT4n() { }

void Z80::RES4b() { }
void Z80::RES4c() { }
void Z80::RES4d() { }
void Z80::RES4e() { }
void Z80::RES4h() { }
void Z80::RES4l() { }
void Z80::RES4a() { }
void Z80::RES4n() { }

void Z80::SET4b() { }
void Z80::SET4c() { }
void Z80::SET4d() { }
void Z80::SET4e() { }
void Z80::SET4h() { }
void Z80::SET4l() { }
void Z80::SET4a() { }
void Z80::SET4n() { }

void Z80::BIT5b() { }
void Z80::BIT5c() { }
void Z80::BIT5d() { }
void Z80::BIT5e() { }
void Z80::BIT5h() { }
void Z80::BIT5l() { }
void Z80::BIT5a() { }
void Z80::BIT5n() { }

void Z80::RES5b() { }
void Z80::RES5c() { }
void Z80::RES5d() { }
void Z80::RES5e() { }
void Z80::RES5h() { }
void Z80::RES5l() { }
void Z80::RES5a() { }
void Z80::RES5n() { }

void Z80::SET5b() { }
void Z80::SET5c() { }
void Z80::SET5d() { }
void Z80::SET5e() { }
void Z80::SET5h() { }
void Z80::SET5l() { }
void Z80::SET5a() { }
void Z80::SET5n() { }

void Z80::BIT6b() { }
void Z80::BIT6c() { }
void Z80::BIT6d() { }
void Z80::BIT6e() { }
void Z80::BIT6h() { }
void Z80::BIT6l() { }
void Z80::BIT6a() { }
void Z80::BIT6n() { }

void Z80::RES6b() { }
void Z80::RES6c() { }
void Z80::RES6d() { }
void Z80::RES6e() { }
void Z80::RES6h() { }
void Z80::RES6l() { }
void Z80::RES6a() { }
void Z80::RES6n() { }

void Z80::SET6b() { }
void Z80::SET6c() { }
void Z80::SET6d() { }
void Z80::SET6e() { }
void Z80::SET6h() { }
void Z80::SET6l() { }
void Z80::SET6a() { }
void Z80::SET6n() { }

void Z80::BIT7b() { }
void Z80::BIT7c() { }
void Z80::BIT7d() { }
void Z80::BIT7e() { }
void Z80::BIT7h() { }
void Z80::BIT7l() { }
void Z80::BIT7a() { }
void Z80::BIT7n() { }

void Z80::RES7b() { }
void Z80::RES7c() { }
void Z80::RES7d() { }
void Z80::RES7e() { }
void Z80::RES7h() { }
void Z80::RES7l() { }
void Z80::RES7a() { }
void Z80::RES7n() { }

void Z80::SET7b() { }
void Z80::SET7c() { }
void Z80::SET7d() { }
void Z80::SET7e() { }
void Z80::SET7h() { }
void Z80::SET7l() { }
void Z80::SET7a() { }
void Z80::SET7n() { }

void Z80::RLA() { }
void Z80::RLCA() { }
void Z80::RRA() { }
void Z80::RRCA() { }

void Z80::RLr_b() { }
void Z80::RLr_c() { }
void Z80::RLr_d() { }
void Z80::RLr_e() { }
void Z80::RLr_h() { }
void Z80::RLr_l() { }
void Z80::RLr_a() { }
void Z80::RLHL() { }

void Z80::RLCr_b() { }
void Z80::RLCr_c() { }
void Z80::RLCr_d() { }
void Z80::RLCr_e() { }
void Z80::RLCr_h() { }
void Z80::RLCr_l() { }
void Z80::RLCr_a() { }
void Z80::RLCHL() { }

void Z80::RRr_b() { }
void Z80::RRr_c() { }
void Z80::RRr_d() { }
void Z80::RRr_e() { }
void Z80::RRr_h() { }
void Z80::RRr_l() { }
void Z80::RRr_a() { }
void Z80::RRHL() { }

void Z80::RRCr_b() { }
void Z80::RRCr_c() { }
void Z80::RRCr_d() { }
void Z80::RRCr_e() { }
void Z80::RRCr_h() { }
void Z80::RRCr_l() { }
void Z80::RRCr_a() { }
void Z80::RRCHL() { }

void Z80::SLAr_b() { }
void Z80::SLAr_c() { }
void Z80::SLAr_d() { }
void Z80::SLAr_e() { }
void Z80::SLAr_h() { }
void Z80::SLAr_l() { }
void Z80::SLAr_a() { }

void Z80::SLLr_b() { }
void Z80::SLLr_c() { }
void Z80::SLLr_d() { }
void Z80::SLLr_e() { }
void Z80::SLLr_h() { }
void Z80::SLLr_l() { }
void Z80::SLLr_a() { }

void Z80::SRAr_b() { }
void Z80::SRAr_c() { }
void Z80::SRAr_d() { }
void Z80::SRAr_e() { }
void Z80::SRAr_h() { }
void Z80::SRAr_l() { }
void Z80::SRAr_a() { }

void Z80::SRLr_b() { }
void Z80::SRLr_c() { }
void Z80::SRLr_d() { }
void Z80::SRLr_e() { }
void Z80::SRLr_h() { }
void Z80::SRLr_l() { }
void Z80::SRLr_a() { }

void Z80::CPL() { }
void Z80::NEG() { }

void Z80::CCF() { }
void Z80::SCF() { }

/* Operacje stosowe */
void Z80::PUSHBC() { }
void Z80::PUSHDE() { }
void Z80::PUSHHL() { }
void Z80::PUSHAF() { }

void Z80::POPBC() { }
void Z80::POPDE() { }
void Z80::POPHL() { }
void Z80::POPAF() { }

/* Operacje skokowe */
void Z80::JPnn() { }
void Z80::JPHL() { }
void Z80::JPNZnn() { }
void Z80::JPZnn() { }
void Z80::JPNCnn() { }
void Z80::JPCnn() { }

void Z80::JRn() { }
void Z80::JRNZn() { }
void Z80::JRZn() { }
void Z80::JRNCn() { }
void Z80::JRCn() { }

void Z80::DJNZn() { }

void Z80::CALLnn() { }
void Z80::CALLNZnn() { }
void Z80::CALLZnn() { }
void Z80::CALLNCnn() { }
void Z80::CALLCnn() { }

void Z80::RET() { }
void Z80::RETI() { }
void Z80::RETNZ() { }
void Z80::RETZ() { }
void Z80::RETNC() { }
void Z80::RETC() { }

void Z80::RST00() { }
void Z80::RST08() { }
void Z80::RST10() { }
void Z80::RST18() { }
void Z80::RST20() { }
void Z80::RST28() { }
void Z80::RST30() { }
void Z80::RST38() { }
void Z80::RST40() { }
void Z80::RST48() { }
void Z80::RST50() { }
void Z80::RST58() { }
void Z80::RST60() { }

void Z80::NOP() { }
void Z80::HALT() { }

void Z80::DI() { }
void Z80::EI() { }

/* Domniemane puste miejsce */
void Z80::XX() { }

/* Prefiks polecenia dwubajtowego, tu powinno byæ wywo³anie funkcji z tablicy dwubajtowej. */
void Z80::MAPcb() { }
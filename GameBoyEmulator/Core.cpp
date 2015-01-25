#include "GPU.h"
#include "MMU.h"
#include "Z80.h"
#include "Input.h"

int main()
{
	MMU mmu = MMU();
	GPU gpu = GPU();
	Z80 z80 = Z80();
	Input input = Input();

	z80._mmu = &mmu;
	z80._gpu = &gpu;

	mmu._gpu = &gpu;

	gpu._input = &input;
	gpu._mmu = &mmu;

	input.init();
	mmu.init();
	gpu.init();
	z80.init();

	z80.dispatch();

	return 0;
}

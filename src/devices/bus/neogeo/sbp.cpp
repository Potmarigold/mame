// license:BSD-3-Clause
// copyright-holders:S. Smith,David Haywood,Fabio Priuli
/***********************************************************************************************************

 Neo Geo cart emulation
 Super Bubble Pop cart type

 Note: since protection here involves accesses to ROM, we include the scrambling in this
 file rather than in a separate prot_* source

 ***********************************************************************************************************/


#include "emu.h"
#include "sbp.h"

DEFINE_DEVICE_TYPE(NEOGEO_SBP_CART, neogeo_sbp_cart_device, "neocart_sbp", "Neo Geo Super Bubble Pop Cart")

neogeo_sbp_cart_device::neogeo_sbp_cart_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	neogeo_rom_device(mconfig, NEOGEO_SBP_CART, tag, owner, clock)
{
}


void neogeo_sbp_cart_device::device_start()
{
}

void neogeo_sbp_cart_device::device_reset()
{
}



uint16_t neogeo_sbp_cart_device::protection_r(address_space &space, offs_t offset)
{
	uint16_t* rom = (get_rom_size()) ? get_rom_base() : get_region_rom_base();
	uint16_t origdata = rom[offset + (0x200/2)];
	uint16_t data = bitswap<16>(origdata, 11,10,9,8,15,14,13,12,3,2,1,0,7,6,5,4);

	int realoffset = 0x200 + (offset * 2);
	logerror("sbp_lowerrom_r offset %08x data %04x\n", realoffset, data);

	// there is actually data in the rom here already, maybe we should just return it 'as is'
	if (realoffset == 0xd5e)
		return origdata;

	return data;
}


void neogeo_sbp_cart_device::protection_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	int realoffset = 0x200 + (offset * 2);

	// the actual data written is just pulled from the end of the rom, and unused space
	// maybe this is just some kind of watchdog for the protection device and it doesn't
	// matter?
	if (realoffset == 0x1080)
	{
		if (data == 0x4e75)
		{
			return;
		}
		else if (data == 0xffff)
		{
			return;
		}
	}

	printf("sbp_lowerrom_w offset %08x data %04x\n", realoffset, data);
}


void neogeo_sbp_cart_device::patch(uint8_t* cpurom, uint32_t cpurom_size)
{
	uint16_t* rom = (uint16_t*)cpurom;

	// the game code clears the text overlay used ingame immediately after writing it..
	// why? protection? sloppy code that the hw ignores? imperfect emulation?
	rom[0x2a6f8/2] = 0x4e71;
	rom[0x2a6fa/2] = 0x4e71;
	rom[0x2a6fc/2] = 0x4e71;

	// enable joystick inputs
	rom[0x3ff2d/2] = 0x7001;
}

void neogeo_sbp_cart_device::decrypt_all(DECRYPT_ALL_PARAMS)
{
	patch(cpuregion, cpuregion_size);
}

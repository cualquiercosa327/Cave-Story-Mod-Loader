// Graphics enhancement mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch_bute.h"

#include "mod_loader.h"

#include "../common.h"

void PatchBute(void)
{
	ModLoader_WriteLong((void*)0x4637C3 + 2, ((SCREEN_WIDTH / 2) + 128) * 0x200);
	ModLoader_WriteLong((void*)0x4637D7 + 1, ((SCREEN_WIDTH / 2) + 112) * 0x200);
	ModLoader_WriteLong((void*)0x4637FB + 1, ((SCREEN_WIDTH / 2) + 128) * 0x200);
	ModLoader_WriteLong((void*)0x46380E + 2, ((SCREEN_WIDTH / 2) + 112) * 0x200);

	ModLoader_WriteByte((void*)0x461AE1 + 1, (((SCREEN_WIDTH / 2) + 64) / 16));
	ModLoader_WriteByte((void*)0x461AE3 + 1, -(((SCREEN_WIDTH / 2) + 64) / 16));
	ModLoader_WriteByte((void*)0x461AFE + 1, (((SCREEN_WIDTH / 2) + 16) / 16));
	ModLoader_WriteByte((void*)0x461B00 + 1, -(((SCREEN_WIDTH / 2) + 16) / 16));
}

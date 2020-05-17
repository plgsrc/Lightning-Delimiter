#include "global.h"


#ifndef READU8
#	define READU8(addr) 			*(volatile unsigned char*)(addr)
#endif
#ifndef READU16
#	define READU16(addr) 			*(volatile unsigned short*)(addr)
#endif
#ifndef READU32
#	define READU32(addr) 			*(volatile unsigned int*)(addr)
#endif

#ifndef IO_BASE_PAD
#	define IO_BASE_PAD				1
#endif
#ifndef IO_BASE_LCD
#	define IO_BASE_LCD				2
#endif
#ifndef IO_BASE_PDC
#	define IO_BASE_PDC				3
#endif
#ifndef IO_BASE_GSPHEAP
#	define IO_BASE_GSPHEAP			4
#endif
#ifndef BUTTON_A
#	define BUTTON_A 				0x00000001
#endif
#ifndef BUTTON_B
#	define BUTTON_B 				0x00000002
#endif
#ifndef BUTTON_SE
#	define BUTTON_SE 				0x00000004
#endif
#ifndef BUTTON_ST
#	define BUTTON_ST 				0x00000008
#endif
#ifndef BUTTON_DR
#	define BUTTON_DR 				0x00000010
#endif
#ifndef BUTTON_DL
#	define BUTTON_DL 				0x00000020
#endif
#ifndef BUTTON_DU
#	define BUTTON_DU 				0x00000040
#endif
#ifndef BUTTON_DD
#	define BUTTON_DD 				0x00000080
#endif
#ifndef BUTTON_R
#	define BUTTON_R 				0x00000100
#endif
#ifndef BUTTON_L
#	define BUTTON_L 				0x00000200
#endif
#ifndef BUTTON_ZR
#	define BUTTON_ZR 				0x00008000
#endif
#ifndef BUTTON_ZL
#	define BUTTON_ZL 				0x00004000
#endif
#ifndef BUTTON_X
#	define BUTTON_X 				0x00000400
#endif
#ifndef BUTTON_Y
#	define BUTTON_Y 				0x00000800
#endif

#ifndef A
#	define A						BUTTON_A
#endif
#ifndef B
#	define B						BUTTON_B
#endif
#ifndef X
#	define X						BUTTON_X
#endif
#ifndef Y
#	define Y						BUTTON_Y
#endif
#ifndef L
#	define L						BUTTON_L
#endif
#ifndef R
#	define R						BUTTON_R
#endif
#ifndef ZL
#	define ZL						BUTTON_ZL
#endif
#ifndef ZR
#	define ZR						BUTTON_ZR
#endif
#ifndef ST
#	define ST						BUTTON_ST
#endif
#ifndef SE
#	define SE						BUTTON_SE
#endif
#ifndef DU
#	define DU						BUTTON_DU
#endif
#ifndef DD
#	define DD						BUTTON_DD
#endif
#ifndef DL
#	define DL						BUTTON_DL
#endif
#ifndef DR
#	define DR						BUTTON_DR
#endif

#ifndef ADDSUBFLOAT
    #define ADDTOFLOAT(addr, add_value)            *(float *)(addr) += (float)(add_value)
    #define SUBTOFLOAT(addr, sub_value)            *(float *)(addr) -= (float)(sub_value)
#endif

#ifndef READWRITEFLOAT
    #define READFLOAT(addr)                     *(float *)(addr)
    #define WRITEFLOAT(addr, data)                 *(float *)(addr) = data
#endif

#ifndef ADDSUBTOUNSIGNED
    #define ADDTOUNSIGNED(addr, value)            *(u32 *)(addr) += value
    #define SUBTOUNSIGNED(addr, value)            *(u32 *)(addr) -= value
#endif

#define WRITEU8(addr, data) *(vu8*)(addr) = data
#define WRITEU16(addr, data) *(vu16*)(addr) = data
#define WRITEU32(addr, data) *(vu32*)(addr) = data

u32 threadStack[0x1000];
Handle fsUserHandle;
FS_archive sdmcArchive;


#define IO_BASE_PAD		1
#define IO_BASE_LCD		2
#define IO_BASE_PDC		3
#define IO_BASE_GSPHEAP		4

u32 offset = 0;
u32 data = 0;

u32 IoBasePad = 0xFFFD4000;

u32 getKey() {
	return (*(vu32*)(IoBasePad) ^ 0xFFF) & 0xFFF;
}

void waitKeyUp() {
	while (getKey() != 0) {
		svc_sleepThread(100000000);
	}
}

u8 cheatEnabled[64];
int isNewNtr = 0;


u32 plgGetIoBase(u32 IoType);
GAME_PLUGIN_MENU gamePluginMenu;

void initMenu() {
	memset(&gamePluginMenu, 0, sizeof(GAME_PLUGIN_MENU));
}

void addMenuEntry(u8* str) {
	if (gamePluginMenu.count > 64) {
		return;
	}
	u32 pos = gamePluginMenu.count;
	u32 len = strlen(str) + 1;
	gamePluginMenu.offsetInBuffer[pos] = gamePluginMenu.bufOffset;
	strcpy(&(gamePluginMenu.buf[gamePluginMenu.bufOffset]), str);
	
	gamePluginMenu.count += 1;
	gamePluginMenu.bufOffset += len;
}

u32 updateMenu() {
	PLGLOADER_INFO *plgLoaderInfo = (void*)0x07000000;
	plgLoaderInfo->gamePluginPid = getCurrentProcessId();
	plgLoaderInfo->gamePluginMenuAddr = (u32)&gamePluginMenu;
	
	u32 ret = 0;
	u32 hProcess;
	u32 homeMenuPid = plgGetIoBase(5);
	if (homeMenuPid == 0) {
		return 1;
	}
	ret = svc_openProcess(&hProcess, homeMenuPid);
	if (ret != 0) {
		return ret;
	}
	copyRemoteMemory( hProcess, &(plgLoaderInfo->gamePluginPid), CURRENT_PROCESS_HANDLE,  &(plgLoaderInfo->gamePluginPid), 8);
	final:
	svc_closeHandle(hProcess);
	return ret;
}

int scanMenu() {
	u32 i;
	for (i = 0; i < gamePluginMenu.count; i++) {
		if (gamePluginMenu.state[i]) {
			gamePluginMenu.state[i] = 0;
			return i;
		}
	}
	return -1;
}

// detect language (0: english)
int detectLanguage() {
	// unimplemented
	return 0;
}

// add one cheat menu entry
void addCheatMenuEntry(u8* str) {
	u8 buf[100];
	xsprintf(buf, "[ ] %s", str);
	addMenuEntry(buf);
}

// this function will be called when the state of cheat item changed
void onCheatItemChanged(int id, int enable) {
	// TODO: handle on cheat item is select or unselected
	

}

void LightningDelimiter() 
{
    if (cheatEnabled[0])
	{
        u32 key = getKey();
		
        if (key == R + DD)
		{
			WRITEU32(0x2BA648, 0xE3A03001);
            WRITEU32(0x2BA69C, 0xE3A03001);
            WRITEU32(0x2BA6A4, 0xE3A0100A);
            WRITEU32(0x2BA678, 0xE5900038);
			
			if (READU8(0x147909F4) == 1)
			{
                WRITEU32(0x653FFC, 1);
			}
        }
		if (READU8(0x147909F4) == 1)
		{
            if (READU16(0x653FFC) == 1)
			{
                u32 pointer = READU32(READU32(0x14000074) - 0x1B5C);
				
                if (READU32(pointer) > 0x16400000 && (READU32(pointer) < 0x17800000))
				{
                    pointer = READU32(pointer + 0x27AC);
					
                    if (READU16(pointer + 0x40) == 0x20A)
					{
						WRITEU16(pointer + 0x40, 0xFF);
					}
				}
            }
        }
        if (key == L + DL)
		{
			WRITEU32(0x2BA648, 0xE3A03007);
            WRITEU32(0x2BA69C, 0xE3A03007);
            WRITEU32(0x2BA6A4, 0xE3A0100E);
            WRITEU32(0x2BA678, 0xE5900058);
			
			if (READU8(0x147909F4) == 1)
			{
                WRITEU32(0x653FFC, 0);
				
                u32 pointer = READU32(READU32(0x14000074) - 0x1B5C);
				
                if (READU32(pointer) > 0x16400000 && (READU32(pointer) < 0x17800000))
				{
                    pointer = READU32(pointer + 0x27AC);
					
                    if (READU16(pointer + 0x40) == 0x20A)
					{
                        WRITEU16(pointer + 0x40, 0x20E);
					}
				}
            }
        }        
        if (key == L + DL + A)
		{
			WRITEU32(0x2BA648, 0xE3A03007);
            WRITEU32(0x2BA69C, 0xE3A03007);
            WRITEU32(0x2BA6A4, 0xE3A0100E);
            WRITEU32(0x2BA678, 0xE5900058);
			
			if (READU8(0x147909F4) == 1)
			{
                WRITEU32(0x653FFC, 0);
				
                u32 pointer = READU32(READU32(0x14000074) - 0x1B5C);
				
                if (READU32(pointer) > 0x16400000 && (READU32(pointer) < 0x17800000))
				{
                    pointer = READU32(pointer + 0x27AC);
					
                    if (READU16(pointer + 0x40) == 0x20A)
					{
                        WRITEU16(pointer + 0x40, 0x20E);
					}
				}
            }
        }        
        if (key == L + DL + B)
		{
			WRITEU32(0x2BA648, 0xE3A03007);
            WRITEU32(0x2BA69C, 0xE3A03007);
            WRITEU32(0x2BA6A4, 0xE3A0100E);
            WRITEU32(0x2BA678, 0xE5900058);
			
			if (READU8(0x147909F4) == 1)
			{
                WRITEU32(0x653FFC, 0);
				
                u32 pointer = READU32(READU32(0x14000074) - 0x1B5C);
				
                if (READU32(pointer) > 0x16400000 && (READU32(pointer) < 0x17800000))
				{
                    pointer = READU32(pointer + 0x27AC);
					
                    if (READU16(pointer + 0x40) == 0x20A)
					{
                        WRITEU16(pointer + 0x40, 0x20E);
					}
				}
            }
        }        
        if (key == L + DL + ST)
		{
			WRITEU32(0x2BA648, 0xE3A03007);
            WRITEU32(0x2BA69C, 0xE3A03007);
            WRITEU32(0x2BA6A4, 0xE3A0100E);
            WRITEU32(0x2BA678, 0xE5900058);
			
			if (READU8(0x147909F4) == 1)
			{
                WRITEU32(0x653FFC, 0);
				
                u32 pointer = READU32(READU32(0x14000074) - 0x1B5C);
				
                if (READU32(pointer) > 0x16400000 && (READU32(pointer) < 0x17800000))
				{
                    pointer = READU32(pointer + 0x27AC);
					
                    if (READU16(pointer + 0x40) == 0x20A)
					{
                        WRITEU16(pointer + 0x40, 0x20E);
					}
				}
            }
        }        
	} 
}

// update the menu status
void updateCheatEnableDisplay(id) {
	gamePluginMenu.buf[gamePluginMenu.offsetInBuffer[id] + 1] = cheatEnabled[id] ? 'X' : ' ';
}

// scan and handle events
void scanCheatMenu() {
	int ret = scanMenu();
	if (ret != -1) {
		cheatEnabled[ret] = !cheatEnabled[ret];
		updateCheatEnableDisplay(ret);
		onCheatItemChanged(ret, cheatEnabled[ret]);
	}
}

// init 
void initCheatMenu() {
	initMenu();
	addCheatMenuEntry("Lucky 7 is Lightning Delimiter");
    addMenuEntry("  (R+DD=ON L+DL=OFF)");

	updateMenu();
}

void gamePluginEntry() {
	u32 ret, key;
	INIT_SHARED_FUNC(plgGetIoBase, 8);
	INIT_SHARED_FUNC(copyRemoteMemory, 9);
	// wait for game starts up (5 seconds)
	svc_sleepThread(5000000000);

	if (((NS_CONFIG*)(NS_CONFIGURE_ADDR))->sharedFunc[8]) {
		isNewNtr = 1;
	} else {
		isNewNtr = 0;
	}
	
	if (isNewNtr) {
		IoBasePad = plgGetIoBase(IO_BASE_PAD);
	}
	initCheatMenu();
	while (1) {
		svc_sleepThread(100000000);
		scanCheatMenu();
		LightningDelimiter();
	}
}
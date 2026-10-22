// LevelTest.cpp : Defines the exported functions for the DLL application.
//
// Emerald Coast replaces City Escape (level 13). With the SA2 2P mod
// (SA2_1P_in_2P) installed it registers as its own 2P stage instead: its
// patches and file replacements are applied only while an Emerald Coast match
// is played (Apply / Revert below), so City Escape stays City Escape
// everywhere else. With 2P stage API 3 it also gets its own level ID (96): it
// still runs City Escape's level code (patched), but its set and camera files
// and its positions belong to level 96. Without the 2P mod it applies
// everything permanently to level 13 on the first frame, as before.

#include "stdafx.h"
#include "LevelTest.h"
#include "LandTableInfo.h"
#include "ModelInfo.h"
#include "sa2_2p_api.h"
#include <string>
#include <vector>

NJS_OBJECT *stru_103B270;
NJS_OBJECT *stru_103B37C;

NJS_TEXNAME beach01_texname[258];

NJS_TEXLIST beach01_texlist = { arrayptrandlengthT(beach01_texname, Uint32) };

NJS_TEXNAME BG_BEACH_TEXNAMES[3];

NJS_TEXLIST BG_BEACH_TEXLIST = { arrayptrandlengthT(BG_BEACH_TEXNAMES, Uint32) };

NJS_TEXNAME BEACH_SEA_TEXNAMES[17];

NJS_TEXLIST BEACH_SEA_TEXLIST = { arrayptrandlengthT(BEACH_SEA_TEXNAMES, Uint32)  };

TexPackInfo bgtexpacks[] = {
	{ "BG_BEACH", &BG_BEACH_TEXLIST },
	{ "BEACH_SEA", &BEACH_SEA_TEXLIST },
	{ 0 }
};

NJS_TEXLIST **copylist = { nullptr };

void LandManagerHook(int a1, LandTable *a2)
{
	LandTableSA2BModels = 0;
	LoadLandManager(a2);
}

void __cdecl MusicMan_Main(ObjectMaster *obj)
{
	if (!MainCharObj1[0]) return;
	switch (obj->Data1.Entity->Action)
	{
	case 0:
		if (MainCharObj1[0]->Position.z < -1000 || (MainCharObj1[1] && MainCharObj1[1]->Position.z < -1000))
		{
			obj->Data1.Entity->Action = 1;
			StopMusic();
			PlayMusic("ecoast2.adx");
			ResetMusic();
		}
		else if (MainCharObj1[0]->Position.z > 2000 || (MainCharObj1[1] && MainCharObj1[1]->Position.z > 2000))
		{
			obj->Data1.Entity->Action = 3;
			StopMusic();
			PlayMusic("ecoast3.adx");
			ResetMusic();
		}
		break;
	case 1:
		if (MainCharObj1[0]->Position.z > -1000 && (!MainCharObj1[1] || MainCharObj1[1]->Position.z > -1000))
		{
			obj->Data1.Entity->Action = 0;
			StopMusic();
			PlayMusic("ecoast1.adx");
			ResetMusic();
		}
		break;
	case 3:
		if (MainCharObj1[0]->Position.z < -1000 || (MainCharObj1[1] && MainCharObj1[1]->Position.z < -1000))
		{
			obj->Data1.Entity->Action = 1;
			StopMusic();
			PlayMusic("ecoast2.adx");
			ResetMusic();
		}
		else if (MainCharObj1[0]->Position.z < 1900 && (!MainCharObj1[1] || MainCharObj1[1]->Position.z < 1900))
		{
			obj->Data1.Entity->Action = 0;
			StopMusic();
			PlayMusic("ecoast1.adx");
			ResetMusic();
		}
		break;
	}
}

void __cdecl TeleportMan_Main(ObjectMaster *obj)
{
	for (int i = 0; i < 2; i++)
	{
		if (MainCharObj1[i])
		{
			float v9 = (-986.5f - MainCharObj1[i]->Position.x) * (-986.5f - MainCharObj1[i]->Position.x)
				+ (1145 - MainCharObj1[i]->Position.y) * (1145 - MainCharObj1[i]->Position.y)
				+ (-4856 - MainCharObj1[i]->Position.z) * (-4856 - MainCharObj1[i]->Position.z);
			if (sqrt(v9) < 22 && MainCharObj1[i]->Action == 58)
				MainCharObj1[i]->Action = 0;
			v9 = (5746 - MainCharObj1[i]->Position.x) * (5746 - MainCharObj1[i]->Position.x)
				+ (406 - MainCharObj1[i]->Position.y) * (406 - MainCharObj1[i]->Position.y)
				+ (655 - MainCharObj1[i]->Position.z) * (655 - MainCharObj1[i]->Position.z);
			if (sqrt(v9) < 22)
			{
				MainCharObj1[i]->Position.x = -986.5f;
				MainCharObj1[i]->Position.y = 1145;
				MainCharObj1[i]->Position.z = -4856;
				MainCharObj1[i]->Rotation.y = 0x4000;
				MainCharObj1[i]->Action = 58;
			}
		}
	}
}

struct PalmtreeData
{
	char id;
	NJS_VECTOR pos;
	int xrot;
	int yrot;
	int zrot;
};

PalmtreeData palmtreearray[] = {
	{ 0, { 48, 0.5f, 44 }, 0x240, 0x1053, 0xFF0C },
	{ 0, { -77, 2, 154 }, 0xFC56, 0xFE5D, 0xFFDA },
	{ 0, { -498.55f, 2.5f, 255.84f }, 0x36F, 0x25CE, 0xFB71 },
	{ 0, { -474, 0.5f, 361 }, 0x256, 0x3132, 0xF9E9 },
	{ 0, { -656, 0, 394 }, 0xEC, 0x303C, 0xFDBD },
	{ 0, { -691, 1, 637 }, 0x56D, 0xDBB4, 0x6A2 },
	{ 0, { -579, 0.5f, 612 }, 0xFFDE, 0x4150, 0xFBEE },
	{ 0, { -27, 2.5f, 74 }, 0xFF11, 0x9C98, 0xCA },
	{ 0, { 5532.64f, 0.86f, 1127.39f }, 0x35E, 0xDBFA, 0x417 },
	{ 0, { 5885, -1, 1112 }, 0xFC66, 0x61A3, 0xFCB0 },
	{ 0, { 6181, -1, 1027 }, 0x55E, 0x9C0F, 0xFB9C },
	{ 0, { 6096, -3, 966 }, 0xFEFE, 0xB4AB, 0x387 },
	{ 0, { 5959, -0.5f, 1211 }, 0x35E, 0xDBFA, 0x417 },
	{ 0, { 6084, -1.5f, 1068 }, 0xFC64, 0x93B5, 0x1E4 },
	{ 2, { 2215.46f, 95.7f, 263.95f }, 0xE18D, 0xA033, 0x260D },
	{ 0, { 254, 3, 1012 }, 0xFEF1, 0x3085, 0x2A6 },
	{ 0, { -1041, 0, 1235 }, 0xFC5C, 0xB163, 0x986 },
	{ 0, { 459, 0.5f, 547 }, 0x447, 0x9D99, 0xFC38 },
	{ 0, { -737, -0.5f, 1443 }, 0xFEF5, 0xBAD7, 0x815 },
	{ 0, { -221.55f, 1, 213.84f }, 0x2C3, 0xA36, 0xFF4B },
	{ 0, { 284, 1, 843 }, 0xDB, 0x3BFC, 0xF77C },
	{ 0, { 126, 1, 889 }, 0x348, 0xAAAA, 0xFA5C },
	{ 0, { -881, 3, 1294 }, 0xFDDF, 0x5CF6, 0xFD88 },
	{ 0, { -1009, 1, 1372 }, 0x673, 0xDB84, 0x7EB },
	{ 0, { 171, 3, 1146 }, 0x3EE, 0x147A, 0xFDD8 },
	{ 1, { 213, 3.5f, 1085 }, 0x289, 0x2FDB, 0xF9FD },
	{ 1, { 362, 1.5f, 629 }, 0xF9D9, 0x2180, 0x68B },
	{ 0, { -355.55f, 2.5f, 196.84f }, 0x5C8, 0xD96, 0xFE02 },
	{ 1, { 576, 2, 519 }, 0xF97B, 0, 0 },
	{ 2, { -1075.47f, -2.5f, 1274.45f }, 0xF8B5, 0xBD1C, 0x3085 },
	{ 2, { 6039.53f, -6.5f, 909.45f }, 0xFE1D, 0xBE7F, 0x2485 },
	{ 2, { 5880.03f, -3, 1184.45f }, 0x341C, 0xA01, 0xF667 },
	{ 2, { -116.47f, 0, 214.45f }, 0x2F2E, 0x151C, 0xEC65 },
	{ 2, { 256.53f, -1.5f, 735.45f }, 0xE994, 0xB5CE, 0x2D7A },
	{ 2, { 303.53f, -1.5f, 1025.45f }, 0x131, 0x3F9A, 0xCD31 },
	{ 1, { -52, 2, -19 }, 0x3D9, 0x52C3, 0xFFD4 },
	{ 2, { 213.53f, -3, 1172.45f }, 0x19C1, 0x2E37, 0xDB3C },
	{ 2, { -1006.47f, -2.5f, 1489.45f }, 0xACF, 0xC66B, 0x29CF },
	{ 2, { -594.47f, -2, 1573.45f }, 0x6A2, 0x3AF8, 0xDAA2 },
	{ 2, { 6026.53f, -3, 1246.45f }, 0x27B3, 0xE96D, 0x1346 },
	{ 2, { -536.47f, -3.5f, 529.45f }, 0xFD8F, 0x42B2, 0xE20B },
	{ 2, { -707.47f, -3.5f, 569.45f }, 0x234, 0xC121, 0x2CAC },
	{ 2, { 6025.53f, -3, 1036.45f }, 0xE8D9, 0xB7CB, 0x313A },
	{ 2, { 404.53f, -1.5f, 652.45f }, 0x1BE8, 0x3250, 0xD48D },
	{ 2, { 37.53f, -2, 996.45f }, 0xEEAB, 0xB8BA, 0x2F34 },
	{ 2, { 6181.53f, 312.5f, 1281.45f }, 0x18E7, 0xDEB5, 0x165C },
	{ 2, { 6092.53f, 156.5f, 1252.45f }, 0x2073, 0xFDAB, 0x1AB },
	{ 0, { 570, 1, 615 }, 0x6A7, 0x745E, 0x1F1 },
	{ 1, { 218, 2, 812 }, 0xF993, 0x2AD5, 0xAEB },
	{ 0, { 6036.48f, 55.41f, 1162.69f }, 0xF4D6, 0x9900, 0xB8 },
	{ 1, { 6334, 402.5f, 1221 }, 0x57B, 0xA823, 0xF7E0 },
	{ 1, { 6317, 403.5f, 1044 }, 0xFB3B, 0xDCA2, 0xFA6B },
	{ 1, { 5771, -0.4f, 1119 }, 0xFC16, 0xAE00, 0x400 },
	{ 1, { 6356, 403, 1144 }, 0x2FB, 0xB39D, 0xF6AE },
	{ 1, { 6351, 403, 1186 }, 0x148, 0x31B4, 0xFE68 },
	{ 2, { 6134.05f, 154.65f, 1145.02f }, 0x9A23, 0x297D, 0xAA3C },
	{ 2, { 5598.53f, -3, 1053.45f }, 0xDBC5, 0x6045, 0xE55B },
	{ 0, { -401.87f, 83.86f, 1250.29f }, 0x300, 0xEE79, 0xF7C5 },
	{ 2, { -576.47f, -3, 723.45f }, 0x11D0, 0x34DE, 0xD7D2 },
	{ 2, { -816.94f, -1.82f, 1285.73f }, 0xF8B8, 0x477E, 0xCF47 },
	{ 2, { -422.6f, 84.07f, 1174.35f }, 0xE520, 0x69AE, 0xF2B0 },
	{ 2, { 652.18f, 3.92f, 515.15f }, 0xD649, 0x8363, 0xCFC },
	{ 0, { 697.83f, 11.84f, 734.11f }, 0x5D9, 0xE363, 0x672 },
	{ 1, { 712.69f, 78.4f, 520.83f }, 0xF618, 0x3863, 0xFC0E },
	{ 2, { 779.11f, 30.42f, 948.69f }, 0x1343, 0xD263, 0x1C71 },
	{ 1, { 939.95f, 55.67f, 986.29f }, 0x738, 0xDDA3, 0x696 },
	{ 1, { 888.42f, 78.92f, 671.05f }, 0xFAF4, 0x57B3, 0xFF4F },
	{ 0, { 1019.36f, 75.46f, 780.15f }, 0xFB5B, 0x7103, 0xFD9F },
	{ 0, { 963.45f, 77.67f, 702.65f }, 0xFFEF, 0x5F03, 0xFE1A },
	{ 0, { 1613.75f, 114.3f, 884.82f }, 0x400, 0x533, 0x200 },
	{ 1, { 799.6f, 79.99f, 646.85f }, 0xFEB1, 0xA003, 0xBAD },
	{ 2, { 1910.95f, 194.5f, 998.94f }, 0x2C00, 0xDE33, 0x2000 },
	{ 1, { 1920.77f, 197.4f, 937.2f }, 0xB00, 0x4033, 0 },
	{ 2, { 1821.35f, 195.13f, 888.83f }, 0xFF00, 0xC433, 0x2700 },
	{ 0, { 5715.05f, -22.1f, 1159.4f }, 0x41C, 0xF500, 0x3C7 },
	{ 0, { 6146.66f, 1.63f, 1250.92f }, 0xCCC, 0x9500, 0xFFF2 },
	{ 2, { 5825.63f, 417.25f, 636.91f }, 0xFA3, 0x2F7D, 0xE33C },
	{ 2, { 6274.69f, 405.75f, 1073.41f }, 0xA523, 0x297D, 0x9A3C },
	{ 0, { 2116.63f, 6.88f, 235.61f }, 0xF400, 0x7D24, 0x143 },
	{ 1, { -319, 3, 107 }, 0x4E4, 0, 0 },
	{ 0, { 1919.04f, 41.4f, 365.9f }, 0xC3, 0x4808, 0xFE9B },
	{ 0, { -791, 0.5f, 1373 }, 0xFA6E, 0x5C94, 0xF97C },
	{ 0, { -859.17f, 2.51f, 1405.76f }, 0xFA6E, 0xFF04, 0xF97C },
	{ 2, { 824.76f, 74.32f, 578.48f }, 0xE7EF, 0x5503, 0xE91A },
	{ 1, { 6347, 403, 1087 }, 0x6D, 0x9C90, 0xFFD5 },
	{ 2, { 738, 77.97f, 548.18f }, 0xBE2, 0xCC63, 0x28EF },
	{ 1, { -524, 0.5f, 459 }, 0x526, 0xABB7, 0xF6B8 },
	{ 1, { -455, 3, 185 }, 0xFD3A, 0xA194, 0x2FD },
	{ 2, { 1024, 66.9f, 934.35f }, 0x27E4, 0x503, 0x5DB },
	{ 0, { 337.9f, -1.5f, 724.77f }, 0xFAC1, 0xA6D6, 0x74A },
	{ 1, { -684, 1, 506 }, 0xFE00, 0x5968, 0x284 },
	{ 1, { 1562.89f, 115, 783.85f }, 0xFF00, 0x8403, 0 },
	{ 3, { 2121.25f, 114, 952.54f }, 0x760, 0x1593, 0xFC60 },
	{ 0, { 1930.79f, 197.51f, 1001.44f }, 0xF900, 0x533, 0xF700 },
	{ 0, { 1855.47f, 197.7f, 949.43f }, 0xF9E0, 0xF483, 0xF460 },
	{ 1, { 6064.88f, 159, 1233.99f }, 0xFDA9, 0x1BC4, 0x4C4 },
	{ 1, { 6253.39f, 314, 1255 }, 0x452, 0xAA26, 0xFB1E },
	{ 0, { 5739.66f, 451.49f, 694.74f }, 0xD10, 0xF1F4, 0xF8B0 },
	{ 2, { 5782.96f, 427.35f, 681.54f }, 0x31A3, 0x1D7D, 0xDE3C },
	{ 3, { 5854.93f, 464.39f, 581.48f }, 0x610, 0x23F4, 0xF8B0 },
	{ 3, { 5853.08f, 454.39f, 553.73f }, 0xB10, 0x3EF4, 0xF4B0 },
	{ 3, { 5923.12f, 420.39f, 475.52f }, 0xED10, 0x30F4, 0xFEB0 },
	{ 3, { 5693.02f, 403.49f, 722.33f }, 0x1220, 0x1594, 0x90 },
	{ 3, { 6182.06f, 456.09f, 1174.17f }, 0x720, 0xD974, 0x890 },
	{ 3, { 6208.69f, 208.69f, 1254.82f }, 0x3220, 0xA874, 0xF090 },
	{ 2, { 834.94f, 78.17f, 721.35f }, 0x17B2, 0xCE03, 0x3193 },
	{ 0, { 739.7f, 23.12f, 833.82f }, 0xFA3E, 0xF903, 0x619 },
	{ 0, { 861.37f, 42.12f, 1017.96f }, 0x13E, 0x4703, 0x619 },
	{ 0, { 897.21f, 80.25f, 795.24f }, 0xB68, 0xF803, 0xFEF7 },
	{ 0, { -296.55f, 55.31f, 1179.01f }, 0xFC00, 0x6379, 0xFB00 },
	{ 1, { -291.37f, 1.68f, 380.92f }, 0x596, 0xB10E, 0xFD65 },
	{ 0, { 126, 3, 1024 }, 0x40, 0x3C70, 0xFD29 },
	{ 0, { -190, 1, 57 }, 0x71D, 0x9F37, 0xF941 },
	{ 2, { 1890.65f, 173.4f, 561.02f }, 0xF9C3, 0x4808, 0xD99B },
	{ 0, { 1985.68f, 7.74f, 247.13f }, 0xFE41, 0x87D8, 0xFD3E },
	{ 0, { 1861.08f, 104.74f, 460.77f }, 0xFD91, 0xC108, 0xFA3E },
	{ 2, { 2084.18f, 4.4f, 262.01f }, 0x2600, 0x433, 0x1200 },
	{ 0, { 5965.66f, 7.15f, 1127.27f }, 0xF4A3, 0x37D, 0xFF3C },
	{ 1, { -605, 3, 342 }, 0xFC64, 0x3423, 0xBB2 },
	{ 1, { -254.02f, 55.33f, 1232.02f }, 0x7A0, 0xFF3, 0x23B },
	{ 2, { 33.53f, -3, 151.45f }, 0x11D0, 0x34DE, 0xD7D2 },
	{ 0, { 1909.01f, 197.6f, 878.76f }, 0x900, 0x6C33, 0xB70 },
	{ 0, { 1637.49f, 114.1f, 794.01f }, 0xFD00, 0x5193, 0xFA00 },
	{ 1, { 6169.85f, -97.7f, 2396.22f }, 0xFC11, 0x7EBE, 0xA05 },
	{ 0, { 1107.81f, 544, -2917.96f }, 0x300, 0x41EE, 0xD00 },
	{ 0, { 3659.48f, 508, -4833.1f }, 0, 0x9BD8, 0 },
	{ 0, { 1417.62f, 545, -3102.51f }, 0x400, 0x32A2, 0xA00 },
	{ 0, { 2990.68f, 363.51f, -4737.28f }, 0xF25B, 0x1E0, 0x699 },
	{ 2, { 1075.12f, 646.86f, -2793.48f }, 0xCCDA, 0x88DE, 0x1511 },
	{ 0, { 3328.73f, 4, -3375.37f }, 0x300, 0x4C4A, 0xD00 },
	{ 1, { 5087.59f, 22.25f, -4686.49f }, 0xF64D, 0xD271, 0xFF38 },
	{ 2, { 2678.69f, 295.34f, -4642.38f }, 0x1991, 0xD041, 0x2700 },
	{ 1, { 7140.82f, -0.22f, -4234.85f }, 0xFB00, 0x2B7, 0 },
	{ 0, { 4838.09f, 138.98f, -4815.11f }, 0x33E, 0x6166, 0x106F },
	{ 0, { 3245.75f, -1.97f, -4722.31f }, 0x600, 0xA132, 0 },
	{ 2, { 3477.15f, 216.05f, -4519.52f }, 0xDCF3, 0xAE22, 0x1DF5 },
	{ 0, { 3611.04f, 137.05f, -4597.03f }, 0xFAF3, 0xC132, 0x3F5 },
	{ 0, { 2667.11f, 329.84f, -4854.22f }, 0xE9D, 0x34C0, 0x52A },
	{ 2, { 3268.52f, -2.77f, -4582.03f }, 0x2CE0, 0x332, 0 },
	{ 0, { 2739.46f, 341.72f, -4896 }, 0x16, 0xA4C0, 0x1158 },
	{ 2, { 4570.93f, 219.22f, -4870.15f }, 0xD7C3, 0x8CFF, 0xF3 },
	{ 2, { 4796.06f, 162.53f, -4925.56f }, 0x2C95, 0xEC66, 0xD26 },
	{ 3, { 3678.73f, 250.38f, -4658.79f }, 0xC8C, 0xEE56, 0xD29 },
	{ 3, { 493.36f, 806.72f, -3013.03f }, 0x199, 0x8594, 0x1B95 },
	{ 0, { 4851.56f, 117.28f, -4697.98f }, 0x3A7, 0xD866, 0xFEBD },
	{ 3, { 3705.61f, 267.93f, -4461.15f }, 0xF9CA, 0x8856, 0xDD89 },
	{ 0, { 2855.39f, 296.97f, -4207.52f }, 0x8F6, 0xB484, 0x21C },
	{ 0, { 3177.69f, 4, -3516.91f }, 0xF800, 0xCD4A, 0xF300 },
	{ 2, { 3253.99f, 0.9f, -3613.68f }, 0x6E00, 0x4C4A, 0xACFF },
	{ 2, { 2989.77f, 1.27f, -3473.21f }, 0xDB4, 0xC91B, 0x2ACF },
	{ 0, { 2926.4f, 0, -4012.88f }, 0, 0xCD4A, 0 },
	{ 2, { 3027.57f, -2.5f, -4091.79f }, 0xEE00, 0x434A, 0xCC00 },
	{ 1, { 3045.51f, -1.8f, -3973.59f }, 0xFF00, 0x464A, 0xFB00 },
	{ 2, { 3270.51f, 1.1f, -3421.95f }, 0xF000, 0x4D4A, 0xE100 },
	{ 1, { 2911.62f, 292.37f, -4241.3f }, 0xFAF6, 0xFB84, 0xFC1C },
	{ 1, { 3047.84f, 8.46f, -3480.08f }, 0xF21E, 0x9C4A, 0x51B },
	{ 0, { 2707.05f, 274.54f, -4440.66f }, 0x150, 0xD0A2, 0x700 },
	{ 2, { 4273.28f, 404.7f, -4823.36f }, 0xEC00, 0x3C71, 0xD9ED },
	{ 2, { 4436.48f, 254.4f, -4757.26f }, 0x1C00, 0x2671, 0xD9ED },
	{ 0, { 5154.33f, 9.46f, -4638.68f }, 0x879, 0xE471, 0xFF5F },
	{ 2, { 5005.75f, 44.46f, -4623.32f }, 0x245F, 0x151, 0xFF95 },
	{ 1, { 4901.45f, 91.64f, -4637.57f }, 0x9B2, 0x3066, 0xAB },
	{ 2, { 3724.08f, 493, -4867.84f }, 0xD700, 0xA400, 0x1700 },
	{ 0, { 4812.58f, 170.16f, -4981.89f }, 0xE70, 0x7BFF, 0xFD42 },
	{ 0, { 4677.79f, 203.98f, -4909.23f }, 0xFE07, 0x79FF, 0x1B8 },
	{ 0, { 4159.41f, 490.89f, -4758.15f }, 0x813, 0xDAFF, 0xFFF8 },
	{ 1, { 6111.41f, -1.24f, -4417.69f }, 0x678, 0xFDB7, 0xB2 },
	{ 0, { 7046.06f, 0, -4284.59f }, 0x920, 0x87B7, 0x500 },
	{ 0, { 4943.92f, 83.56f, -4702.41f }, 0xF510, 0xF951, 0xF791 },
	{ 2, { 5978.46f, -2.2f, -4712.02f }, 0xD400, 0x91B7, 0xD00 },
	{ 1, { 3402, 70.01f, -4753.68f }, 0, 0x6EAF, 0x300 },
	{ 0, { 5844.92f, -0.26f, -4554 }, 0x2F5, 0xAB7, 0x25D },
	{ 0, { 3864.38f, 586, -4907.75f }, 0, 0x9BD8, 0 },
	{ 0, { 3283.62f, 69, -4755.02f }, 0x700, 0x332, 0 },
	{ 0, { 1169.32f, 544, -2895.05f }, 0x800, 0x71EE, 0x700 },
	{ 2, { 4887.1f, 131.87f, -4843.12f }, 0xFDD8, 0x4266, 0xCFBD },
	{ 0, { 2894.09f, 334.15f, -4883.86f }, 0xD2, 0xBD00, 0xF2C9 },
	{ 0, { -614.68f, 900.64f, -4144.61f }, 0xF6D0, 0x9BD2, 0xFF60 },
	{ 0, { 3461.53f, 70.01f, -4576.03f }, 0, 0x80AF, 0xFF00 },
	{ 2, { 3321.29f, 67, -4606.48f }, 0x1D00, 0xD132, 0x2700 },
	{ 2, { 3062.27f, -3, -4576.87f }, 0, 0xC132, 0x2800 },
	{ 0, { 3705.42f, 139, -4536.82f }, 0x600, 0xF532, 0 },
	{ 3, { -736.27f, 960.69f, -4148.78f }, 0x53E, 0x635A, 0x10D },
	{ 3, { -968.92f, 980.98f, -4401.78f }, 0xF1E1, 0x17DD, 0xF8D8 },
	{ 0, { 1065.1f, 648, -2772.33f }, 0xFD00, 0xA9DE, 0x800 },
	{ 1, { 1073.01f, 647.55f, -2710.54f }, 0xFFC7, 0xD9DE, 0x835 },
	{ 1, { 1098.56f, 545, -3113.95f }, 0xFB00, 0x94CE, 0xF300 },
	{ 0, { 1102.43f, 544, -3014.65f }, 0x700, 0xCFEE, 0xEF00 },
	{ 0, { 1100.65f, 544.8f, -3164.93f }, 0, 0xB6CE, 0xF300 },
	{ 0, { 1357.82f, 545, -3173.63f }, 0x800, 0x7DA2, 0x530 },
	{ 0, { 1303.71f, 545, -3224.32f }, 0xC00, 0x14A2, 0 },
	{ 0, { 1125.44f, 544, -2851.86f }, 0x800, 0x41EE, 0x700 },
	{ 1, { 1304.34f, 543.9f, -3116.65f }, 0x200, 0x24A2, 0xEF20 },
	{ 0, { 1418.99f, 545, -2912 }, 0xFA00, 0xD7A2, 0xF200 },
	{ 0, { 1530.12f, 545, -2897.81f }, 0xED00, 0xEAA2, 0xFD00 },
	{ 1, { 1517.48f, 544.5f, -3020.67f }, 0xD00, 0x12A2, 0xFE00 },
	{ 0, { 6117.6f, -0.26f, -4607.89f }, 0xFA15, 0x3B7, 0xF95D },
	{ 2, { 3002.09f, 379.31f, -4678.05f }, 0x800, 0x4B00, 0xD100 },
	{ 0, { 3003.36f, 381.76f, -4699.62f }, 0xFD6B, 0x3500, 0xFC1D },
	{ 0, { 2904.22f, 381.76f, -4699.12f }, 0x27B, 0x4B00, 0xF21D },
	{ 0, { 4641.38f, 209.99f, -4821.61f }, 0xF1C9, 0xCFF, 0xFDF7 },
	{ 0, { 4088.05f, 495.7f, -4908.12f }, 0x300, 0xEA71, 0xFE00 },
	{ 0, { 7018.55f, -1, -4139.89f }, 0xA20, 0x85B7, 0 },
	{ 2, { 6803.59f, -2.2f, -4261.84f }, 0xD800, 0xA0B7, 0x1C00 },
	{ 0, { 7027.9f, 0, -4359.79f }, 0xFB00, 0x2B7, 0 },
	{ 2, { 5612.35f, -2.2f, -4604.17f }, 0x2E00, 0x3B7, 0 },
	{ 1, { 6844.31f, -0.24f, -4179.85f }, 0xFE78, 0x2B7, 0xB2 },
	{ 0, { 5714.9f, 1.54f, -4710.92f }, 0xFBF5, 0x3B7, 0x5D },
	{ 3, { 217.38f, 654.35f, -3317.55f }, 0xF708, 0x42AF, 0xF4CC },
	{ 0, { 2997, 4.46f, -3515.08f }, 0x28A, 0x9C4A, 0x880 },
	{ 2, { 262.64f, 649.61f, -3292.2f }, 0xF8F7, 0x39AF, 0xC6A7 },
	{ 0, { 387.42f, 580.25f, -3164.84f }, 0xA5, 0x38AF, 0x99B }
};

NJS_OBJECT *palmtreemodels[4];

void __cdecl PalmtreeMan_Display(ObjectMaster *obj)
{
	RenderInfo->CurrentTexlist = &beach01_texlist;
	for (size_t i = 0; i < LengthOfArray(palmtreearray); i++)
	{
		njPushMatrix(nullptr);
		njTranslate(_nj_current_matrix_ptr_, palmtreearray[i].pos.x, palmtreearray[i].pos.y, palmtreearray[i].pos.z);
		if (palmtreearray[i].zrot)
			njRotateZ(_nj_current_matrix_ptr_, palmtreearray[i].zrot);
		if (palmtreearray[i].xrot)
			njRotateX(_nj_current_matrix_ptr_, palmtreearray[i].xrot);
		if (palmtreearray[i].yrot)
			njRotateY(_nj_current_matrix_ptr_, palmtreearray[i].yrot);
		sub_42E730(palmtreemodels[palmtreearray[i].id]);
		njPopMatrix(1);
	}
}

void __cdecl PalmtreeMan_Main(ObjectMaster *obj)
{
	if (!obj->DisplaySub_Delayed1)
		obj->DisplaySub_Delayed1 = PalmtreeMan_Display;
}

VoidFunc(sub_6EDE40, 0x6EDE40);
void MusicHook()
{
	LoadObject(0, "MusicMan", MusicMan_Main, LoadObj_Data1);
	PlayMusic("ecoast1.adx");
	LoadObject(0, "TeleportMan", TeleportMan_Main, (LoadObj)0);
	LoadObject(0, "PalmtreeMan", PalmtreeMan_Main, (LoadObj)0);
	if (!WaterRippleFunc_ptr)
		WaterRippleFunc_ptr = ObjectWaterripple_Load;
	if (!ExecFunc_ptr)
		ExecFunc_ptr = Exec_Load;
	sub_6EDE40();
}

FunctionPointer(void, sub_77F9F0, (NJS_TEXLIST *), 0x77F9F0);
VoidFunc(sub_6DFE10, 0x6DFE10);
void DeInitHook()
{
	sub_6DFE10();
	if (ExecFunc_ptr == Exec_Load)
	{
		ExecFunc_ptr = 0;
	}
	sub_77F9F0((NJS_TEXLIST*)0xB09818);
	if (WaterRippleFunc_ptr == ObjectWaterripple_Load)
	{
		WaterRippleFunc_ptr = 0;
	}
}

NJS_TEXLIST *objtexlists[18];

const char *const treemodelnames[] = { "\\YASI0.sa2mdl", "\\YASI1.sa2mdl", "\\YASI2.sa2mdl", "\\YASI3.sa2mdl" };

StartPosition startpos = { LevelIDs_CityEscape, 0x6000, 0x6000, 0x6000, { -9, 16, 4 }, { -9, 16, 0 }, { -9, 16, 8 } };
LevelEndPosition _2pintro = { LevelIDs_CityEscape, 0x8000u, 0x4000, 0, { 5956.75f, 400, 765.875f }, { -623.25f, 116, 1224.625f } };
StartPosition endpos = { LevelIDs_CityEscape, 0, 0, 0, { 6968.11f, 0.48f, -4253.34f }, { 6968.11f, 0.48f, -4253.34f }, { 6968.11f, 0.48f, -4253.34f } };
LevelEndPosition endpos23 = { LevelIDs_CityEscape, 0, 0, 0, { 6968.11f, 0.48f, -4253.34f }, { 6968.11f, 0.48f, -4253.34f } };

// ---- Reversible patching ----------------------------------------------------
// Every write Apply makes goes through these, which save the original bytes
// so Revert can put them back (in reverse order).

struct SavedBytes
{
	void *address;
	std::vector<uint8_t> original;
};

static std::vector<SavedBytes> savedBytes;

static void SaveBytes(void *address, size_t size)
{
	savedBytes.push_back({ address, std::vector<uint8_t>((uint8_t *)address, (uint8_t *)address + size) });
}

template <typename T>
static void PatchData(T *address, const T &value)
{
	SaveBytes(address, sizeof(T));
	WriteData(address, value);
}

static void PatchFill(void *address, uint8_t value, size_t count)
{
	SaveBytes(address, count);
	std::vector<uint8_t> bytes(count, value);
	WriteData(address, bytes.data(), count);
}

static void PatchCall(void *address, void *function)
{
	SaveBytes(address, 5);
	WriteCall(address, function);
}

static void RestoreAllPatches()
{
	for (auto it = savedBytes.rbegin(); it != savedBytes.rend(); ++it)
		WriteData(it->address, it->original.data(), it->original.size());
	savedBytes.clear();
}

// ---- Files ------------------------------------------------------------------
// Replacements for level 13 live in <mod>\stage13\ (same layout as gd_PC),
// not in gd_PC, so the mod loader doesn't apply them globally at start-up.

// The level Emerald Coast is played on: City Escape's 13, or its own ID when
// the 2P mod gave it one.
static const int kEmeraldCoastLevelId = 96;
static int stageLevel = LevelIDs_CityEscape;

// Files the game names after the level ID (stored under the level-13 names).
static const char *const levelFiles[] = {
	"set%04d_2p_s.bin", "set%04d_2p_u.bin", "set%04d_s.bin", "set%04d_u.bin",
	"stg%02dcam.prs",
};

// Files City Escape's level code loads by name.
static const char *const stageFiles[] = {
	"MLT\\se_ac_ce\\7.csb",
	"Message\\missione.prs",
	"PRS\\beach01.pak", "PRS\\beach_sea.pak", "PRS\\bg_beach.pak",
	"PRS\\stg_title13.pak",
};

// The Stage Select's textures (City Escape's entry on the map shown as
// Emerald Coast). Only while Emerald Coast replaces City Escape (level 13;
// on its own level ID it isn't on the map), and only if no other mod
// replaces the file: Character Select Plus's has extra character icons, and
// after the stock-sized one is loaded they point at freed textures (its
// character menu crashed).
static const char *const stageMapFile = "PRS\\stageMap.pak";

static std::string modPath;
static const HelperFunctions *helpers = nullptr;

static std::string GamePath(const char *file)
{
	return std::string("resource\\gd_PC\\") + file;
}

static std::string LevelFileName(const char *format, int level)
{
	char name[64];
	snprintf(name, sizeof(name), format, level);
	return name;
}

// The file another mod replaces the path with, or null. GetReplaceablePath
// returns its argument when nothing replaces the path.
static const char *OtherReplacement(const std::string &path)
{
	const char *current = helpers->GetReplaceablePath(path.c_str());
	if (current && current != path.c_str() && _stricmp(current, path.c_str()) != 0)
		return current;
	return nullptr;
}

// A file swapped in, and the replacement it covered: another mod's file is
// put back afterwards (UnreplaceFile alone removes every replacement).
struct SwappedFile
{
	std::string path;
	std::string previous; // empty = none
	int previousIndex;
};
static std::vector<SwappedFile> swappedFiles;

static void SwapIn(const std::string &path, const std::string &file)
{
	SwappedFile f = { path, std::string(), -1 };
	if (const char *previous = OtherReplacement(path))
	{
		f.previous = previous;
		if (helpers->Version >= 16)
			f.previousIndex = helpers->GetFileModIndex(path.c_str());
	}
	helpers->ReplaceFile(path.c_str(), file.c_str());
	swappedFiles.push_back(f);
}

static void ReplaceStageFiles()
{
	swappedFiles.clear();
	for (const char *format : levelFiles)
		SwapIn(GamePath(LevelFileName(format, stageLevel).c_str()),
			modPath + "\\stage13\\" + LevelFileName(format, LevelIDs_CityEscape));
	for (const char *file : stageFiles)
		SwapIn(GamePath(file), modPath + "\\stage13\\" + file);
	if (stageLevel == LevelIDs_CityEscape && !OtherReplacement(GamePath(stageMapFile)))
		SwapIn(GamePath(stageMapFile), modPath + "\\stage13\\" + stageMapFile);
}

static void UnreplaceStageFiles()
{
	if (helpers->Version < 12)
		return;
	for (const SwappedFile &f : swappedFiles)
	{
		std::string path = f.path;
		helpers->UnreplaceFile(path.c_str());
		// ReplaceFile also maps "gd_PC\PRS\x.pak" to "gd_PC\x.prs"; undo that too.
		size_t prs = path.find("\\PRS\\");
		if (prs != std::string::npos)
		{
			path.erase(prs, 4);
			path.replace(path.size() - 4, 4, ".prs");
			helpers->UnreplaceFile(path.c_str());
		}
		if (f.previous.empty())
			continue;
		if (f.previousIndex >= 0)
			helpers->ReplaceFileAtIndex(f.path.c_str(), f.previous.c_str(), f.previousIndex);
		else
			helpers->ReplaceFile(f.path.c_str(), f.previous.c_str());
	}
	swappedFiles.clear();
}

// ---- Positions ----------------------------------------------------------------
// The mod loader keeps one start/end/intro position per character and level,
// so re-registering the stock level-13 entries restores them.

static const StartPosition *FindStart(const StartPosition *list, int count)
{
	for (int i = 0; i < count; i++)
		if (list[i].Level == LevelIDs_CityEscape)
			return &list[i];
	return nullptr;
}

static const LevelEndPosition *FindEnd(const LevelEndPosition *list, int count)
{
	for (int i = 0; i < count; i++)
		if (list[i].Level == LevelIDs_CityEscape)
			return &list[i];
	return nullptr;
}

struct StockPositions
{
	bool hasStart, hasEnd, has2PIntro, hasMission23;
	StartPosition start, end;
	LevelEndPosition intro2p, mission23;
};

static StockPositions stockPositions[Characters_Amy];

static void CaptureStockPositions()
{
	struct Lists
	{
		const StartPosition *start; int startCount;
		const StartPosition *end; int endCount;
		const LevelEndPosition *intro; int introCount;
		const LevelEndPosition *m23; int m23Count;
	};
	const Lists lists[Characters_Amy] = {
		{ &SonicStartArray[0], 21, &SonicEndArray[0], 22, &Sonic2PIntroArray[0], 12, &SonicMission23EndArray[0], 9 },
		{ &ShadowStartArray[0], 18, &ShadowEndArray[0], 19, &Shadow2PIntroArray[0], 12, &ShadowMission23EndArray[0], 6 },
		{ &TailsStartArray[0], 2, nullptr, 0, nullptr, 0, nullptr, 0 },
		{ nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0 },
		{ &KnucklesStartArray[0], 18, &KnucklesEndArray[0], 18, &Knuckles2PIntroArray[0], 12, &KnucklesMission23EndArray[0], 7 },
		{ &RougeStartArray[0], 17, &RougeEndArray[0], 18, &Rouge2PIntroArray[0], 12, &RougeMission23EndArray[0], 6 },
		{ &MechTailsStartArray[0], 20, &MechTailsEndArray[0], 19, &MechTails2PIntroArray[0], 11, &MechTailsMission23EndArray[0], 6 },
		{ &MechEggmanStartArray[0], 22, &MechEggmanEndArray[0], 21, &MechEggman2PIntroArray[0], 11, &MechEggmanMission23EndArray[0], 7 },
	};
	for (int c = 0; c < Characters_Amy; c++)
	{
		StockPositions &s = stockPositions[c];
		const StartPosition *p;
		const LevelEndPosition *e;
		if ((s.hasStart = (p = FindStart(lists[c].start, lists[c].startCount)) != nullptr)) s.start = *p;
		if ((s.hasEnd = (p = FindStart(lists[c].end, lists[c].endCount)) != nullptr)) s.end = *p;
		if ((s.has2PIntro = (e = FindEnd(lists[c].intro, lists[c].introCount)) != nullptr)) s.intro2p = *e;
		if ((s.hasMission23 = (e = FindEnd(lists[c].m23, lists[c].m23Count)) != nullptr)) s.mission23 = *e;
	}
}

static void RegisterEmeraldCoastPositions()
{
	for (uint8_t i = 0; i < Characters_Amy; i++)
	{
		helpers->RegisterStartPosition(i, startpos);
		helpers->Register2PIntroPosition(i, _2pintro);
		if (helpers->Version >= 5)
		{
			helpers->RegisterEndPosition(i, endpos);
			helpers->RegisterMission23EndPosition(i, endpos23);
		}
	}
}

// Characters without a stock level-13 entry keep Emerald Coast's; they never
// load City Escape in the stock game.
static void RegisterStockPositions()
{
	for (uint8_t i = 0; i < Characters_Amy; i++)
	{
		const StockPositions &s = stockPositions[i];
		if (s.hasStart) helpers->RegisterStartPosition(i, s.start);
		if (s.has2PIntro) helpers->Register2PIntroPosition(i, s.intro2p);
		if (helpers->Version >= 5)
		{
			if (s.hasEnd) helpers->RegisterEndPosition(i, s.end);
			if (s.hasMission23) helpers->RegisterMission23EndPosition(i, s.mission23);
		}
	}
}

// ---- Load / Apply / Revert ----------------------------------------------------

static LandTable *cityEscapeLand = nullptr; // objLandTable0013 in Data_DLL
static LandTable emeraldCoastLand;
static bool applied = false;

// Memory only: loads models and the land table, prepares lists. No patches.
static void LoadAssets()
{
	HMODULE hmodule = GetModuleHandle(__TEXT("Data_DLL_orig"));
	cityEscapeLand = (LandTable *)GetProcAddress(hmodule, "objLandTable0013");
	emeraldCoastLand = *(new LandTableInfo(modPath + "\\LandTable.sa2lvl"))->getlandtable();
	emeraldCoastLand.TextureList = &beach01_texlist;
	emeraldCoastLand.TextureName = (char*)"beach01";
	stru_103B270 = (new ModelInfo(modPath + "\\Skybox model.sa2mdl"))->getmodel();
	stru_103B37C = (new ModelInfo(modPath + "\\Skybox bottom model.sa2mdl"))->getmodel();
	Obj_EmeraldCoast_InitOceanData();
	for (int i = 0; i < 4; i++)
		palmtreemodels[i] = (new ModelInfo(modPath + treemodelnames[i]))->getmodel();
	memcpy(objtexlists, (void*)0x109E6E0, 0x40);
	objtexlists[16] = (NJS_TEXLIST*)0xB0A54C;
	objtexlists[17] = nullptr;
	CaptureStockPositions();
}

// Turns level 13 into Emerald Coast.
static void Apply()
{
	if (applied)
		return;
	applied = true;
	PatchData(cityEscapeLand, emeraldCoastLand);
	PatchData((const char **)0x109E820, (const char *)"landtx13");
	PatchCall((void*)0x5DCDF7, LandManagerHook);
	PatchData((LoopHead***)0x5DD124, static_cast<LoopHead**>(PathList_EmeraldCoast0));
	PatchCall((void*)0x5DD119, MusicHook);
	PatchData((DeathZone**)0x5DD226, static_cast<DeathZone*>(EmeraldCoast1DeathZones));
	PatchData((char*)0x173B319, (char)2); // mission 4 time
	PatchData((char*)0x5DD4F0, (char)0xC3); // disable voice, hardcoded death boundary
	PatchData((ObjectFuncPtr*)0x5DD213, (ObjectFuncPtr)Background_Display);
	PatchData((ObjectFuncPtr*)0x5DD21A, (ObjectFuncPtr)Obj_EC1Water_Display);
	PatchData((NJS_TEXLIST****)0x5DCE6C, &copylist);
	PatchData((TexPackInfo**)0x5DCE71, static_cast<TexPackInfo*>(bgtexpacks));
	PatchData((NJS_TEXLIST****)0x5DD3E4, &copylist);
	PatchData((TexPackInfo**)0x5DD3E9, static_cast<TexPackInfo*>(bgtexpacks));
	PatchFill((void*)0x47C2BC, 0x90, 2); // transparency fix
	PatchData((char*)0x5DCD6D, (char)0xEB); // 2p bgm
	PatchFill((void*)0x5DD035, 0x90, 6); // 2p bgm
	PatchData(&CityEscape_ObjectArray[91], ObjectListEntry{ LoadObj_Data1, 2, 4, 0, E_SARU, (char*)"E SARU" });
	PatchData(&CityEscape_ObjectArray[57], ObjectListEntry{ LoadObj_Data1, 2, 4, 0, (ObjectFuncPtr)0x4FDD00, (char*)"E EMI" });
	PatchData(&CityEscape_ObjectArray[53], ObjectListEntry{ LoadObj_Data1, 2, 1, 1000000, (ObjectFuncPtr)0x638CE0, (char*)"WIND2" });
	PatchData((NJS_TEXLIST***)0x109E748, static_cast<NJS_TEXLIST**>(objtexlists));
	PatchCall((void*)0x5DD360, DeInitHook);
	PatchCall((void*)0x5DD22F, LoadDryLagoon2PCharAnims);
	// On its own level ID the 2P mod registers the positions (descriptor).
	if (stageLevel == LevelIDs_CityEscape)
		RegisterEmeraldCoastPositions();
	ReplaceStageFiles();
}

// Puts City Escape back.
static void Revert()
{
	if (!applied)
		return;
	applied = false;
	RestoreAllPatches();
	if (stageLevel == LevelIDs_CityEscape)
		RegisterStockPositions();
	UnreplaceStageFiles();
}

// ---- 2P mod integration (api/sa2_2p_api.h in SA2_1P_in_2P) -----------------

static bool registeredWith2P = false;
static bool checkedFallback = false;

static void __cdecl Activate2P(void *) { Apply(); }
static void __cdecl Deactivate2P(void *) { Revert(); }

extern "C"
{
	// Called by the SA2 2P mod from its Init (before or after ours).
	__declspec(dllexport) void __cdecl SA2TP_Init(const SA2TP_API *api, const HelperFunctions *)
	{
		if (!api || api->version < 1)
			return;
		SA2TP_StageDesc d = {};
		d.size = sizeof(d);
		d.id = "MainMemory.EmeraldCoast";
		d.name = "Emerald Coast";
		d.modes = SA2TP_MODE_RACE;
		d.hostLevel = LevelIDs_CityEscape;
		d.activate = Activate2P;
		d.deactivate = Deactivate2P;
		// API v4: the 1P Stage Select's Custom page. Missions 1 (goal) and 2
		// (100 rings) work as they are; 3 (lost Chao) and 4 (time limit) would
		// need level data of their own.
		d.character = Characters_Sonic;
		d.missions = 2;
		// API v6: rank requirements for D, C, B, A (example values).
		// A short level: lower than City Escape's.
		// Mission 1: least points (score + time bonus).
		// Mission 2 (100 rings): slowest time in seconds.
		const int m1[4] = { 5000, 8000, 11000, 14000 };
		const int m2[4] = { 270, 210, 165, 130 }; // 4:30, 3:30, 2:45, 2:10
		for (int k = 0; k < 4; ++k)
		{
			d.rankRequirements[0][k] = m1[k];
			d.rankRequirements[1][k] = m2[k];
		}
		if (api->version >= 3)
		{
			// Own level ID: City Escape's (patched) code with level 96's files
			// and positions. The 2P mod registers the positions for level 96.
			d.levelId = kEmeraldCoastLevelId;
			d.startPosition = &startpos;
			d.endPosition = &endpos;
			d.intro2P = &_2pintro;
			d.end23 = &endpos23;
			if (api->AddStage(&d))
			{
				stageLevel = kEmeraldCoastLevelId;
				registeredWith2P = true;
				return;
			}
			// ID taken or custom level IDs off: borrow level 13 instead.
			d.levelId = 0;
			d.startPosition = d.endPosition = nullptr;
			d.intro2P = d.end23 = nullptr;
		}
		registeredWith2P = api->AddStage(&d) != 0;
	}

	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		if (helperFunctions.Version < 5)
			MessageBoxA(MainWindowHandle, "Your copy of the mod loader does not support API version 5. Some functionality will not be available.\n\nPlease exit the game and update the mod loader for the best experience.", "Emerald Coast Mod", MB_ICONWARNING);
		modPath = path;
		helpers = &helperFunctions;
		LoadAssets();
	}

	// All mods' Init have run by the first frame, so by now we know whether
	// the 2P mod registered us. If not, replace City Escape for good.
	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (checkedFallback)
			return;
		checkedFallback = true;
		if (!registeredWith2P)
			Apply();
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}

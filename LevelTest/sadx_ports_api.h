#pragma once
/*
 * SADX Object Ports API: lets other mods (level mods) place the ported SADX
 * objects in their own stages.
 *
 * Copy this header into your mod. At run time (after every mod's Init, e.g.
 * when your level is set up), look the API up:
 *
 *   HMODULE dll = GetModuleHandleA("sadx_ports.dll");
 *   SADXPorts_GetAPIFunc get = dll ? (SADXPorts_GetAPIFunc)GetProcAddress(dll, "SADXPorts_GetAPI") : NULL;
 *   const SADXPorts_API *api = get ? get() : NULL;
 *
 * If it's missing (SADX Object Ports not installed or disabled), fall back to
 * your own objects.
 *
 * Using it:
 *  - FindObject gives an object list entry for a ported object, by its SADX
 *    object list name ("O GOMBAN", "E SARU", ...). Put the entries in your
 *    level's object list; the SET entries use SADX's own parameters (rotation,
 *    position, scale as in SADX's SET file).
 *  - Call LoadTexturesForObjects (version 2; LoadLevelTextures with version 1)
 *    while your level loads (before its objects) and FreeLevelTextures when
 *    it is freed. The ported objects draw with these
 *    textures; free also stops the objects' sounds.
 *  - StepSea (version 7): Emerald Coast act 0's sea ripples (the orcas).
 *  - MSetPosition (version 3): SADX's ball sweep against the level (sonic.exe
 *    0x43A030), on SA2's collision, for code ported from SADX that calls it
 *    (e.g. the path cameras' wall stop).
 *
 * Needs SA2ModLoader.h included first (ObjectListEntry).
 */

#define SADXPORTS_API_VERSION 7

/* Version 4: a SADX path (LoopHead): ends 0 stop / 1 loop, the points
   (SADX Loop: Sint16 ang x, ang z, float length to the next, position). */
typedef struct SADXPorts_PathPoint
{
	Sint16 angX, angZ;
	float length;
	NJS_VECTOR pos;
} SADXPorts_PathPoint;

typedef struct SADXPorts_Path
{
	int ends;
	int count;
	float length;
	const SADXPorts_PathPoint *points;
} SADXPorts_Path;

typedef struct SADXPorts_API
{
	int version; /* SADXPORTS_API_VERSION of the installed mod */

	/* The ported object with this SADX object list name (case and trailing
	   spaces ignored), or NULL if it isn't ported. Valid for the session.
	   An enemy type switched off in the mod's options gives an entry that
	   deletes itself. */
	const ObjectListEntry *(__cdecl *FindObject)(const char *sadxName);

	/* The ported objects' textures, loaded / freed with your level.
	   LoadLevelTextures loads every object's (~1450 textures): with a big
	   level of your own that can pass SA2's limit of 2048 and crash. Use
	   LoadTexturesForObjects (version 2) instead. */
	void (__cdecl *LoadLevelTextures)(void);
	void (__cdecl *FreeLevelTextures)(void);

	/* Version 2: only the textures the named objects need (the same SADX
	   names as FindObject), e.g. the names you put in your object list.
	   Free them with FreeLevelTextures. */
	void (__cdecl *LoadTexturesForObjects)(const char *const *sadxNames, int count);

	/* Version 3: SADX MSetPosition (0x43A030) with a null angle: a ball of
	   radius from pos along speed against the level's collision. Returns
	   nonzero on a contact (1 ground, 2 wall, or both), and then pos and
	   speed take the nearest contact's: the ball's place against the
	   surface and the speed along it. */
	int (__cdecl *MSetPosition)(NJS_VECTOR *pos, NJS_VECTOR *speed, float radius);

	/* Version 4: a task for a SADX path whose handler (the LoopHead's object
	   function, a sonic.exe address, e.g. 0x603640 Red Mountain's pulleys) is
	   ported; NULL if it isn't. The points are copied. The task is an SA2
	   object of list 2; delete it with DeleteObject_ (e.g. on an act change). */
	ObjectMaster *(__cdecl *LoadPathTask)(unsigned sadxHandler, const SADXPorts_Path *path);

	/* Version 5: the tasks a SADX stage's own task makes as an act starts
	   (e.g. Red Mountain act 0's bursting rocks, mountain_start_act), for the
	   ported ones: up to max into out; returns how many. The caller deletes
	   them with DeleteObject_ (e.g. on an act change). */
	int (__cdecl *LoadActTasks)(int sadxStage, int act, ObjectMaster **out, int max);

	/* Version 6: nonzero once when a ported object has asked for the stage's
	   next act (SADX's world / placement / camera / stage change act: Sky
	   Deck's big cannons, 60 steps after a rocket breaks one). Poll it each
	   frame while your level plays, and change act when it fires. */
	int (__cdecl *TakeActChange)(void);

	/* Version 7: Emerald Coast act 0's sea task (SADX Obj_EC1Water_Main
	   0x5017D0): call once a step while act 0 plays. It steps the ripples
	   the ported orcas made (EC_SpawnRipple) and returns how many live.
	   SADX PC draws act 0's ocean (OceanDataArray[0]) at
	   sin(FrameCounterUnpaused << 11) * 3 + 2.5 while any live, else
	   lowers it 0.1 a step to -1.5 (Obj_EC1Water_Display 0x501130). */
	int (__cdecl *StepSea)(void);
} SADXPorts_API;

/* Exported by sadx_ports.dll as "SADXPorts_GetAPI". */
typedef const SADXPorts_API *(__cdecl *SADXPorts_GetAPIFunc)(void);

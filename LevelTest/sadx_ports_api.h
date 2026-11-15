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
 *
 * Needs SA2ModLoader.h included first (ObjectListEntry).
 */

#define SADXPORTS_API_VERSION 2

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
} SADXPorts_API;

/* Exported by sadx_ports.dll as "SADXPorts_GetAPI". */
typedef const SADXPorts_API *(__cdecl *SADXPorts_GetAPIFunc)(void);

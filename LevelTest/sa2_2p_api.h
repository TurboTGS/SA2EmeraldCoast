/*
 * SA2 2P stage API (SA2_1P_in_2P mod), version 1.
 *
 * Lets a level mod add its level to the 2P battle stage select WITHOUT
 * replacing a stock level. A mod stage borrows a stock "host" level slot only
 * while it is being played: the 2P mod calls `activate` just before the match
 * loads and `deactivate` once the game is back in the menus, so the host
 * level is the stock level everywhere else (story mode, other 2P picks).
 *
 * How to use (copy this header into your mod):
 *
 *   extern "C" __declspec(dllexport)
 *   void __cdecl SA2TP_Init(const SA2TP_API* api, const HelperFunctions* helpers)
 *   {
 *       SA2TP_StageDesc d = {};
 *       d.size = sizeof(d);
 *       d.id = "Author.MyLevel";
 *       d.name = "My Level";
 *       d.modes = SA2TP_MODE_RACE;
 *       d.hostLevel = LevelIDs_CityEscape;   // whose files/code you patch
 *       d.activate = MyApply;                 // apply your patches + files
 *       d.deactivate = MyRevert;              // undo them
 *       api->AddStage(&d);
 *   }
 *
 * The 2P mod calls SA2TP_Init in every loaded mod that exports it, from its
 * own Init. All mod DLLs are loaded before any Init runs, so mod order does
 * not matter. SA2TP_Init may run before or after your own Init, so don't
 * depend on state your Init sets up. If SA2TP_Init is never called (2P mod
 * not installed), apply your patches permanently yourself, e.g. on your
 * first OnFrame.
 *
 * Full guide: docs/mod_stage_api.md in the SA2_1P_in_2P repository.
 */
#ifndef SA2_2P_API_H
#define SA2_2P_API_H

#define SA2TP_API_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

struct HelperFunctions; /* from the SA2 mod loader (SA2ModInfo.h) */

enum {
    SA2TP_MODE_RACE  = 1, /* Action Race     (BattleSelection 1) */
    SA2TP_MODE_HUNT  = 2, /* Treasure Hunt   (BattleSelection 2) */
    SA2TP_MODE_SHOOT = 4  /* Shooting Battle (BattleSelection 3) */
};

typedef struct SA2TP_StageDesc {
    int         size;      /* sizeof(SA2TP_StageDesc), for future versions */
    const char* id;        /* unique, e.g. "MainMemory.EmeraldCoast"      */
    const char* name;      /* shown in the menus, e.g. "Emerald Coast"    */
    int         modes;     /* SA2TP_MODE_* bitmask                         */
    int         hostLevel; /* level ID borrowed while playing             */
    int         group;     /* Current2PLevelGroup ruleset, 0 = default:
                              1 special (no health bar...), 2 = 4 min,
                              3 = 5 min, 4 = 7 min, 5+ = 10 min           */
    /* Called before the level loads / once back in the menus. Must be
       idempotent-safe: deactivate is only called after activate. */
    void (__cdecl* activate)(void* user);
    void (__cdecl* deactivate)(void* user);
    void*       user;      /* passed to the callbacks                      */
} SA2TP_StageDesc;

typedef struct SA2TP_API {
    int version; /* SA2TP_API_VERSION of the 2P mod */
    /* Returns 1 on success. The descriptor and its strings are copied. */
    int (__cdecl* AddStage)(const SA2TP_StageDesc* desc);
} SA2TP_API;

/* Export this from your mod as "SA2TP_Init". */
typedef void (__cdecl* SA2TP_InitFunc)(const SA2TP_API* api,
                                       const struct HelperFunctions* helpers);

#ifdef __cplusplus
}
#endif

#endif /* SA2_2P_API_H */

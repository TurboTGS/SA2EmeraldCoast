Source code for the mod and the program used to convert the level geometry and object layouts.


This is the first ever fully custom action stage for SA2, a port of Emerald Coast from the PC version of Sonic Adventure DX. The level replaces City Escape.

The main geometry was converted to SA2's format by a program I wrote. This program also handled merging many objects into the level geometry, and converting others to SA2 equivalents, as well as merging all three acts of the level into one. I had to write an object to render the palm trees, as SA2's transparency handling is awful and they were rendering behind the ocean. I've also written custom objects to handle music changes and the act transition. I've ported the skybox and ocean code from SADX, which I believe is the first time something like that has been done.

Unfortunately, the cameras could not be ported from SADX to SA2 in time for the contest. This will be fixed later.



## 2P stage (SA2_1P_in_2P)

This branch registers Emerald Coast with the SA2 2P mod
([SA2_1P_in_2P](https://github.com/TurboTGS/SA2_1P_in_2P)) through its 2P
stage API (`LevelTest/sa2_2p_api.h`). With the 2P mod installed, Emerald
Coast is its own stage in the 2P Race stage select, and City Escape stays
City Escape: the level-13 patches and file replacements are applied only
while an Emerald Coast match is played and undone afterwards.

Without the 2P mod it behaves as before and replaces City Escape for good
(applied on the first frame).

Files that replace level-13 data now live in `dist/stage13/` (same layout
as `gd_PC`) and are swapped in at runtime; the new music stays in
`dist/gd_PC/ADX/`.

### Building
Build `LevelTest/LevelTest.vcxproj` (Release, Win32). It uses the
`sa2-mod-loader` submodule by default; to use another copy:

    msbuild LevelTest\LevelTest.vcxproj /p:Configuration=Release /p:Platform=Win32 ^
        /p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0 ^
        /p:SA2ModLoaderDir=path\to\sa2-mod-loader\

It links `bin\libmodutils.lib`, built from that copy's
`libmodutils\libmodutils.vcxproj` with the same toolset. Copy the result to
`dist\leveltest.dll`.

Source code for the mod and the program used to convert the level geometry and object layouts.


This is the first ever fully custom action stage for SA2, a port of Emerald Coast from the PC version of Sonic Adventure DX. The level replaces City Escape.

The main geometry was converted to SA2's format by a program I wrote. This program also handled merging many objects into the level geometry, and converting others to SA2 equivalents, as well as merging all three acts of the level into one. I had to write an object to render the palm trees, as SA2's transparency handling is awful and they were rendering behind the ocean. I've also written custom objects to handle music changes and the act transition. I've ported the skybox and ocean code from SADX, which I believe is the first time something like that has been done.

Unfortunately, the cameras could not be ported from SADX to SA2 in time for the contest. This will be fixed later.


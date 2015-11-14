# bgbtech_engine
My previous 3D engine.

License: Previously CC BY-NC-ND
* Was intended so people would pay me if they wanted to use it for something.
* Now, dunno, may change it if it matters...

Changed: CC BY-NC
* Excptions for BSCC and other subsystems marked as MIT, which are MIT licensed.

Or, Alternatively: BGB3
* All this isn't really "set in stone" as of yet.


Things granted:
* People are free to modify and compile the code for personal use.
* People are free to redistribute the code, or mods for the code, in source or binary form.
* It can be used for non-commercial purposes.
* If you are making money off it, I ask 1% (of net profit) if this value would exceed $25.
* Can be used under MIT terms if you pay me the 1%.

Drop of ND:
* Originally the ND was intended as a restriction on distributing otherwise fully complete games.
* However, this makes mods unclear.
* I want to allow mods, as well as general redistribution of source and binary programs.


Everything in "bscc/" is licensed under MIT, as well as any sub-libraries which carry the MIT license.


Contents:
* apps: engine front ends
* bgbbtjpg: mostly video compression, image processing, and texture compression stuff.
* bgbmid1: MIDI synthesis engine + 3D engine's main mixer
* bscc: BGBScript VM, also a lot of engine infrastructure (MM, VFS, ...).
* btanim2d: partial old 2D animation tool
* btgecl: game-engine client end
* btgecm: game-engine common
* btgesv: game-engine server end
* bttile2d: experimental mini 2D sidescroller thing.
* lbxgl3: main 3D renderer front-end, does world rendering and similar
* libbsde21: rigid body physics engine, not really used (didn't implement voxel terrain support for it)
* libpdgl: main 3D renderer back-end, interfaces with OpenGL and OS
* libvecmath: assorted math code
* tremor: Ogg/Vorbis decoding
* tools: some misc batch tools
* 
* docs: some random documentation.
* include: header files
* resource: root directory for resources/assets
* resource_mini: released assets
* resource_minitmp: release assets prior to 'pk' packaging.

Within BSCC:
* bgbasm21: Assembler+Linker library (converts ASM code into machine code and links into running program)
* bgbcc2: old C compiler front-end
* bgbdy: most of the VM and 3D engine core-runtime stuff
* bgbfrir*: early predecessors for the FRVM (unused)
* bgbgc1: main memory-manager / garbage-collector.
* bgbmeta: tool to process code and generate metadata for the script VM
* bgbnet: network code
* bgbsvm12: BGBScript VM (parser, compiler/interpreter, and JIT)
* bs2bvm0: early root for what later mutated into BGBScript-RT
* docs: more random documentation (related to BSCC)
* include: headers
* tools: some misc tools
* zpak2: library which does read/write ZIP stuff (for the VFS).

Files:
* 'pk': VFS is mostly stored in ZIP files with a '.pk' extension.
* 'pak': audio mixer uses an extended variant of the Quake 1/2 PAK format.
* Audio is compressed here using BTAC, which is a specialized CBR audio codec.
* Audio data may be decompressed in a random-access pattern, and the BTAC compression saves RAM.
* The main alternative would have been to use a constrained form of IMA ADPCM.

Textures:
* The engine can load PNG or JPEG textures, but in the PK, a modified JPEG is used.
* In engine, the main (actually used) feature was the addition of an embedded alpha channel.
* Most should decode in normal JPEG viewers, but these will ignore the alpha channel.

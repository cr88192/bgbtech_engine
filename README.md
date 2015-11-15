# bgbtech_engine
My previous 3D engine.

License: MIT

Decided to move the engine over to MIT.

Now, all the engine code is MIT, at least as far as the code I have written is concerned.

Donations will be appriciated though.


'engine0.exe' is the main 3D engine/game frontend.

Other EXEs represent various tools or internal tests or other things.


Main Contents
=============

Contents:
* apps: engine front ends
* bgbbtjpg: mostly video compression, image processing, and texture compression stuff. Can also build a VfW driver and VLC plug-in.
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


BGBBTJPG
--------

bgbbtjpg:
Main library for things like JPEG decompression, texture compression, video compression/decompression, ... This can also generate a VfW driver, as well as a VLC plug-in. These can be used for BTIC-family codecs (presently excluding 1G or 1H).

The engine also uses MJPEG and RPZA, which are generally optional (disabled by default) codecs supported by libavcodec/ffdshow. Both support hacks to add alpha-channel support, but these don't normally effect playback.

Had also experimented some with Theora video, but this wasn't really used by the engine.

An area for concern is that the DXTn compression may be subject to S3TC.


BTGECM/BTGECL/BTGESV
--------------------

btecm/btgecl/btgesv:
These implement much of the "game proper".
This includes the voxel terrain, and implementing player and entity movement messages. These are based on a binary serialized form of Cons-Lists / S-Expressions. Voxel terrain messages consist of large blobs of binary data. In retrospect, this was a pretty bad design, as it was a high-overhead design.

General architecture in this area was inspired to some extent by the idTech family of engines (mostly Quake 1 and 2), but it is not based on id's code.


LBXGL
-----

lbxgl:
Basically implements the renderer.
The renderer uses a multi-pass rendering strategy, where first all the lighted geometry is drawn, and then the the textures are drawn on top of this, followed by any translucent geometry being drawn afterwards.

Shadow-maps are used for terrain shadows, and depth-pass stencil shadows are used for those from moving entities or from dynamic light sources.

Early on, depth-fail had been used, but depth-pass is a legally safer alternative.


For older hardware, the renderer is capable of falling back to a single-pass fixed-function rendering strategy. In this case, vertex colors are used for lighting.


Misc
----

3rd Party Libraries:
Several Xiph.org libraries, mpglib, and an older version of the OculusODK are lurking in the codebase. These have their own licenses.

These are for non-critical functions, and could be omitted.


Assets:
Some of the art assets are derived from materials from OpenQuartz, CrystalSpace, FreeDoom, and GIMP. These may require different handling.

Some other assets are less certain, use at your own risk.


BSCC
====

Within BSCC:
* bgbasm21: Assembler+Linker library (converts ASM code into machine code and links into running program)
* bgbcc2: old C compiler front-end (unused by 3D engine)
* bgbdy: most of the VM and 3D engine core-runtime stuff
* bgbfrir*: early predecessors for the FRVM (unused)
* bgbgc1: main memory-manager / garbage-collector.
* bgbmeta: tool to process code and generate metadata for the script VM
* bgbnet: network code
* bgbsvm12: BGBScript VM (parser, compiler/interpreter, and JIT)
* bs2bvm0: early root for what later mutated into BGBScript-RT (unused)
* docs: more random documentation (related to BSCC)
* include: headers
* tools: some misc tools
* zpak2: library which does read/write ZIP stuff (for the VFS).


bgbasm21:
Uses a variant of NASM style ASM syntax with a modified C preprocessor.
Supports x86 and x86-64, with partial/experimental ARM and Thumb support (incomplete and untested). It is mostly intended as a back-end to allow JITs to more easily produce native code. It is capable of linking assembled modules against the running program, as well as patching over previously linked code.

bgbgc1:
Implements a conservative mark/sweep GC with support for annotating types with names (by convention constrained to C identifier rules). Given a pointer, you may fetch the type-name of an object, or verify whether the pointer points to a heap object at all, ...

For most smaller objects, memory is allocated in terms of 16-byte cells, which are allocated via bitmaps. For objects over a few kB, it falls back to using a conventional allocator with a big object array to track memory allocations. Binary search is used to locate specific objects.

bgbdy:
Builds on top of BGBASM and BGBGC, and implements many of the core VM facilities. This includes the dynamic typesystem, object system, VM metadata (via a system similar to the Windows Registry), and VFS. The VFS is loosely similar to the one in Linux, though it leaves it up to the individual FS drivers to manage their directory trees, and allows multiple filesystems to share a single mount point.

bgbsvm12:
Implements the core script interpreter. Its main purpose is to support running BGBScript code.

It uses a stack-based bytecode which is converted into a sort of threaded code. Alternatively, the code may be compiled to native via a fairly simplistic JIT (some operations produce specialized native code, others are simply calls back into the interpreter).


zpak2:
Supports random read/write for files, and may incrementally update the underlying ZIP (though a proper unmount is needed for a useable central-directory). In-use files are kept live in an in-memory cache, and recompressed and stored into the ZIP image periodically (or when the file is closed). This involves needing to keep the entire file in RAM. This library is not well suited to large members, and ZIPs are limited to around 2GB or so.

It is fairly high overhead so is not an ideal solution to read-only ZIPs (a separate VFS driver is used for read-only).


Files:
* 'pk': VFS is mostly stored in ZIP files with a '.pk' extension.
* 'pak': audio mixer uses an extended variant of the Quake 1/2 PAK format.
* Audio is compressed here using BTAC, which is a specialized CBR audio codec.
* Audio data may be decompressed in a random-access pattern, and the BTAC compression saves RAM.
* The main alternative would have been to use a constrained form of IMA ADPCM.
* 'bs': scripts in the BGBScript language.

Textures:
* The engine can load PNG or JPEG textures, but in the PK, a modified JPEG is used.
* In engine, the main (actually used) feature was the addition of an embedded alpha channel.
* Most should decode in normal JPEG viewers, but these will ignore the alpha channel.

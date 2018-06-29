# LockonSSE

port of https://github.com/himika/LockOn/tree/master/dll/src

thanks to iammitochondrion for releasing the source to https://www.nexusmods.com/skyrimspecialedition/mods/18086 which let me double check a few of my class definitions

## Building

- Download skse64's source
- Clone this repository as a folder in the skse64 solution folder
- Open skse64 solution and add the project
- in skse64/GameReferences.cpp update RelocPtr<CrosshairRefHandleHolder*> g_crosshairRefHandleHolder(0x02F281D0); on line 159
- Build
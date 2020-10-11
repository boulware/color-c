@echo off
SET CompilerDebugFlags = /showIncludes
SET META_PRINT=0
SET OUTPUT_PREPROCESSED=0
SET CompilerFlags=-DDEBUG_BUILD=1 -DMETA_PRINT=%META_PRINT%
pushd %~dp0

IF /I "%OUTPUT_PREPROCESSED%" EQU "1" (
cl.exe ../src/meta.cpp %CompilerFlags% /P /Fe../meta.exe /nologo /Zi /I /link user32.lib gdi32.lib
cl.exe ../src/game.cpp %CompilerFlags% /std:c++latest /P /LD /Fegame.dll /nologo /Zi /I "../src_extern/freetype/include" /link /LIBPATH:"libs" user32.lib gdi32.lib opengl32.lib freetype.lib /EXPORT:GameUpdateAndRender /EXPORT:GameHook /EXPORT:GameInit /out:../game.dll /incremental:no /pdb:game.pdb
cl.exe ../src/win32_platform.cpp %CompilerFlags% /std:c++latest /P /Fegame.exe /Zi /nologo /I "../src_extern/freetype/include" /link /LIBPATH:"libs" user32.lib gdi32.lib opengl32.lib freetype.lib /out:../game.exe /incremental:no /pdb:platform.pdb
)

cl.exe ../src/meta.cpp %CompilerFlags% /Fe../meta.exe /nologo /Zi /link user32.lib gdi32.lib
"../meta.exe"
cl.exe ../src/game.cpp %CompilerFlags% /std:c++latest /LD /Fegame.dll /nologo /Zi /I "../src_extern/freetype/include" /link /LIBPATH:"libs" user32.lib gdi32.lib opengl32.lib freetype.lib /EXPORT:GameUpdateAndRender /EXPORT:GameHook /EXPORT:GameInit /out:../game.dll /incremental:no /pdb:game.pdb
cl.exe ../src/win32_platform.cpp %CompilerFlags% /std:c++latest /Fegame.exe /Zi /nologo /I "../src_extern/freetype/include" /link /LIBPATH:"libs" user32.lib gdi32.lib opengl32.lib freetype.lib /out:../game.exe /incremental:no /pdb:platform.pdb
popd
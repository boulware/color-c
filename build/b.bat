@echo off
SET CompilerFlags=-DDEBUG_BUILD=1
pushd %~dp0
REM cl.exe ../src/meta.cpp /Fe../meta.exe /nologo /Zi
cl.exe ../src/game.cpp %CompilerFlags% /std:c++latest /P /LD /Fegame.dll /nologo /Zi /I "../src_extern/freetype/include" /link /LIBPATH:"libs" user32.lib gdi32.lib opengl32.lib freetype.lib /EXPORT:GameUpdateAndRender /EXPORT:GameHook /EXPORT:GameInit /out:../game.dll /incremental:no /pdb:game.pdb
cl.exe ../src/game.cpp %CompilerFlags% /std:c++latest /LD /Fegame.dll /nologo /Zi /I "../src_extern/freetype/include" /link /LIBPATH:"libs" user32.lib gdi32.lib opengl32.lib freetype.lib /EXPORT:GameUpdateAndRender /EXPORT:GameHook /EXPORT:GameInit /out:../game.dll /incremental:no /pdb:game.pdb
cl.exe ../src/win32_platform.cpp %CompilerFlags% /std:c++latest /P /Fegame.exe /Zi /nologo /I "../src_extern/freetype/include" /link user32.lib gdi32.lib opengl32.lib /out:../game.exe /incremental:no /pdb:platform.pdb
cl.exe ../src/win32_platform.cpp %CompilerFlags% /std:c++latest /Fegame.exe /Zi /nologo /I "../src_extern/freetype/include" /link user32.lib gdi32.lib opengl32.lib /out:../game.exe /incremental:no /pdb:platform.pdb
popd
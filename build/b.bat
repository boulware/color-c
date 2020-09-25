@echo off
pushd %~dp0
cl.exe ../src/game.cpp /P /LD /Fegame.dll /nologo /Zi /I "../src_extern/freetype/include" /link /LIBPATH:"libs" user32.lib gdi32.lib opengl32.lib freetype.lib /EXPORT:GameUpdateAndRender /EXPORT:GameHook /EXPORT:GameInit /out:../game.dll /incremental:no /pdb:game.pdb
cl.exe ../src/game.cpp /LD /Fegame.dll /nologo /Zi /I "../src_extern/freetype/include" /link /LIBPATH:"libs" user32.lib gdi32.lib opengl32.lib freetype.lib /EXPORT:GameUpdateAndRender /EXPORT:GameHook /EXPORT:GameInit /out:../game.dll /incremental:no /pdb:game.pdb
cl.exe ../src/win32_platform.cpp /Fegame.exe /Zi /nologo /I "../src_extern/freetype/include" /link user32.lib gdi32.lib opengl32.lib /out:../game.exe /incremental:no /pdb:platform.pdb
popd
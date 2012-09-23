rmdir /s /q bin
mkdir bin
mkdir bin\Languages

xcopy "..\bin\x86-ansicode\flyExplorer.exe" "bin" /c /r /y
xcopy "..\bin\x86-ansicode\fxcrash.dll" "bin" /c /r /y
xcopy "..\bin\x86-ansicode\libgfl340.dll" "bin" /c /r /y
xcopy "..\bin\x86-ansicode\libgfle340.dll" "bin" /c /r /y
cd bin
ren "flyExplorer.exe" "flyExplorerA.exe"
ren "libgfl340.dll" "libgfl340A.dll"
ren "libgfle340.dll" "libgfle340A.dll"
cd ..

xcopy "..\bin\x86-unicode\flyExplorer.exe" "bin" /c /r /y
xcopy "..\bin\x86-unicode\fxcrashu.dll" "bin" /c /r /y
xcopy "..\bin\x86-unicode\libgfl340.dll" "bin" /c /r /y
xcopy "..\bin\x86-unicode\libgfle340.dll" "bin" /c /r /y
xcopy "..\bin\x86-unicode\iconv.dll" "bin" /c /r /y
xcopy "..\bin\x86-unicode\zlib1.dll" "bin" /c /r /y
xcopy "..\bin\x86-unicode\libxml2.dll" "bin" /c /r /y
xcopy "..\apps\flyExplorer2\Languages" "bin\Languages" /c /e /r /y
cd bin
ren "flyExplorer.exe" "flyExplorerU.exe"
ren "libgfl340.dll" "libgfl340U.dll"
ren "libgfle340.dll" "libgfle340U.dll"
cd ..

xcopy "..\bin\fxKeyHook.dll" "bin" /c /r /y
xcopy "..\bin\fxLauncher.exe" "bin" /c /r /y

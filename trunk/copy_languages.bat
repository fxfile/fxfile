mkdir "bin"
mkdir "bin\x86-ansicode"
mkdir "bin\x86-ansicode\Languages"
mkdir "bin\x86-unicode"
mkdir "bin\x86-unicode\Languages"
mkdir "bin\x64-unicode"
mkdir "bin\x64-unicode\Languages"

xcopy "src\fxfile\Languages\*.*" "bin\x86-ansicode\Languages" /c /e /r /y
xcopy "src\fxfile\Languages\*.*" "bin\x86-unicode\Languages" /c /e /r /y
xcopy "src\fxfile\Languages\*.*" "bin\x64-unicode\Languages" /c /e /r /y

mkdir "bin"
mkdir "bin\x86-multibyte"
mkdir "bin\x86-multibyte\Languages"
mkdir "bin\x86-unicode"
mkdir "bin\x86-unicode\Languages"
mkdir "bin\x64"
mkdir "bin\x64\Languages"

xcopy "src\fxfile\Languages\*.*" "bin\x86-multibyte\Languages" /c /e /r /y
xcopy "src\fxfile\Languages\*.*" "bin\x86-unicode\Languages" /c /e /r /y
xcopy "src\fxfile\Languages\*.*" "bin\x64\Languages" /c /e /r /y

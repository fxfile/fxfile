mkdir "bin"
mkdir "bin\x86-ansicode"
mkdir "bin\x86-unicode"
mkdir "bin\x64-unicode"

xcopy "lib\fxCrash\bin\fxcrash.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\fxCrash\bin\fxcrashu.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\fxCrash\bin\fxcrashu.dll" "bin\x64-unicode" /c /e /r /y

xcopy "lib\gfl\lib\libgfl340.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\gfl\lib\libgfle340.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\gfl\libW\libgfl340.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\gfl\libW\libgfle340.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\gfl\lib64W\libgfl340.dll" "bin\x64-unicode" /c /e /r /y
xcopy "lib\gfl\lib64W\libgfle340.dll" "bin\x64-unicode" /c /e /r /y

xcopy "lib\iconv\bin\iconv.dll" "bin" /c /e /r /y
xcopy "lib\iconv\bin\iconv.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\iconv\bin\iconv.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\iconv\bin\iconv.dll" "bin\x64-unicode" /c /e /r /y

xcopy "lib\libxml2\bin\libxml2.dll" "bin" /c /e /r /y
xcopy "lib\libxml2\bin\libxml2.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libxml2\bin\libxml2.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libxml2\bin\libxml2.dll" "bin\x64-unicode" /c /e /r /y

xcopy "lib\libxml2\bin\zlib1.dll" "bin" /c /e /r /y
xcopy "lib\libxml2\bin\zlib1.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libxml2\bin\zlib1.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libxml2\bin\zlib1.dll" "bin\x64-unicode" /c /e /r /y

xcopy "lib\libcurl\bin\libcurl.dll" "bin" /c /e /r /y
xcopy "lib\libcurl\bin\libcurl.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libcurl\bin\libcurl.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libcurl\bin\libcurl.dll" "bin\x64-unicode" /c /e /r /y

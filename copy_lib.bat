mkdir "bin"
mkdir "bin\x86-ansicode"
mkdir "bin\x86-unicode"
mkdir "bin\x64"

xcopy "lib\gfl\lib\libgfl340.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\gfl\lib\libgfle340.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\gfl\libW\libgfl340.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\gfl\libW\libgfle340.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\gfl\lib64W\libgfl340.dll" "bin\x64" /c /e /r /y
xcopy "lib\gfl\lib64W\libgfle340.dll" "bin\x64" /c /e /r /y

xcopy "lib\mingwrt\bin\libgcc_s_sjlj-1.dll" "bin" /c /e /r /y
xcopy "lib\mingwrt\bin\libgcc_s_sjlj-1.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\mingwrt\bin\libgcc_s_sjlj-1.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\mingwrt\bin64\libgcc_s_sjlj-1.dll" "bin\x64" /c /e /r /y

xcopy "lib\mingwrt\bin\libwinpthread-1.dll" "bin" /c /e /r /y
xcopy "lib\mingwrt\bin\libwinpthread-1.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\mingwrt\bin\libwinpthread-1.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\mingwrt\bin64\libwinpthread-1.dll" "bin\x64" /c /e /r /y

xcopy "lib\iconv\bin\libiconv-2.dll" "bin" /c /e /r /y
xcopy "lib\iconv\bin\libiconv-2.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\iconv\bin\libiconv-2.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\iconv\bin64\libiconv-2.dll" "bin\x64" /c /e /r /y

xcopy "lib\iconv\bin\libcharset-1.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\iconv\bin\libcharset-1.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\iconv\bin64\libcharset-1.dll" "bin\x64" /c /e /r /y

xcopy "lib\libxml2\bin\libxml2-2.dll" "bin" /c /e /r /y
xcopy "lib\libxml2\bin\libxml2-2.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libxml2\bin\libxml2-2.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libxml2\bin64\libxml2-2.dll" "bin\x64" /c /e /r /y

xcopy "lib\libxml2\bin\zlib1.dll" "bin" /c /e /r /y
xcopy "lib\libxml2\bin\zlib1.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libxml2\bin\zlib1.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libxml2\bin64\zlib1.dll" "bin\x64" /c /e /r /y

xcopy "lib\libcurl\bin\libcurl.dll" "bin" /c /e /r /y
xcopy "lib\libcurl\bin\libcurl.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libcurl\bin\libcurl.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libcurl\bin64\libcurl.dll" "bin\x64" /c /e /r /y

xcopy "lib\libcurl\bin\libeay32.dll" "bin" /c /e /r /y
xcopy "lib\libcurl\bin\libeay32.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libcurl\bin\libeay32.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libcurl\bin64\libeay32.dll" "bin\x64" /c /e /r /y

xcopy "lib\libcurl\bin\libssh2.dll" "bin" /c /e /r /y
xcopy "lib\libcurl\bin\libssh2.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libcurl\bin\libssh2.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libcurl\bin64\libssh2.dll" "bin\x64" /c /e /r /y

xcopy "lib\libcurl\bin\ssleay32.dll" "bin" /c /e /r /y
xcopy "lib\libcurl\bin\ssleay32.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\libcurl\bin\ssleay32.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\libcurl\bin64\ssleay32.dll" "bin\x64" /c /e /r /y

xcopy "lib\vld\bin\Win32\vld_x86.dll" "bin" /c /e /r /y
xcopy "lib\vld\bin\Win32\dbghelp.dll" "bin" /c /e /r /y
xcopy "lib\vld\bin\Win32\Microsoft.DTfW.DHL.manifest" "bin" /c /e /r /y
xcopy "lib\vld\bin\Win32\vld_x86.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\vld\bin\Win32\dbghelp.dll" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\vld\bin\Win32\Microsoft.DTfW.DHL.manifest" "bin\x86-ansicode" /c /e /r /y
xcopy "lib\vld\bin\Win32\vld_x86.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\vld\bin\Win32\dbghelp.dll" "bin\x86-unicode" /c /e /r /y
xcopy "lib\vld\bin\Win32\Microsoft.DTfW.DHL.manifest" "bin\x86-unicode" /c /e /r /y
xcopy "lib\vld\bin\Win64\vld_x64.dll" "bin\x64" /c /e /r /y
xcopy "lib\vld\bin\Win64\dbghelp.dll" "bin\x64" /c /e /r /y
xcopy "lib\vld\bin\Win64\Microsoft.DTfW.DHL.manifest" "bin\x64" /c /e /r /y

rmdir /s /q bin
mkdir bin
mkdir bin\x86-multibyte
mkdir bin\x86-unicode
mkdir bin\x64
mkdir bin\x86-multibyte\updater
mkdir bin\x86-unicode\updater
mkdir bin\x64\updater
mkdir bin\x86-multibyte\Languages
mkdir bin\x86-unicode\Languages
mkdir bin\x64\Languages



xcopy "..\bin\x86-multibyte\fxfile.exe"                  "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\fxfile-crash.dll"            "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\fxfile-keyhook.dll"          "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\fxfile-launcher.exe"         "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\libcharset-1.dll"            "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\libgcc_s_sjlj-1.dll"         "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\libgfl340.dll"               "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\libgfle340.dll"              "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\libiconv-2.dll"              "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\libwinpthread-1.dll"         "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\libxml2-2.dll"               "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\libxpr.dll"                  "bin\x86-multibyte" /c /r /y
xcopy "..\bin\x86-multibyte\zlib1.dll"                   "bin\x86-multibyte" /c /r /y

xcopy "..\bin\x86-multibyte\updater\dbghelp.dll"         "bin\x86-multibyte\updater" /c /r /y
xcopy "..\bin\x86-multibyte\updater\fxfile-updater.exe"  "bin\x86-multibyte\updater" /c /r /y
xcopy "..\bin\x86-multibyte\updater\libcurl.dll"         "bin\x86-multibyte\updater" /c /r /y
xcopy "..\bin\x86-multibyte\updater\libgcc_s_sjlj-1.dll" "bin\x86-multibyte\updater" /c /r /y
xcopy "..\bin\x86-multibyte\updater\libiconv-2.dll"      "bin\x86-multibyte\updater" /c /r /y
xcopy "..\bin\x86-multibyte\updater\libwinpthread-1.dll" "bin\x86-multibyte\updater" /c /r /y
xcopy "..\bin\x86-multibyte\updater\libxml2-2.dll"       "bin\x86-multibyte\updater" /c /r /y
xcopy "..\bin\x86-multibyte\updater\libxpr.dll"          "bin\x86-multibyte\updater" /c /r /y
xcopy "..\bin\x86-multibyte\updater\zlib1.dll"           "bin\x86-multibyte\updater" /c /r /y

xcopy "..\src\fxfile\Languages\*.xml"                    "bin\x86-multibyte\Languages" /c /e /r /y



xcopy "..\bin\x86-unicode\fxfile.exe"                    "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\fxfile-crash.dll"              "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\fxfile-keyhook.dll"            "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\fxfile-launcher.exe"           "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\libcharset-1.dll"              "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\libgcc_s_sjlj-1.dll"           "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\libgfl340.dll"                 "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\libgfle340.dll"                "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\libiconv-2.dll"                "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\libwinpthread-1.dll"           "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\libxml2-2.dll"                 "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\libxpr.dll"                    "bin\x86-unicode" /c /r /y
xcopy "..\bin\x86-unicode\zlib1.dll"                     "bin\x86-unicode" /c /r /y

xcopy "..\bin\x86-unicode\updater\dbghelp.dll"           "bin\x86-unicode\updater" /c /r /y
xcopy "..\bin\x86-unicode\updater\fxfile-updater.exe"    "bin\x86-unicode\updater" /c /r /y
xcopy "..\bin\x86-unicode\updater\libcurl.dll"           "bin\x86-unicode\updater" /c /r /y
xcopy "..\bin\x86-unicode\updater\libgcc_s_sjlj-1.dll"   "bin\x86-unicode\updater" /c /r /y
xcopy "..\bin\x86-unicode\updater\libiconv-2.dll"        "bin\x86-unicode\updater" /c /r /y
xcopy "..\bin\x86-unicode\updater\libwinpthread-1.dll"   "bin\x86-unicode\updater" /c /r /y
xcopy "..\bin\x86-unicode\updater\libxml2-2.dll"         "bin\x86-unicode\updater" /c /r /y
xcopy "..\bin\x86-unicode\updater\libxpr.dll"            "bin\x86-unicode\updater" /c /r /y
xcopy "..\bin\x86-unicode\updater\zlib1.dll"             "bin\x86-unicode\updater" /c /r /y

xcopy "..\src\fxfile\Languages\*.xml"                    "bin\x86-unicode\Languages" /c /e /r /y



xcopy "..\bin\x64\fxfile.exe"                           "bin\x64" /c /r /y
xcopy "..\bin\x64\fxfile-crash.dll"                     "bin\x64" /c /r /y
xcopy "..\bin\x64\fxfile-keyhook.dll"                   "bin\x64" /c /r /y
xcopy "..\bin\x64\fxfile-launcher.exe"                  "bin\x64" /c /r /y
xcopy "..\bin\x64\libcharset-1.dll"                     "bin\x64" /c /r /y
xcopy "..\bin\x64\libgcc_s_sjlj-1.dll"                  "bin\x64" /c /r /y
xcopy "..\bin\x64\libgfl340.dll"                        "bin\x64" /c /r /y
xcopy "..\bin\x64\libgfle340.dll"                       "bin\x64" /c /r /y
xcopy "..\bin\x64\libiconv-2.dll"                       "bin\x64" /c /r /y
xcopy "..\bin\x64\libwinpthread-1.dll"                  "bin\x64" /c /r /y
xcopy "..\bin\x64\libxml2-2.dll"                        "bin\x64" /c /r /y
xcopy "..\bin\x64\libxpr.dll"                           "bin\x64" /c /r /y
xcopy "..\bin\x64\zlib1.dll"                            "bin\x64" /c /r /y

xcopy "..\bin\x64\updater\dbghelp.dll"                  "bin\x64\updater" /c /r /y
xcopy "..\bin\x64\updater\fxfile-updater.exe"           "bin\x64\updater" /c /r /y
xcopy "..\bin\x64\updater\libcurl.dll"                  "bin\x64\updater" /c /r /y
xcopy "..\bin\x64\updater\libgcc_s_sjlj-1.dll"          "bin\x64\updater" /c /r /y
xcopy "..\bin\x64\updater\libiconv-2.dll"               "bin\x64\updater" /c /r /y
xcopy "..\bin\x64\updater\libwinpthread-1.dll"          "bin\x64\updater" /c /r /y
xcopy "..\bin\x64\updater\libxml2-2.dll"                "bin\x64\updater" /c /r /y
xcopy "..\bin\x64\updater\libxpr.dll"                   "bin\x64\updater" /c /r /y
xcopy "..\bin\x64\updater\zlib1.dll"                    "bin\x64\updater" /c /r /y

xcopy "..\src\fxfile\Languages\*.xml"                   "bin\x64\Languages" /c /e /r /y

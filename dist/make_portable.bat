set version=2.0.4-dev

cd bin

cd x64
..\..\7zip\7za.exe a ../../fxfile_v%version%_x64_portable.zip *

cd ..

cd x86-unicode
..\..\7zip\7za.exe a ../../fxfile_v%version%_x86_unicode_portable.zip *

cd ..

cd x86-multibyte
..\..\7zip\7za.exe a ../../fxfile_v%version%_x86_multibyte_portable.zip *

exit

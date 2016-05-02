::
:: Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
::
::   homepage: http://www.flychk.com
::   e-mail:   mailto:flychk@flychk.com
::
:: Use of this source code is governed by a GPLv3 license that can be
:: found in the LICENSE file.
@echo off

set PATH=%~dp0tools\python_274;%PATH%

set msvs_version=2015
set target_arch=x64

if not %1/==/ set msvs_version=%1
if not %2/==/ set target_arch=%2

@echo msvs_version = %msvs_version%
@echo target_arch = %target_arch%
@echo ----------

call tools\gyp\gyp.bat fxfile.gyp --depth=. -G msvs_version=%msvs_version% -D target_arch=%target_arch% -D msvs_version=%msvs_version%

;NSIS Modern User Interface version 2.0 rc4
;Written by Leon Lee

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;Product Info
  !define PRODUCT_NAME "flyExplorer"
  !define PRODUCT_VER "v2.0.0 alpha2"
  Name "${PRODUCT_NAME} ${PRODUCT_VER}"

;--------------------------------
;Configuration

  ;General
  OutFile "flyExplorer_v2.0.0_alpha2.exe"

  ;Folder selection page
  InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
  
  ;Remember install folder
  InstallDirRegKey HKCU "Software\${PRODUCT_NAME}" ""

  ;$9 is being used to store the Start Menu Folder.
  ;Do not use this variable in your script (or Push/Pop it)!

  ;To change this variable, use MUI_STARTMENUPAGE_VARIABLE.
  ;Have a look at the Readme for info about other options (default folder,
  ;registry).

  !define TEMP $R0

  RequestExecutionLevel admin

;--------------------------------
;Variables

  Var MUI_TEMP
  Var STARTMENU_FOLDER
  Var INI_VALUE1
  Var INI_VALUE2
  Var UN_INI_VALUE1
  Var UN_INI_VALUE2
  Var PLATFORM

;--------------------------------
;Modern UI Configuration

  !define MUI_ABORTWARNING
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_RIGHT

  !define MUI_UI "${NSISDIR}\Contrib\UIs\modern.exe"
  !define MUI_WELCOMEFINISHPAGE_BITMAP "wizard2.bmp"
  !define MUI_HEADERIMAGE_BITMAP "header2.bmp"
;  !define MUI_SPECIALBITMAP "${NSISDIR}\Contrib\Graphics\Wizard\arrow.bmp"
;  !define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
;  !define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
  !define MUI_ICON "install2.ico"
  !define MUI_UNICON "uninstall2.ico"
  !define MUI_COMPONENTSPAGE_SMALLDESC
  !define MUI_LICENSEPAGE_CHECKBOX
  !define MUI_WELCOMEPAGE_TITLE "${PRODUCT_NAME} ${PRODUCT_VER} 설치를 시작합니다."
  !define MUI_WELCOMEPAGE_TEXT "이 프로그램은 사용자 컴퓨터에 ${PRODUCT_NAME} ${PRODUCT_VER}(을)를 설치할 것입니다.\r\n\r\n설치를 시작하기 전에 flyExplorer를 종료하여 주시기 바랍니다. 설치를 시작하면 강제 종료됩니다.\r\n\r\n계속하시려면 '다음' 버튼을 눌러 주세요."
  !define MUI_FINISHPAGE_TITLE "${PRODUCT_NAME} ${PRODUCT_VER} 설치 완료"
  !define MUI_FINISHPAGE_TEXT "${PRODUCT_NAME} ${PRODUCT_VER}의 설치가 완료되었습니다. 설치 프로그램을 마치려면 '마침' 버튼을 눌러 주세요."
  !define MUI_FINISHPAGE_RUN_TEXT "${PRODUCT_NAME} 실행하기(&R)"
  !define MUI_FINISHPAGE_SHOWREADME_TEXT "Readme 파일 보기(&S)"

;--------------------------------
;Pages
    !define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME}"
    !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
    !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${PRODUCT_NAME}"
    !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

    !define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
    !define MUI_FINISHPAGE_RUN "$INSTDIR\flyExplorer.exe"
    
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "License.txt"
  Page custom OptionPage
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  UninstPage custom un.RemoveOptionPage
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Modern UI Configuration

;  !define MUI_LICENSEPAGE
;  !define MUI_COMPONENTSPAGE
;  !define MUI_DIRECTORYPAGE
;  !define MUI_STARTMENUPAGE

;  !define MUI_ABORTWARNING
  
;  !define MUI_UNINSTALLER
;  !define MUI_UNCONFIRMPAGE
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "Korean"

;--------------------------------
;Language Strings

  ;Description
;  LangString DESC_SecCopyUI ${LANG_KOREAN} "Copy the modern.exe file to the application folder."
  LangString DESC_DTSC ${LANG_KOREAN} "flyExplorer의 바로 가기를 바탕 화면에 생성합니다."
  LangString DESC_QLSC ${LANG_KOREAN} "flyExplorer의 바로 가기를 빠른 실행에 생성합니다."
  LangString TEXT_NOTICE_TITLE ${LANG_KOREAN} "주의 사항"
  LangString TEXT_NOTICE_SUBTITLE ${LANG_KOREAN} "이전 버전 사용자만 해당되는 주의 사항입니다."
  LangString TEXT_OPTION_TITLE ${LANG_KOREAN} "코드 버전 선택"
  LangString TEXT_OPTION_SUBTITLE ${LANG_KOREAN} "설치하고자 하는 코드 버전을 선택하여 주십시오."
  LangString TEXT_UN_OPTION_TITLE ${LANG_KOREAN} "추가 제거 옵션"
  LangString TEXT_UN_OPTION_SUBTITLE ${LANG_KOREAN} "추가적으로 제거할 옵션을 선택하십시오."

;--------------------------------
;Reserve Files
  
  ;These files should be inserted before other files in the data block
  ;Keep these lines before any File command
  ;Only for solid compression (by default, solid compression is enabled for BZIP2 and LZMA)
  
  ReserveFile "option.ini"
  !insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

;--------------------------------
;Data

  LicenseData "License.txt"

;--------------------------------
;Installer Sections

Section "바탕 화면에 바로 가기 만들기" DTSC
   CreateShortCut "$DESKTOP\flyExplorer.lnk" "$INSTDIR\flyExplorer.exe"
SectionEnd

Section "빠른 실행에 바로 가기 만들기" QLSC
   CreateShortCut "$QUICKLAUNCH\flyExplorer.lnk" "$INSTDIR\flyExplorer.exe"
SectionEnd

Section "" SecCopyUI

  ;ADD YOUR OWN STUFF HERE!
  SetShellVarContext all

  SetOutPath "$INSTDIR"
  KillProcDLL::KillProc "flyExplorer.exe"
  KillProcDLL::KillProc "fxLauncher.exe"

  inFindProcflyExplorerLoop:
    FindProcDLL::FindProc "flyExplorer.exe"

    IntCmp $R0 0 inFindProcflyExplorerLoopDone
    goto inFindProcflyExplorerLoop
  inFindProcflyExplorerLoopDone:

  inFindProcfxLauncherLoop:
    FindProcDLL::FindProc "fxLauncher.exe"

    IntCmp $R0 0 inFindProcfxLauncherLoopDone
    goto inFindProcfxLauncherLoop
  inFindProcfxLauncherLoopDone:

  SetOverwrite on
  
  File /r "bin\*.*"
  File "flyExplorer.chm"
  File "flychk.url"
  File "custom_image_list_16.bmp"
  File "custom_image_list_32.bmp"
  File "license.txt"
  File "readme.txt"
  File "history.txt"

  ; Store install folder
  DeleteRegKey HKCU "Software\flyExplorer\flyExplorer"

  ; for Add / Remove Programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "DisplayName"     "flyExplorer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "DisplayVersion"  "${PRODUCT_NAME}"
  ;WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "VersionMajor"    "1"
  ;WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "VersionMinor"    "90"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "HelpLink"        "http://flychk.com"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "Publisher"       "flychk (Leon Lee)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "URLInfoAbout"    "http://flychk.com"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer" "URLUpdateInfo"   "http://flychk.com"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create Directory / Shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\flyExplorer.lnk" "$INSTDIR\flyExplorer.exe"
    CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\flyExplorer 도움말.lnk" "$INSTDIR\flyExplorer.chm"
    CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\fxLauncher.lnk" "$INSTDIR\fxLauncher.exe"
    CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\flychk WebSite.lnk" "$INSTDIR\flychk.url"
    CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\Readme.lnk" "$INSTDIR\readme.txt"
    CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\History.lnk" "$INSTDIR\history.txt"
    CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\License.lnk" "$INSTDIR\license.txt"
    CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Read a value from an InstallOptions INI file
  !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE1 "option.ini" "Field 2" "State"
  !insertmacro MUI_INSTALLOPTIONS_READ $INI_VALUE2 "option.ini" "Field 3" "State"
  
  StrCmp $INI_VALUE1 "1" "" +11
    Delete "$INSTDIR\flyExplorer.exe"
    Delete "$INSTDIR\libgfl340.dll"
    Delete "$INSTDIR\libgfle340.dll"
    Rename "$INSTDIR\flyExplorerU.exe" "$INSTDIR\flyExplorer.exe"
    Rename "$INSTDIR\libgfl340U.dll" "$INSTDIR\libgfl340.dll"
    Rename "$INSTDIR\libgfle340U.dll" "$INSTDIR\libgfle340.dll"
    Delete "$INSTDIR\flyExplorerA.exe"
    Delete "$INSTDIR\libgfl340A.dll"
    Delete "$INSTDIR\libgfle340A.dll"
    Delete "$INSTDIR\fxcrash.dll"

  StrCmp $INI_VALUE2 "1" "" +11
    Delete "$INSTDIR\flyExplorer.exe"
    Delete "$INSTDIR\libgfl340.dll"
    Delete "$INSTDIR\libgfle340.dll"
    Rename "$INSTDIR\flyExplorerA.exe" "$INSTDIR\flyExplorer.exe"
    Rename "$INSTDIR\libgfl340A.dll" "$INSTDIR\libgfl340.dll"
    Rename "$INSTDIR\libgfle340A.dll" "$INSTDIR\libgfle340.dll"
    Delete "$INSTDIR\flyExplorerU.exe"
    Delete "$INSTDIR\libgfl340U.dll"
    Delete "$INSTDIR\libgfle340U.dll"
    Delete "$INSTDIR\fxcrashu.dll"

SectionEnd

;--------------------------------
;Installer Functions
Function .onInit

  ;Extract InstallOptions INI files
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "option.ini"
  
FunctionEnd

Function .onInstSuccess

  ExecShell "" "$INSTDIR\fxLauncher.exe"
  
FunctionEnd
 
Function un.onInit

  ;Extract InstallOptions INI files
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "option_uninstall.ini"
  
FunctionEnd

Function OptionPage

  Call GetWindowsVersion
  
  StrCmp $PLATFORM "9x" "" +4
    !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 2" "State" "0"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 3" "State" "1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 2" "Flags" "DISABLED"
  
  !insertmacro MUI_HEADER_TEXT "$(TEXT_OPTION_TITLE)" "$(TEXT_OPTION_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "option.ini"

FunctionEnd

Function un.RemoveOptionPage

  !insertmacro MUI_HEADER_TEXT "$(TEXT_UN_OPTION_TITLE)" "$(TEXT_UN_OPTION_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "option_uninstall.ini"

FunctionEnd

Function GetWindowsVersion
 
   ReadRegStr $PLATFORM HKLM \
   "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

   IfErrors 0 lbl_winnt
   
   ; we are not NT
   ReadRegStr $PLATFORM HKLM \
   "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber
 
   StrCpy $PLATFORM '9x'
   Goto lbl_done
 
   lbl_winnt:
   StrCpy $PLATFORM 'NT'
 
   lbl_done:
 
FunctionEnd

;Display the Finish header
;Insert this macro after the sections if you are not using a finish page
;!insertmacro MUI_SECTIONS_FINISHHEADER

;--------------------------------
;Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${DTSC} $(DESC_DTSC)
  !insertmacro MUI_DESCRIPTION_TEXT ${QLSC} $(DESC_QLSC)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;all users
  SetShellVarContext all
  
  KillProcDLL::KillProc "flyExplorer.exe"
  KillProcDLL::KillProc "fxLauncher.exe"

  unFindProcflyExplorerLoop:
    FindProcDLL::FindProc "flyExplorer.exe"

    IntCmp $R0 0 unFindProcflyExplorerLoopDone
    goto unFindProcflyExplorerLoop
  unFindProcflyExplorerLoopDone:

  unFindProcfxLauncherLoop:
    FindProcDLL::FindProc "fxLauncher.exe"

    IntCmp $R0 0 unFindProcfxLauncherLoopDone
    goto unFindProcfxLauncherLoop
  unFindProcfxLauncherLoopDone:

  ;
  ;current user
  ;
  SetShellVarContext current
  
  Delete "$INSTDIR\flyExplorer.exe"
  Delete "$INSTDIR\flyExplorer.chm"
  Delete "$INSTDIR\fxcrashu.dll"
  Delete "$INSTDIR\fxcrash.dll"
  Delete "$INSTDIR\fxKeyHook.dll"
  Delete "$INSTDIR\fxLauncher.exe"
  Delete "$INSTDIR\libgfl340.dll"
  Delete "$INSTDIR\libgfle340.dll"
  Delete "$INSTDIR\iconv.dll"
  Delete "$INSTDIR\libxml2.dll"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$INSTDIR\custom_image_list_16.bmp"
  Delete "$INSTDIR\custom_image_list_32.bmp"
  Delete "$INSTDIR\flychk.url"
  Delete "$INSTDIR\readme.txt"
  Delete "$INSTDIR\history.txt"
  Delete "$INSTDIR\license.txt"
  RMDir /r "$INSTDIR\Languages"
  RMDir /r "$APPDATA\fxLauncher"

  ;Read a value from an InstallOptions INI file
  !insertmacro MUI_INSTALLOPTIONS_READ $UN_INI_VALUE1 "option_uninstall.ini" "Field 2" "State"
  !insertmacro MUI_INSTALLOPTIONS_READ $UN_INI_VALUE2 "option_uninstall.ini" "Field 4" "State"
  
  StrCmp $UN_INI_VALUE1 "1" "" +27
    Delete "$INSTDIR\coolbar.dat"
    Delete "$INSTDIR\toolbar.dat"
    Delete "$INSTDIR\barstate.ini"
    Delete "$INSTDIR\accel.dat"
    Delete "$INSTDIR\main.ini"
    Delete "$INSTDIR\config.ini"
    Delete "$INSTDIR\link.ini"
    Delete "$INSTDIR\filter.ini"
    Delete "$INSTDIR\fileclt.ini"
    Delete "$INSTDIR\fileclt_group.ini"
    Delete "$INSTDIR\thumbnail.dat"
    Delete "$INSTDIR\thumbnail.idx"
    RMDir /r "$INSTDIR\Settings"
    Delete "$APPDATA\coolbar.dat"
    Delete "$APPDATA\toolbar.dat"
    Delete "$APPDATA\barstate.ini"
    Delete "$APPDATA\accel.dat"
    Delete "$APPDATA\main.ini"
    Delete "$APPDATA\config.ini"
    Delete "$APPDATA\link.ini"
    Delete "$APPDATA\filter.ini"
    Delete "$APPDATA\fileclt.ini"
    Delete "$APPDATA\fileclt_group.ini"
    Delete "$APPDATA\thumbnail.dat"
    Delete "$APPDATA\thumbnail.idx"
    RMDir /r "$APPDATA\Settings"

  StrCmp $UN_INI_VALUE2 "1" "" +3
    RMDir /r "$INSTDIR"
    RMDir /r "$APPDATA\flyExplorer"

  ;
  ;all users
  ;
  SetShellVarContext all

  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP

    Delete "$SMPROGRAMS\$MUI_TEMP\flyExplorer.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\flyExplorer 도움말.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\fxLauncher.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\flyExplorer.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\flychk WebSite.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\readme.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\history.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\license.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
    Delete "$DESKTOP\flyExplorer.lnk"
    Delete "$QUICKLAUNCH\flyExplorer.lnk"
    RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"

  ;
  ;current users
  ;
  SetShellVarContext current
  
    Delete "$DESKTOP\flyExplorer.lnk"
    Delete "$QUICKLAUNCH\flyExplorer.lnk"

  ;Delete empty start menu parent diretories
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"

  startMenuDeleteLoop:
    RMDir /r $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
    
    IfErrors startMenuDeleteLoopDone
  
    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\flyExplorer"
  DeleteRegKey HKCU "Software\flyExplorer"
  DeleteRegKey HKCR "Folder\shell\flyExplorer로 열기"

  ;Display the Finish header
;  !insertmacro MUI_UNFINISHHEADER

SectionEnd

!include                  "MUI.nsh"

!define PRODUCT_NAME      "fxfile"
!define PRODUCT_MAJOR_VER "2"
!define PRODUCT_MINOR_VER "0.4-dev"
!define PRODUCT_VER       "v${PRODUCT_MAJOR_VER}.${PRODUCT_MINOR_VER}"

Name                      "${PRODUCT_NAME} ${PRODUCT_VER}"
OutFile                   "${PRODUCT_NAME}_${PRODUCT_VER}.exe"
InstallDir                "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKCU     "Software\${PRODUCT_NAME}" ""
LicenseData               "License.txt"
RequestExecutionLevel     admin
ReserveFile               "option.ini"
ReserveFile               "option_uninstall.ini"

!define TEMP $R0

Var MUI_TEMP
Var STARTMENU_FOLDER
Var OPT_32BIT_UNICODE
Var OPT_32BIT_MULTIBYTE
Var OPT_64BIT
Var UN_OPT_RMCONF
Var UN_OPT_RMALL
Var PLATFORM
Var PLATFORM_BIT

#--------------------------------
# Modern UI Configuration

!define MUI_UI                               "${NSISDIR}\Contrib\UIs\modern.exe"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP               "header2.bmp"
!define MUI_ICON                             "install2.ico"
!define MUI_UNICON                           "uninstall2.ico"

!define MUI_ABORTWARNING

!define MUI_COMPONENTSPAGE_SMALLDESC

!define MUI_LICENSEPAGE_CHECKBOX

!define MUI_WELCOMEPAGE_TITLE                "$(TEXT_WELCOMEPAGE_TITLE)"
!define MUI_WELCOMEFINISHPAGE_BITMAP         "wizard2.bmp"
!define MUI_WELCOMEPAGE_TEXT                 "$(TEXT_WELCOMEPAGE_TEXT)"

!define MUI_FINISHPAGE_TITLE                 "$(TEXT_FINISHPAGE_TITLE)"
!define MUI_FINISHPAGE_TEXT                  "$(TEXT_FINISHPAGE_TEXT)"
!define MUI_FINISHPAGE_RUN_TEXT              "$(TEXT_FINISHPAGE_RUN_TEXT)"
!define MUI_FINISHPAGE_RUN                   "$INSTDIR\fxfile.exe"
!define MUI_FINISHPAGE_SHOWREADME_TEXT       "$(TEXT_FINISHPAGE_SHOWREADME_TEXT)"
!define MUI_FINISHPAGE_SHOWREADME            "$INSTDIR\readme.txt"

!define MUI_STARTMENUPAGE_DEFAULTFOLDER      "${PRODUCT_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT      "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY       "Software\${PRODUCT_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

#--------------------------------
# Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "License.txt"
Page custom OptionEnterPage OptionLeavePage
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
UninstPage custom un.RemoveOptionEnterPage un.RemoveOptionLeavePage
!insertmacro MUI_UNPAGE_INSTFILES
 
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

#--------------------------------
# Language Strings

#!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Korean"

LangString TEXT_WELCOMEPAGE_TITLE          ${LANG_KOREAN} "${PRODUCT_NAME} ${PRODUCT_VER} ��ġ�� �����մϴ�."
LangString TEXT_WELCOMEPAGE_TEXT           ${LANG_KOREAN} "�� ���α׷��� ����� ��ǻ�Ϳ� ${PRODUCT_NAME} ${PRODUCT_VER}(��)�� ��ġ�� ���Դϴ�.\r\n\r\n��ġ�� �����ϱ� ���� fxfile�� �����Ͽ� �ֽñ� �ٶ��ϴ�. ��ġ�� �����ϸ� ���� ����˴ϴ�.\r\n\r\n����Ͻ÷��� '����' ��ư�� ���� �ּ���."

LangString TEXT_OPTION_HEADER              ${LANG_KOREAN} "���� ����"
LangString TEXT_OPTION_HEADER_DESC         ${LANG_KOREAN} "��ġ�ϰ��� �ϴ� ������ �����Ͽ� �ֽʽÿ�."
LangString TEXT_OPTION_TEXT                ${LANG_KOREAN} "64bit ������ �������� 64bit�� ��쿡�� ��ġ �����ϰ� 32bit Unicode ������ �������� NT �迭, 32bit Multibyte�� ��� �����쿡�� ��ġ �����մϴ�. �������� NT, 2000, XP, 2003, Vista, 2008, 7, 8, 2012, 8.1 �̻󿡼��� 64bit �Ǵ� 32bit Unicode ������ �����մϴ�."

LangString TEXT_DESKTOP_SHORTCUT           ${LANG_KOREAN} "���� ȭ�鿡 �ٷ� ���� �����"
LangString TEXT_QUICKLAUNCH_SHORTCUT       ${LANG_KOREAN} "���� ���࿡ �ٷ� ���� �����"
LangString TEXT_DESC_DESKTOP_SHORTCUT      ${LANG_KOREAN} "fxfile�� �ٷ� ���⸦ ���� ȭ�鿡 �����մϴ�."
LangString TEXT_DESC_QUICKLAUNCH_SHORTCUT  ${LANG_KOREAN} "fxfile�� �ٷ� ���⸦ ���� ���࿡ �����մϴ�."

LangString TEXT_FINISHPAGE_TITLE           ${LANG_KOREAN} "${PRODUCT_NAME} ${PRODUCT_VER} ��ġ �Ϸ�"
LangString TEXT_FINISHPAGE_TEXT            ${LANG_KOREAN} "${PRODUCT_NAME} ${PRODUCT_VER}�� ��ġ�� �Ϸ�Ǿ����ϴ�. ��ġ ���α׷��� ��ġ���� '��ħ' ��ư�� ���� �ּ���."
LangString TEXT_FINISHPAGE_RUN_TEXT        ${LANG_KOREAN} "${PRODUCT_NAME} �����ϱ�(&R)"
LangString TEXT_FINISHPAGE_SHOWREADME_TEXT ${LANG_KOREAN} "Readme ���� ����(&S)"

LangString TEXT_UN_OPTION_HEADER           ${LANG_KOREAN} "�߰� ���� �ɼ�"
LangString TEXT_UN_OPTION_HEADER_DESC      ${LANG_KOREAN} "�߰����� ���� �ɼ��� �����Ͻʽÿ�."
LangString TEXT_UN_OPTION_CHECK_CONF       ${LANG_KOREAN} "ȯ�� ���� ���� ��� ����(&C)"
LangString TEXT_UN_OPTION_CHECK_INSTDIR    ${LANG_KOREAN} "��ġ�� ���丮 ��� ����(&D)"
LangString TEXT_UN_OPTION_SUBTITLE         ${LANG_KOREAN} "�߰������� ������ �ɼ��� �����Ͻʽÿ�."

!macro GET_WINDOWS_VERSION_FUNCTION_MACRO un
Function ${un}GetWindowsVersion
 
    #
    # check 9x or NT
    #
    ReadRegStr $PLATFORM HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion

    IfErrors 0 WinNT
        ReadRegStr $PLATFORM HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion" VersionNumber

        StrCpy $PLATFORM '9x'
        Goto EndCheckWin98NT
    WinNT:
        StrCpy $PLATFORM 'NT'
    EndCheckWin98NT:

    #
    # check 64bit
    #
    System::Call "kernel32::GetCurrentProcess() i .s"
    System::Call "kernel32::IsWow64Process(i s, *i .r0)"
    StrCmp $0 '0' Win32 Win64
    Win32:
        StrCpy $PLATFORM_BIT '32'
        Goto EndCheckWin3264
    Win64:
        StrCpy $PLATFORM_BIT '64'
    EndCheckWin3264:
    
FunctionEnd
!macroend

!insertmacro GET_WINDOWS_VERSION_FUNCTION_MACRO ""
!insertmacro GET_WINDOWS_VERSION_FUNCTION_MACRO "un."

Function .onInit

    Call GetWindowsVersion

    # Select language
    !insertmacro MUI_LANGDLL_DISPLAY
    
    # Extract InstallOptions INI files
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT "option.ini"

FunctionEnd

Function .onInstSuccess

    ExecShell "" "$INSTDIR\fxfile-launcher.exe"
  
FunctionEnd

Function OptionEnterPage

    # Set default option value
    !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 1" "Text" "$(TEXT_OPTION_TEXT)"
    
    StrCmp $PLATFORM_BIT "64" Opt64 Opt32
    Opt64:
        !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 2" "State" "1"
        !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 3" "State" "0"
        !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 4" "State" "0"
        Goto EndOptPlatform
    Opt32:
        !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 2" "Flags" "DISABLED"
        StrCmp $PLATFORM "9x" "" +4
            !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 3" "State" "0"
            !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 4" "State" "1"
            !insertmacro MUI_INSTALLOPTIONS_WRITE "option.ini" "Field 3" "Flags" "DISABLED"
    EndOptPlatform:

    !insertmacro MUI_HEADER_TEXT "$(TEXT_OPTION_HEADER)" "$(TEXT_OPTION_HEADER_DESC)"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "option.ini"

FunctionEnd

Function OptionLeavePage

    # Read option value
    !insertmacro MUI_INSTALLOPTIONS_READ $OPT_64BIT           "option.ini" "Field 2" "State"
    !insertmacro MUI_INSTALLOPTIONS_READ $OPT_32BIT_UNICODE   "option.ini" "Field 3" "State"
    !insertmacro MUI_INSTALLOPTIONS_READ $OPT_32BIT_MULTIBYTE "option.ini" "Field 4" "State"

    #
    # Set install directory
    #
    StrCmp $OPT_64BIT "1" InstallDir64 InstallDir32
    InstallDir64:
        StrCpy $INSTDIR "$PROGRAMFILES64\${PRODUCT_NAME}"
        SetRegView 64
        Goto EndInstallDir
    InstallDir32:
        StrCpy $INSTDIR "$PROGRAMFILES\${PRODUCT_NAME}"
    EndInstallDir:

FunctionEnd

Section "$(TEXT_DESKTOP_SHORTCUT)" DESKTOP_SHORTCUT
   CreateShortCut "$DESKTOP\fxfile.lnk" "$INSTDIR\fxfile.exe"
SectionEnd

Section "$(TEXT_QUICKLAUNCH_SHORTCUT)" QUICKLAUNCH_SHORTCUT
   CreateShortCut "$QUICKLAUNCH\fxfile.lnk" "$INSTDIR\fxfile.exe"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${DESKTOP_SHORTCUT}     $(TEXT_DESC_DESKTOP_SHORTCUT)
    !insertmacro MUI_DESCRIPTION_TEXT ${QUICKLAUNCH_SHORTCUT} $(TEXT_DESC_QUICKLAUNCH_SHORTCUT)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "" SecCopyUI

    SetShellVarContext all

    SetOutPath "$INSTDIR"

    #
    # Kill processes
    #
    FindProcDLL::KillProc "fxfile.exe"
    FindProcDLL::KillProc "fxfile-launcher.exe"
    FindProcDLL::KillProc "fxfile-updater.exe"

    FindProcLoopfxfile:
        FindProcDLL::FindProc "fxfile.exe"

        IntCmp $R0 0 FindProcLoopfxfileDone
        Goto FindProcLoopfxfile
    FindProcLoopfxfileDone:

    FindProcLoopfxfilelauncher:
        FindProcDLL::FindProc "fxfile-launcher.exe"

        IntCmp $R0 0 FindProcLoopfxfilelauncherDone
        Goto FindProcLoopfxfilelauncher
    FindProcLoopfxfilelauncherDone:

    FindProcLoopfxfileupdater:
        FindProcDLL::FindProc "fxfile-updater.exe"

        IntCmp $R0 0 FindProcLoopfxfileupdaterDone
        Goto FindProcLoopfxfileupdater
    FindProcLoopfxfileupdaterDone:

    SetOverwrite on
  
    File /r "bin\*.*"
    File "fxfile.chm"
    File "flychk.url"
    File "custom_image_list_16.bmp"
    File "custom_image_list_32.bmp"
    File "license.txt"
    File "readme.txt"
    File "history.txt"

    StrCmp $OPT_32BIT_UNICODE "1" "" +2
        CopyFiles /silent "$INSTDIR\x86-unicode\*.*" "$INSTDIR"

    StrCmp $OPT_32BIT_MULTIBYTE "1" "" +2
        CopyFiles /silent "$INSTDIR\x86-multibyte\*.*" "$INSTDIR"

    StrCmp $OPT_64BIT "1" "" +2
        CopyFiles /silent "$INSTDIR\x64\*.*" "$INSTDIR"

    RMDir /r "$INSTDIR\x64"
    RMDir /r "$INSTDIR\x86-unicode"
    RMDir /r "$INSTDIR\x86-multibyte"

    # Store install folder
    DeleteRegKey HKCU "Software\fxfile\fxfile"

    # for Add / Remove Programs
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "DisplayName"     "${PRODUCT_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "DisplayVersion"  "${PRODUCT_MAJOR_VER}.${PRODUCT_MINOR_VER}"
    #WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "VersionMajor"    "${PRODUCT_MAJOR_VER}"
    #WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "VersionMinor"    "${PRODUCT_MINOR_VER}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "HelpLink"        "http://flychk.com"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "Publisher"       "flychk (Leon Lee)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "URLInfoAbout"    "http://flychk.com"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile" "URLUpdateInfo"   "http://flychk.com"

    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
        # Create Directory / Shortcuts
        CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
        CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\fxfile.lnk" "$INSTDIR\fxfile.exe"
        CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\fxfile help.lnk" "$INSTDIR\fxfile.chm"
        CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\fxfile-launcher.lnk" "$INSTDIR\fxfile-launcher.exe"
        CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\flychk WebSite.lnk" "$INSTDIR\flychk.url"
        CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\Readme.lnk" "$INSTDIR\readme.txt"
        CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\History.lnk" "$INSTDIR\history.txt"
        CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\License.lnk" "$INSTDIR\license.txt"
        CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
    !insertmacro MUI_STARTMENU_WRITE_END
  
    # Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Function un.onInit

    # Extract InstallOptions INI files
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT "option_uninstall.ini"
  
FunctionEnd

Function un.RemoveOptionEnterPage

    # Set default option value
    !insertmacro MUI_INSTALLOPTIONS_WRITE "option_uninstall.ini" "Field 1" "Text" "$(TEXT_UN_OPTION_HEADER_DESC)"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "option_uninstall.ini" "Field 2" "Text" "$(TEXT_UN_OPTION_CHECK_CONF)"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "option_uninstall.ini" "Field 3" "Text" "$(TEXT_UN_OPTION_CHECK_INSTDIR)"

    !insertmacro MUI_HEADER_TEXT "$(TEXT_UN_OPTION_HEADER)" "$(TEXT_UN_OPTION_SUBTITLE)"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "option_uninstall.ini"
    
FunctionEnd

Function un.RemoveOptionLeavePage

FunctionEnd

Section "Uninstall"

    # all users
    SetShellVarContext all

    #
    # Kill processes
    #
    FindProcDLL::KillProc "fxfile.exe"
    FindProcDLL::KillProc "fxfile-launcher.exe"
    FindProcDLL::KillProc "fxfile-updater.exe"

    unFindProcLoopfxfile:
        FindProcDLL::FindProc "fxfile.exe"

        IntCmp $R0 0 unFindProcLoopfxfileDone
        goto unFindProcLoopfxfile
    unFindProcLoopfxfileDone:

    unFindProcLoopLauncher:
        FindProcDLL::FindProc "fxfile-launcher.exe"

        IntCmp $R0 0 unFindProcLoopLauncherDone
        goto unFindProcLoopLauncher
    unFindProcLoopLauncherDone:

    unFindProcLoopUpdater:
        FindProcDLL::FindProc "fxfile-updater.exe"

        IntCmp $R0 0 unFindProcLoopUpdaterDone
        goto unFindProcLoopUpdater
    unFindProcLoopUpdaterDone:
    
    #
    # current user
    #
    SetShellVarContext current

    Delete "$INSTDIR\fxfile.exe"
    Delete "$INSTDIR\fxfile.chm"
    Delete "$INSTDIR\fxfile-crash.dll"
    Delete "$INSTDIR\fxfile-keyhook.dll"
    Delete "$INSTDIR\fxfile-launcher.exe"
    Delete "$INSTDIR\libcharset-1.dll"
    Delete "$INSTDIR\libgcc_s_sjlj-1.dll"
    Delete "$INSTDIR\libgfl340.dll"
    Delete "$INSTDIR\libgfle340.dll"
    Delete "$INSTDIR\libiconv-2.dll"
    Delete "$INSTDIR\libwinpthread-1.dll"
    Delete "$INSTDIR\libxml2-2.dll"
    Delete "$INSTDIR\libxpr.dll"
    Delete "$INSTDIR\zlib1.dll"
    Delete "$INSTDIR\Uninstall.exe"
    Delete "$INSTDIR\custom_image_list_16.bmp"
    Delete "$INSTDIR\custom_image_list_32.bmp"
    Delete "$INSTDIR\flychk.url"
    Delete "$INSTDIR\readme.txt"
    Delete "$INSTDIR\history.txt"
    Delete "$INSTDIR\license.txt"
    RMDir /r "$INSTDIR\Languages"
    RMDir /r "$INSTDIR\updater"
    RMDir /r "$APPDATA\fxfile-launcher"

    # Read a value from an InstallOptions INI file
    !insertmacro MUI_INSTALLOPTIONS_READ $UN_OPT_RMCONF "option_uninstall.ini" "Field 2" "State"
    !insertmacro MUI_INSTALLOPTIONS_READ $UN_OPT_RMALL  "option_uninstall.ini" "Field 3" "State"

    StrCmp $UN_OPT_RMCONF "1" "" +7
        Delete   "$INSTDIR\.fxfile"
        RMDir /r "$INSTDIR\conf"
        RMDir /r "$INSTDIR\reports"
        RMDir /r "$INSTDIR\update"
        Delete   "$APPDATA\.fxfile"
        RMDir /r "$APPDATA\fxfile\conf"
        RMDir /r "$APPDATA\fxfile\reports"
        RMDir /r "$APPDATA\fxfile\update"

    StrCmp $UN_OPT_RMALL "1" "" +3
        RMDir /r "$INSTDIR"
        RMDir /r "$APPDATA\fxfile"

    #
    # all users
    #
    SetShellVarContext all

    !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP

        Delete "$SMPROGRAMS\$MUI_TEMP\fxfile.lnk"
        Delete "$SMPROGRAMS\$MUI_TEMP\fxfile help.lnk"
        Delete "$SMPROGRAMS\$MUI_TEMP\fxfile-launcher.lnk"
        Delete "$SMPROGRAMS\$MUI_TEMP\fxfile.lnk"
        Delete "$SMPROGRAMS\$MUI_TEMP\flychk WebSite.lnk"
        Delete "$SMPROGRAMS\$MUI_TEMP\readme.lnk"
        Delete "$SMPROGRAMS\$MUI_TEMP\history.lnk"
        Delete "$SMPROGRAMS\$MUI_TEMP\license.lnk"
        Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
        Delete "$DESKTOP\fxfile.lnk"
        Delete "$QUICKLAUNCH\fxfile.lnk"
        RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"

    #
    # current users
    #
    SetShellVarContext current
  
        Delete "$DESKTOP\fxfile.lnk"
        Delete "$QUICKLAUNCH\fxfile.lnk"

    # Delete empty start menu parent diretories
    StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"

    startMenuDeleteLoop:
        RMDir /r $MUI_TEMP
        GetFullPathName $MUI_TEMP "$MUI_TEMP\.."

        IfErrors startMenuDeleteLoopDone

        StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
    startMenuDeleteLoopDone:

    SetRegView 32
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile"
    DeleteRegKey HKCU "Software\fxfile"
    DeleteRegKey HKCR "Folder\shell\fxfile�� ����"
    
    SetRegView 64
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\fxfile"
    DeleteRegKey HKCU "Software\fxfile"
    DeleteRegKey HKCR "Folder\shell\fxfile�� ����"

    # Display the Finish header
    #!insertmacro MUI_UNFINISHHEADER

SectionEnd

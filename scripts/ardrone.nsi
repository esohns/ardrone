; ardrone.nsi
;--------------------------------

; Includes
;!include "FileFunc.nsh"
;--------------------------------

!define PROGRAM "ardrone"
!define CONFIGURATION_SUBDIR "etc"

; Languages
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"

; The name of the installer
Name ${PROGRAM}

; The file to write
!searchparse /file "..\CMakeLists.txt" `set (VERSION_MAJOR ` VER_MAJOR `)`
!searchparse /file "..\CMakeLists.txt" `set (VERSION_MINOR ` VER_MINOR `)`
!searchparse /file "..\CMakeLists.txt" `set (VERSION_MICRO ` VER_MICRO `)`
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" ${PROGRAM}
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "a VR FPV drone client prototype"
;VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" ""
!define /date NOW "%Y"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© ${NOW}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PROGRAM} installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VER_MAJOR}.${VER_MINOR}.${VER_MICRO}.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VER_MAJOR}.${VER_MINOR}.${VER_MICRO}.0"
;VIAddVersionKey /LANG=${LANG_ENGLISH} "InternalName" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "PrivateBuild" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "SpecialBuild" ""
VIProductVersion "${VER_MAJOR}.${VER_MINOR}.${VER_MICRO}.0"
;DetailPrint 'VIProductVersion: "${VER_MAJOR}.${VER_MINOR}.${VER_MICRO}.0"'

OutFile "${PROGRAM}-${VER_MAJOR}.${VER_MINOR}.exe"

; The default installation directory
;InstallDir $DESKTOP\${PROGRAM}
InstallDir $PROGRAMFILES\${PROGRAM}

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\${PROGRAM}" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

; License
LicenseData "..\LICENSE"

; Options
AutoCloseWindow true
Icon "..\src\test_i\etc\ardrone_small.ico"
XPStyle on

;--------------------------------

; Pages

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; the stuff to install
Section "${PROGRAM} (required)"

SectionIn RO
  
; set output path to the installation directory
SetOutPath $INSTDIR

; put files there (application)
File "..\cmake\src\test_i\${release}\ardrone.exe"

; put files there (3rd party)
!if ${release} == "Debug"
File "D:\projects\ATCD\ACE\lib\ACEd.dll"
!else
File "D:\projects\ATCD\ACE\lib\ACE.dll"
!endif

File "D:\projects\ffmpeg\libavcodec\avcodec-57.dll"
File "D:\projects\ffmpeg\libavformat\avformat-57.dll"
File "D:\projects\ffmpeg\libavutil\avutil-55.dll"
File "D:\projects\ffmpeg\libswresample\swresample-2.dll"
File "D:\projects\ffmpeg\libswscale\swscale-4.dll"

File "D:\projects\gtk\gtk\Win32\debug\bin\atk-1.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\cairo.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\cairo-gobject.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\epoxy-0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\fontconfig.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\gdk_pixbuf-2.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\gio-2.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\glib-2.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\gmodule-2.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\gobject-2.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\gdk-3-3.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\gtk-3-3.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\harfbuzz.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\iconv.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\intl.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\libpng16.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\libxml2.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\pango-1.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\pangocairo-1.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\pangoft2-1.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\pangowin32-1.0.dll"
File "D:\projects\gtk\gtk\Win32\debug\bin\zlib1.dll"

;File "D:\projects\libglade\bin\libglade-2.0-0.dll"

;File "D:\software\Develop\libiconv-2.dll"
;File "D:\software\Develop\libxml2-2.dll"

File "..\..\libCommon\cmake\src\err\${release}\CommonError.dll"
File "..\..\libCommon\cmake\src\ui\${release}\CommonUI.dll"
File "..\..\libACEStream\cmake\src\modules\dec\${release}\ACEStream_Decoder.dll"
File "..\..\libACEStream\cmake\src\modules\dev\${release}\ACEStream_Device.dll"

; Config
; set output path to the installation directory
SetOutPath $INSTDIR\${CONFIGURATION_SUBDIR}

; Config - glade
File "..\src\test_i\etc\ardrone.glade"

; set output path to the installation directory
SetOutPath $INSTDIR\share\glib-2.0\schemas

; Config - GIO schemas
File "D:\projects\gtk\gtk\Win32\debug\share\glib-2.0\schemas\gschemas.compiled"
File "D:\projects\gtk\gtk\Win32\debug\share\glib-2.0\schemas\org.gtk.Settings.ColorChooser.gschema.xml"
File "D:\projects\gtk\gtk\Win32\debug\share\glib-2.0\schemas\org.gtk.Settings.Debug.gschema.xml"
File "D:\projects\gtk\gtk\Win32\debug\share\glib-2.0\schemas\org.gtk.Settings.FileChooser.gschema.xml"

; Write the installation path into the registry
WriteRegStr HKLM SOFTWARE\${PROGRAM} "Install_Dir" "$INSTDIR"

; Write the uninstall keys for Windows
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "DisplayName" "${PROGRAM}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "UninstallString" '"$INSTDIR\uninstall.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "NoRepair" 1
WriteUninstaller "uninstall.exe"

SectionEnd

;--------------------------------

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

CreateDirectory "$SMPROGRAMS\${PROGRAM}"
CreateShortCut "$SMPROGRAMS\${PROGRAM}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
CreateShortCut "$SMPROGRAMS\${PROGRAM}\${PROGRAM}.lnk" "$INSTDIR\${PROGRAM}.exe" "" "$INSTDIR\${PROGRAM}.exe" 0

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

; Auto-Close
SetAutoClose true

; Remove registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}"
DeleteRegKey HKLM SOFTWARE\${PROGRAM}

; Remove files AND uninstaller (yes this works !!!)
Delete "$INSTDIR\*.*"

; Remove shortcuts, if any
Delete "$SMPROGRAMS\${PROGRAM}\*.*"

; Remove directories used
RMDir /r "$INSTDIR"
RMDir "$SMPROGRAMS\${PROGRAM}"

SectionEnd

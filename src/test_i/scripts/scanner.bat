@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: scanner.bat
@rem #//
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@echo off
set RC=0
setlocal enabledelayedexpansion
pushd . >NUL 2>&1

@rem set FlexEXE="%ProgramFiles(x86)%\GnuWin32\bin\flex.exe"
set FlexEXE="C:\cygwin64\bin\flex.exe"
if exist %FlexEXE% goto Next
echo invalid file ^(was: "%FlexEXE%"^)^, exiting
goto Failed

:Next
%FlexEXE% --noline control_scanner.l 2>control_scanner_report_scanner.txt
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate scanner from control_scanner.l^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

%FlexEXE% --noline mavlink_scanner.l 2>mavlink_scanner_report_scanner.txt
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate scanner from mavlink_scanner.l^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

%FlexEXE% --noline navdata_scanner.l 2>navdata_scanner_report_scanner.txt
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate scanner from navdata_scanner.l^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

@move /Y ardrone_control_scanner.cpp .. >NUL
@move /Y ardrone_control_scanner.h .. >NUL
@move /Y ardrone_mavlink_scanner.cpp .. >NUL
@move /Y ardrone_mavlink_scanner.h .. >NUL
@move /Y ardrone_navdata_scanner.cpp .. >NUL
@move /Y ardrone_navdata_scanner.h .. >NUL
if %ERRORLEVEL% NEQ 0 (
 echo failed to move scanner file^(s^)^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

goto Clean_Up

:Failed
echo processing scanner...FAILED

:Clean_Up
popd
::endlocal & set RC=%ERRORLEVEL%
endlocal & set RC=%RC%
goto Error_Level

:Exit_Code
:: echo %ERRORLEVEL% %1 *WORKAROUND*
exit /b %1

:Error_Level
call :Exit_Code %RC%

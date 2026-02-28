@echo off
setlocal enabledelayedexpansion

set "COMPILER=compiler\solve_text_win.cpp"
set "BIN_DIR=bin"
set "BIN=%BIN_DIR%\solve_text_win.exe"

if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

if not exist "%BIN%" (
  echo [*] Dang compile decoder Windows...
  g++ -std=gnu++17 "%COMPILER%" -o "%BIN%"
  if errorlevel 1 (
    echo [!] Compile that bai.
    exit /b 1
  )
)

if "%~1"=="" goto usage
if "%~2"=="" goto usage

set "INPUT=%~1"
set "OUTPUT=%~2"

if not exist "%INPUT%" (
  echo [!] Khong tim thay file input: %INPUT%
  exit /b 1
)

echo [*] Dang giai ma: %INPUT% -> %OUTPUT%
"%BIN%" < "%INPUT%" > "%OUTPUT%"

if errorlevel 1 (
  echo [!] Giai ma that bai.
  exit /b 1
)

echo [v] Hoan tat.
exit /b 0

:usage
echo Cach dung:
echo   decode_win.bat ^<input_file^> ^<output_file^>
echo.
echo Vi du:
echo   decode_win.bat in.txt out.txt
exit /b 1

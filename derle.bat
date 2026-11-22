@echo off
rem Derler ve calistirir. g++ PATH'te yoksa WinLibs klasorunu kullanir.
cd /d "%~dp0"
where g++ >nul 2>nul || set "PATH=%LOCALAPPDATA%\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin;%PATH%"
rem -static: exe DLL istemez. Yoksa PATH'teki baska surum libstdc++ (ornegin Git'inki) yuklenebilir.
g++ -std=c++11 -Wall -Wextra -static main.cpp ortak.cpp modul1.cpp modul2.cpp modul3.cpp modul4.cpp -o kutuphane.exe
if errorlevel 1 (
  echo DERLEME HATASI
  pause
  exit /b 1
)
.\kutuphane.exe
pause

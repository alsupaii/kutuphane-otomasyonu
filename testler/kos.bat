@echo off
rem Tum testleri calistirir. Cift tikla ya da: testler\kos.bat
rem 1) Programi derler. 2) Her NN_*.txt girdisini programa verir.
rem 3) Ciktiyi beklenen\ klasorundeki onayli ciktiyla karsilastirir.
rem 4) Iki birim testini (siralama, liste-hash tutarliligi) derleyip calistirir.
rem Onayli ciktiyi yenilemek icin: kos.bat yenile
setlocal
set "MOD=%~1"
cd /d "%~dp0"
where g++ >nul 2>nul || set "PATH=%LOCALAPPDATA%\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin;%PATH%"

set KAYNAK=..\main.cpp ..\ortak.cpp ..\modul1.cpp ..\modul2.cpp ..\modul3.cpp ..\modul4.cpp
g++ -std=c++11 -Wall -Wextra -pedantic -static -I.. %KAYNAK% -o kutuphane_test.exe
if errorlevel 1 goto derlemeHatasi

if not exist cikti mkdir cikti
if not exist beklenen mkdir beklenen
if exist katalog.txt del katalog.txt
set HATA=0

for %%f in (0*.txt 1*.txt) do call :senaryo %%~nf
goto birimTestleri

:senaryo
if "%1"=="11_dosya_bozuk" copy /y bozuk_katalog.txt katalog.txt >nul
.\kutuphane_test.exe < %1.txt > cikti\%1.out
rem Program duzgun kapanmadiysa (cokme, exe yok) cikti eksiktir: bos ciktiyi bos beklenenle esit saymamak icin.
findstr /c:"Program kapandi." cikti\%1.out >nul
if errorlevel 1 (
  echo [HATA ] %1  ^(program duzgun kapanmadi^)
  set HATA=1
  exit /b 0
)
findstr /c:"sizinti yok" cikti\%1.out >nul
if errorlevel 1 (
  echo [HATA ] %1  ^(bellek sizintisi^)
  set HATA=1
  exit /b 0
)
if "%MOD%"=="yenile" copy /y cikti\%1.out beklenen\%1.out >nul
if not exist beklenen\%1.out (
  echo [HATA ] %1  ^(beklenen cikti yok; once: kos.bat yenile^)
  set HATA=1
  exit /b 0
)
fc beklenen\%1.out cikti\%1.out >nul 2>nul
if errorlevel 1 (
  echo [HATA ] %1  ^(cikti beklenenden farkli: fc beklenen\%1.out cikti\%1.out^)
  set HATA=1
) else (
  echo [TAMAM] %1
)
exit /b 0

:birimTestleri
g++ -std=c++11 -Wall -Wextra -static -I.. siralama_testi.cpp ..\ortak.cpp ..\modul1.cpp ..\modul2.cpp ..\modul3.cpp -o siralama_testi.exe
if errorlevel 1 goto derlemeHatasi
.\siralama_testi.exe
if errorlevel 1 set HATA=1
g++ -std=c++11 -Wall -Wextra -static -I.. tutarlilik_testi.cpp ..\ortak.cpp ..\modul1.cpp ..\modul2.cpp -o tutarlilik_testi.exe
if errorlevel 1 goto derlemeHatasi
.\tutarlilik_testi.exe
if errorlevel 1 set HATA=1
g++ -std=c++11 -Wall -Wextra -static -I.. koruma_testi.cpp ..\ortak.cpp ..\modul1.cpp ..\modul2.cpp ..\modul4.cpp -o koruma_testi.exe
if errorlevel 1 goto derlemeHatasi
.\koruma_testi.exe
if errorlevel 1 set HATA=1

echo.
if "%HATA%"=="0" (echo SONUC: tum testler gecti.) else (echo SONUC: HATA VAR, yukaridaki satirlara bakin.)
if "%1"=="" pause
exit /b %HATA%

:derlemeHatasi
echo DERLEME HATASI
if "%1"=="" pause
exit /b 1

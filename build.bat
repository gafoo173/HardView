@echo off
setlocal

REM --- Variables ---
set SRC=src
set OBJS=objs
set OUT_DLL=HardView.dll
set OUT_LIB=HardView.lib
set OUT_STATIC=HardView_static.lib
set DEF=HardView.def

echo === Building HardView library for Windows ===

REM --- Create objects directory if it doesn't exist ---
if not exist %OBJS% mkdir %OBJS%

REM --- Compile C source files ---
echo --- Compiling source files... ---
for %%f in (%SRC%\*.c) do (
    echo Compiling %%f...
    cl /c /nologo /Fo%OBJS%\ "%%f"
)

REM --- Create .def file if it doesn't exist ---
if not exist %DEF% (
    echo LIBRARY %OUT_DLL% > %DEF%
    echo EXPORTS >> %DEF%
)

REM --- Link DLL ---
echo --- Linking DLL... ---
link /DLL /NOLOGO /OUT:%OUT_DLL% /DEF:%DEF% /IMPLIB:%OUT_LIB% %OBJS%\*.obj ole32.lib oleaut32.lib wbemuuid.lib

REM --- Create static library ---
echo --- Creating static library... ---
lib /NOLOGO /OUT:%OUT_STATIC% %OBJS%\*.obj

echo.
echo === Build complete (Windows) ===
echo - %OUT_DLL%
echo - %OUT_LIB%
echo - %OUT_STATIC%
echo - %DEF%

endlocal

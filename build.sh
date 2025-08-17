#!/bin/bash
set -e

SRC="src"
OBJS="objs"
OUT_WIN_DLL="HardView.dll"
OUT_WIN_LIB="HardView.lib"
OUT_WIN_STATIC="HardView_static.lib"
OUT_LNX_SO="libHardView.so"
OUT_LNX_STATIC="libHardView.a"
DEF="HardView.def"

echo "=== Building HardView library ==="

mkdir -p "$OBJS"

UNAME=$(uname | tr '[:upper:]' '[:lower:]')

if [[ "$UNAME" == *"mingw"* || "$OS" == "Windows_NT" ]]; then
    echo "--- Detected Windows ---"

    # Windows-specific source files
    WIN_SRCS="Smart_disk.c advanced_storage_info.c baseboard_info.c bios_info.c chassis_info.c cpu_info.c disk_info.c gpu_info.c helpers.c network_info.c ram_info.c system_info.c win_helpers.c"

    for f in $WIN_SRCS; do
        echo "Compiling $f..."
        cl /c "/Fo${OBJS}\\" "$SRC/$f"
    done

    if [[ ! -f "$DEF" ]]; then
        echo "LIBRARY $OUT_WIN_DLL" > "$DEF"
        echo "EXPORTS" >> "$DEF"
    fi

    echo "--- Linking DLL ---"
    link /DLL /OUT:$OUT_WIN_DLL /DEF:$DEF /IMPLIB:$OUT_WIN_LIB $OBJS/*.obj ole32.lib oleaut32.lib wbemuuid.lib

    echo "--- Creating static library ---"
    lib /OUT:$OUT_WIN_STATIC $OBJS/*.obj

    echo "=== Build complete (Windows) ==="
    echo "- $OUT_WIN_DLL"
    echo "- $OUT_WIN_LIB"
    echo "- $OUT_WIN_STATIC"
    echo "- $DEF"

else
    echo "--- Detected Linux ---"

    # Linux-specific source files
    LNX_SRCS="advanced_storage_info.c baseboard_info.c bios_info.c chassis_info.c cpu_info.c disk_info.c gpu_info.c helpers.c linux_helpers.c network_info.c ram_info.c system_info.c"

    for f in $LNX_SRCS; do
        echo "Compiling $f..."
        gcc -c -fPIC "$SRC/$f" -o "$OBJS/$(basename "$f" .c).o"
    done

    echo "--- Linking shared library ---"
    gcc -shared $OBJS/*.o -o $OUT_LNX_SO

    echo "--- Creating static library ---"
    ar rcs $OUT_LNX_STATIC $OBJS/*.o

    echo "=== Build complete (Linux) ==="
    echo "- $OUT_LNX_SO"
    echo "- $OUT_LNX_STATIC"
fi

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

    for f in $SRC/*.c; do
        echo "Compiling $f..."
        cl /c "/Fo${OBJS}\\" "$f"
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

    for f in $SRC/*.c; do
        echo "Compiling $f..."
        gcc -c -fPIC "$f" -o "$OBJS/$(basename "$f" .c).o"
    done

    echo "--- Linking shared library ---"
    # Removed Windows-specific libraries. The || true is also removed so the script will
    # fail if a genuine linking error occurs.
    gcc -shared $OBJS/*.o -o $OUT_LNX_SO

    echo "--- Creating static library ---"
    ar rcs $OBJS/*.o -o $OUT_LNX_STATIC

    echo "=== Build complete (Linux) ==="
    echo "- $OUT_LNX_SO"
    echo "- $OUT_LNX_STATIC"
fi

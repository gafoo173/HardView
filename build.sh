name: Build HardView

on:
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:
    name: Build (${{ matrix.os }} - ${{ matrix.arch }})
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
        arch: [x86, x64]

    steps:
      - name: Checkout repo
        uses: actions/checkout@v4

      - name: Set up MSVC (Windows only)
        if: runner.os == 'Windows'
        uses: ilammy/msvc-dev-cmd@v1
        with:
          arch: ${{ matrix.arch }}

      - name: Install 32-bit gcc (Linux only, x86)
        if: runner.os == 'Linux' && matrix.arch == 'x86'
        run: sudo apt-get update && sudo apt-get install -y gcc-multilib g++-multilib

      - name: Run build script
        shell: bash
        run: |
          chmod +x build.sh
          ./build.sh

      # ===== Windows artifacts =====
      - name: Upload Windows Dynamic (DLL + import lib + DEF)
        if: runner.os == 'Windows'
        uses: actions/upload-artifact@v4
        with:
          name: hardview-windows-${{ matrix.arch }}-dynamic
          path: |
            HardView.dll
            HardView.lib
            HardView.def

      - name: Upload Windows Static (LIB)
        if: runner.os == 'Windows'
        uses: actions/upload-artifact@v4
        with:
          name: hardview-windows-${{ matrix.arch }}-static
          path: |
            HardView_static.lib

      # ===== Linux artifacts =====
      - name: Upload Linux Dynamic (.so)
        if: runner.os == 'Linux'
        uses: actions/upload-artifact@v4
        with:
          name: hardview-linux-${{ matrix.arch }}-dynamic
          path: |
            libHardView.so

      - name: Upload Linux Static (.a)
        if: runner.os == 'Linux'
        uses: actions/upload-artifact@v4
        with:
          name: hardview-linux-${{ matrix.arch }}-static
          path: |
            libHardView.a

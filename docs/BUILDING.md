# Building EVILWARE The Unholy Core 3 on Windows 10/11 x64

The official EVILWARE build uses a Windows GitHub Actions runner and Qt 6.7.3. The workflow compiles the C++20 Qt Widgets application, deploys the Qt runtime DLLs, and publishes a ZIP artifact.

## Local build
Install Visual Studio 2022 Build Tools (Desktop development with C++), CMake 3.21+, and Qt 6.5+ with a matching MSVC kit. Then run:
```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
```
The executable is `build/Release/EVILWARE.exe`. Run `windeployqt --release --compiler-runtime build/Release/EVILWARE.exe` from the matching Qt kit to deploy runtime dependencies.

WSL, Git and other external tools are optional dependencies and must be installed separately. The terminal executes commands as the logged-in Windows user.


## CI smoke test
The Windows workflow runs EVILWARE with the --self-test argument in Qt's offscreen platform. It constructs the main window and verifies that all 20 navigation items select their matching page. This is a construction/navigation smoke test, not an end-to-end test of every command or external dependency.

## Portable package
The Actions artifact contains the installer executable and the portable ZIP. The portable ZIP includes the deployed Qt runtime. Extract the ZIP completely before launching EVILWARE.exe; do not run it from inside the ZIP preview.

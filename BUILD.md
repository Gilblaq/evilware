# Building EVILWARE

## Supported build target
- Windows 10/11 x64.
- C++20.
- Qt 6.5+ (CI currently uses Qt 6.7.3).
- CMake 3.21+.
- A compatible MSVC or MinGW C++ toolchain.

## Local Windows build
Open a Visual Studio 2022 Developer PowerShell or a shell with the matching C++ compiler environment, then run:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
```

The executable is normally `build/Release/EVILWARE.exe` for the Visual Studio generator. Other generators may use a different output directory.

## Runtime deployment
From the same Qt kit used to compile:

```powershell
windeployqt --release --compiler-runtime build/Release/EVILWARE.exe
```

Do not mix Qt kits or compiler toolchains.

## Automated CI
Every push to `main` runs `.github/workflows/windows-build.yml`. It configures and builds the application, deploys Qt runtime files, runs the offscreen 20-page navigation smoke test, builds the Inno Setup installer and creates a portable ZIP.

The smoke test is not a substitute for interactive testing of terminals, external dependencies or every workspace. See [TESTING.md](TESTING.md).

## Dependencies
Optional tools such as Git, WSL, Python, Node.js, Docker, Ollama, SQLite CLI and cloud CLIs are separate dependencies. EVILWARE should detect missing tools and explain how to install them; it must not silently install them.

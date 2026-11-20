# NEXUS Developer Command Centre

NEXUS is a native Windows desktop workspace built with C++20 and Qt 6. It brings everyday development tools into one dark, keyboard-friendly application.

## Included in this build

- **Overview:** operating system, architecture, host, processor-thread count and memory summary.
- **Build Apps:** create real starter projects for C++/CMake, Python and HTML/CSS/JavaScript.
- **Code Studio:** open, edit and save local text/source files.
- **Terminal:** run a local PowerShell, Command Prompt or optional WSL shell as the signed-in Windows user.
- **Cyber Lab:** basic ping, DNS, route-trace and local network-configuration diagnostics for authorized testing.
- **File Explorer:** browse the current user's home folder.
- **Git Manager:** run Git status, recent log and branch listing in a selected working directory.

## Windows build

Every push to `main` starts the Windows x64 GitHub Actions workflow. The workflow builds the Qt application, deploys the required Qt runtime files and creates:

- `NEXUS-Setup-x64.exe` — Windows installer.
- `NEXUS-Windows-x64-Portable.zip` — portable application folder.

Open the repository's **Actions** tab, select **NEXUS Windows x64 Build**, then download the artifact from a successful run. See [docs/BUILDING.md](docs/BUILDING.md) for local build requirements.

## Optional dependencies

PowerShell and Command Prompt are provided by Windows. WSL and Git must be installed separately to use those integrations. NEXUS does not silently install external tools. A shell runs with the current Windows user's permissions.

## Cyber Lab safety

The Cyber Lab is limited to basic network diagnostics. Only assess devices and networks you own or have explicit permission to test. No exploit automation, credential theft, stealth, persistence or destructive attack features are included.

## Current scope

This is the first integrated desktop build. It does not yet include a full IDE language-server/debugger, database administration suite, Docker orchestration, AI model runtime or third-party plugin marketplace. Those require additional modules and integrations; they are not represented as finished features here.

## License

MIT

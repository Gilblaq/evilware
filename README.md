# EVILWARE — The Unholy Core 3

EVILWARE is a native Windows x64 developer command centre built with C++20 and Qt 6. The Unholy Core 3 edition presents 20 dedicated workspaces in a professional graphite interface.

## 20 workspaces

1. Dashboard
2. Build Apps
3. Code Studio
4. Linux Terminal
5. Windows Terminal
6. Cyber Lab
7. Network Tools
8. Web Security
9. Digital Forensics
10. Reverse Engineering
11. Database Studio
12. AI Lab
13. Docker & DevOps
14. Cloud Manager
15. File Manager
16. Git Manager
17. Plugin Store
18. System Monitor
19. Settings
20. Documentation

## Current implementation status

Implemented foundations include project generation, source-file editing, local PowerShell/CMD and WSL terminals, basic authorized network diagnostics, file browsing, Git status/log/branch inspection, system information and a package manager.

The Plugin & Software Manager currently provides a curated catalogue for Git, Visual Studio Code, Python, Node.js, Docker Desktop, Ubuntu on WSL, Nmap and Wireshark. It checks for installed packages and offers install, update, reinstall and uninstall actions through Windows Package Manager (winget). Package operations require user confirmation and display completion or failure details.

Build Apps checks for template dependencies. When a supported dependency is missing, EVILWARE asks before invoking winget; declining leaves the project files intact. Some toolchains (for example, a C++ compiler) still require separate setup and are explained to the user. Windows Package Manager is supplied by Microsoft's App Installer.

The remaining specialist workspaces are dedicated module foundations with capability and dependency information. They are not yet completed tool suites. See [the release roadmap](docs/RELEASE_ROADMAP.md) for milestones and release gates.

## Build and package

Every push to `main` starts the Windows x64 GitHub Actions workflow. It builds EVILWARE, deploys Qt runtime files, runs a headless navigation smoke test, creates the Windows installer and packages a portable ZIP. Download the `EVILWARE-Unholy-Core-3-Windows-x64` artifact from a successful Actions run.

## Security

Package operations use exact package identifiers and the configured winget source. Users must confirm installations, updates and removals. Review package publishers and system prompts before approving software installation. Use security and network tools only on systems you own or have explicit permission to assess. EVILWARE's security workspaces are intended for defensive research, learning and authorized testing.

## Release readiness

A successful CI build confirms compilation, the automated navigation check and artifact packaging. It does not, by itself, certify every workspace or replace clean-machine manual testing. Public release requires Windows validation, installer testing, privacy/legal materials, checksums and a trustworthy code-signing/update plan.

## License

MIT

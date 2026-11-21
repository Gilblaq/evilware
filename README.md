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

## Implemented tools in this foundation build

- Dashboard: operating system, architecture, host, processor and memory summary.
- Build Apps: starter project generation for C++/CMake, Python and HTML/CSS/JavaScript.
- Code Studio: open, edit and save local source/text files.
- Linux Terminal: WSL shell launch (requires WSL).
- Windows Terminal: PowerShell, Command Prompt and WSL shell selector.
- Cyber Lab: basic ping, DNS, route trace and local network configuration diagnostics for authorized testing.
- File Manager: browse the current user's home folder.
- Git Manager: status, recent log and branch listing in a selected folder.
- Plugin Store: initial catalogue with winget installation handoff for Git, VS Code, Python, Node.js, Docker Desktop, Ubuntu WSL, Nmap and Wireshark.

The remaining specialist workspaces are present as dedicated module foundations with capability and dependency information. They are not yet represented as completed tool suites. They will be implemented and tested in subsequent development milestones.

## Build and package

Every push to `main` starts the Windows x64 GitHub Actions workflow. It builds EVILWARE, deploys Qt runtime files, creates the installer and portable ZIP. Download the `EVILWARE-Unholy-Core-3-Windows-x64` artifact from a successful Actions run.

## Security

Use security and network tools only on systems you own or have explicit permission to assess. EVILWARE's security workspaces are intended for defensive research, learning and authorized testing.

## License

MIT

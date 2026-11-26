# EVILWARE — The Unholy Core 3

EVILWARE is a native Windows x64 developer workstation built with C++20 and Qt 6. It brings coding, terminals, project building, Git, system tools and specialist workspaces into one desktop application.

## Get EVILWARE

**Windows 10/11, 64-bit**

- **Installer:** download the `EVILWARE-Unholy-Core-3-x64.exe` file from the latest GitHub Release, then run it.
- **Portable:** download `EVILWARE-Unholy-Core-3-Windows-x64-Portable.zip`, extract the folder, and run `EVILWARE.exe`.
- Releases: https://github.com/Gilblaq/evilware/releases

If no release is published yet, the latest successful build can be found under the repository's Actions tab in the separate `EVILWARE-Unholy-Core-3-Windows-x64-Portable` artifact.

## Workspaces

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
12. AI Lab — LAMIA AI
13. Docker & DevOps
14. Cloud Manager
15. File Manager
16. Git Manager
17. Plugin Store
18. System Monitor
19. Settings
20. Documentation
21. My Apps — a local portfolio for tracking applications, versions, stages, repositories and working folders.
22. Premium Workspace — account login, server-verified subscriptions and owner customer management. Premium access is granted only after Paystack payment verification by the hosted API.

## What is implemented

My Apps lets you add, edit, remove and open registered app folders. Its portfolio is stored locally in the current Windows user's EVILWARE application-data directory.\n\nImplemented foundations include project generation, source-file editing, local PowerShell/CMD and WSL terminals, basic authorized network diagnostics, file browsing, Git status/log/branch inspection, system information and a package manager.

The Plugin & Software Manager has two sections. EVILWARE Extensions provides downloadable PowerShell plugins for developer workflows (Project Audit Toolkit, Developer Port Inspector) and authorized cybersecurity workflows (DNS Inspector, HTTP Security Header Audit, Local Network Inventory). Extensions download from this repository into the current Windows user's EVILWARE application-data folder; they are not executed automatically, and users should inspect scripts before running them. The Windows Software section manages Git, Visual Studio Code, Python, Node.js, Docker Desktop, Ubuntu on WSL, Nmap and Wireshark through Windows Package Manager (winget), with confirmation for package operations.

LAMIA AI supports local Ollama and OpenAI-compatible API providers. You must configure a provider; API usage may incur charges from the provider. Local Ollama use requires Ollama and a compatible model installed on your computer.

Some specialist workspaces are still foundations rather than complete tool suites. See [the release roadmap](docs/RELEASE_ROADMAP.md) and [user guide](docs/USER_GUIDE.md) for implementation status and known limitations.

## Beta and updates

EVILWARE is being distributed as a free beta through GitHub Releases. New tagged versions are built and packaged by GitHub Actions, then attached to the matching release automatically. Premium Workspace uses a separately deployed subscription API. Payment secrets stay on the server; the desktop client never stores payment-provider credentials. The subscription API is an initial integration foundation and must be deployed and tested before paid access is offered.

## Build and validation

Every push to `main` starts the Windows x64 GitHub Actions workflow. It builds EVILWARE, deploys Qt runtime files, runs a headless navigation smoke test, creates the Windows installer and packages a separately downloadable portable ZIP.

A successful CI run confirms compilation, the automated smoke test and packaging. It does not certify every workspace or replace manual testing on a clean Windows PC.

## Security

Use security and network tools only on systems you own or have explicit permission to assess. Package operations require user confirmation. Review package publishers and system prompts before approving software installation.

## License

MIT

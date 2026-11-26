# EVILWARE — The Unholy Core 3 User Guide

## Edition status
This guide describes functions currently present in the development build. A workspace name in the sidebar does not mean a complete commercial tool suite is included. Workspaces marked foundation or in development are limited utilities and integration entry points.

## First launch
1. Install using the Windows x64 installer, or extract the portable ZIP to a writable folder.
2. Start EVILWARE. The current access screen accepts a local display name or Guest. It does not create an online account and does not protect data with a password.
3. Select a workspace from the left navigation.

## Working workspaces
- **Build Apps:** generate starter projects for supported C++, Python and web templates. Toolchains such as a C++ compiler may need separate installation. EVILWARE asks before supported winget dependency installs.
- **Code Studio:** open, edit and save source files. Save your work outside temporary folders and use Git or another backup.
- **Windows Terminal:** launches a local PowerShell or Command Prompt process as the signed-in Windows user.
- **Linux Terminal:** uses WSL when it is installed and configured.
- **Cyber Lab:** basic ping, DNS lookup, route tracing and local network configuration. Use only targets you own or are authorized to diagnose.
- **File Manager:** browse files under your user account.
- **Git Manager:** inspect Git status, recent log and branches in a selected working directory.
- **Plugin Store:** checks and manages a curated set of software through Windows Package Manager (winget). Review the publisher and package details before approving any install or removal.
- **System Monitor:** read-only process listing and CPU/physical-memory readings. Windows may restrict details for protected processes.

## My Apps\n- **My Apps:** maintain a private local list of applications with version, development stage, target platform, working folder, repository/website and notes. Add, edit, remove and open a registered app's working folder. Removing an entry never deletes the actual project files. Data is saved in the current Windows user's EVILWARE application-data directory. Subscription tracking is reserved for a later update; this build does not process payments.\n\n## Specialist workspace utilities
- **Network Tools:** ping, DNS lookup, route trace and interface configuration.
- **Web Security:** request HTTP response headers and inspect TLS connection output for a URL you are authorized to assess. These are basic checks, not a vulnerability scanner.
- **Digital Forensics:** select a file and calculate its SHA-256 hash. Preserve originals; use forensic write blockers and a proper evidence process for real investigations.
- **Reverse Engineering:** inspect file metadata, SHA-256 and printable strings from a selected file. EVILWARE does not execute the selected file.
- **Database Studio:** select a database and, if the sqlite3 command-line utility is installed, list SQLite tables. Only use databases you own or are authorized to inspect.
- **AI Lab:** inspect local Ollama version, installed models and running models; run a model only when Ollama and that model are already installed.
- **Docker & DevOps:** inspect Docker version, containers, images and Compose version. Docker Desktop is an optional external dependency.
- **Cloud Manager:** inspect Azure, AWS and Google Cloud CLI versions and Azure account status. These commands use the locally configured CLI identity.

## Safety and data
EVILWARE launches local processes under the current Windows account. It does not silently request administrator elevation. External tools may access networks or modify your system according to their own behavior. Read each command, target, package publisher and confirmation dialog before proceeding. Do not run untrusted code or commands copied from unknown sources.

The local display-name screen is not authentication. Do not use EVILWARE as a secure multi-user boundary or store secrets in project files. Cloud CLI credentials remain managed by their respective vendor tools.

## Troubleshooting
- If a tool says it cannot start, install the optional dependency from its official publisher and ensure its executable is on PATH, then restart EVILWARE.
- If WSL commands fail, install and configure WSL separately.
- If winget is missing, update Microsoft's App Installer.
- If a process has unavailable memory information, Windows may have denied access to that process.
- Keep a copy of important projects and evidence outside the application directory.

## Known limitations
The specialist workspaces are useful first utilities, not full enterprise suites. No online account system, encrypted vault, automatic update service, telemetry backend, vulnerability scanner, malware sandbox, evidence chain-of-custody system, full SQL editor, AI chat interface or cloud deployment manager is included in this build.

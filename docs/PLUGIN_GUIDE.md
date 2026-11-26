# EVILWARE Plugin Store

## EVILWARE Extensions

The Plugin Store includes downloadable PowerShell extensions from the official `Gilblaq/evilware` repository.

| Plugin | Purpose |
|---|---|
| Project Audit Toolkit | Read-only project file, language, Git status and TODO report |
| Developer Port Inspector | Lists local listening TCP ports and owning processes |
| DNS Inspector | Looks up A, AAAA, MX, NS, TXT and CNAME records |
| HTTP Security Header Audit | Reviews common security headers on an authorized website |
| Local Network Inventory | Displays this computer's IP configuration and cached neighbours |

### Install and remove

1. Open **Plugin Store** in EVILWARE.
2. Choose **Download & install** on an EVILWARE extension.
3. Confirm the download. EVILWARE downloads the PowerShell script over HTTPS from the repository's `main/plugins` directory.
4. Choose **Open plugin folder** to inspect the installed script.
5. Use **Uninstall** to remove the local plugin folder.

Plugins are saved under the current Windows user's EVILWARE application-data directory, in `plugins/<plugin-id>`. They are not executed automatically. Read the script before running it in PowerShell. Some plugins require Windows PowerShell networking cmdlets. The HTTP header audit makes a request only to the URL supplied by the user; use it only on sites you own or are authorized to assess.

The current extension downloader is deliberately restricted to PowerShell files hosted under this project's `plugins` directory. It limits downloads to 1 MiB, requires a successful HTTPS response from GitHub's raw-content host, and does not support arbitrary third-party plugin URLs or automatic code execution.

## Windows Software

The separate Windows Software section uses Windows Package Manager (winget) for tools such as Git, VS Code, Python, Node.js, Docker Desktop, Ubuntu on WSL, Nmap and Wireshark. These are external applications, not EVILWARE extension plugins. Winget must be installed and available on the computer.

## Catalogue source

The human-readable catalogue is `plugins/catalog.json`. The current application build uses the matching curated catalogue entries in `src/pluginstorewidget.cpp`. Plugin scripts and their descriptions are maintained in this repository.

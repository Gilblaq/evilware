# EVILWARE — Testing Matrix

Updated: 2026-11-24

## Evidence rules

- **CI build** means GitHub Actions compiled and packaged the application.
- **Navigation smoke test** means the application could construct its main window and verify twenty navigation/page mappings using the offscreen Qt platform.
- **Functional test** means the named operation was actually executed and its result checked.
- **Manual Windows test** means the packaged application was run on a real supported Windows installation.
- Never infer functional or manual test success from a green compile.

## Latest known CI baseline

- Workflow: EVILWARE The Unholy Core 3 Windows x64 Build.
- Run: #89.
- Commit: `1f173a287458ec87b32e46ab7b3a299c15932132`.
- Result: success.
- Completed steps: Qt install, CMake configure, Release build, Qt runtime deployment, 20-page navigation smoke test, Inno Setup installer build, portable ZIP packaging and artifact upload.
- Run URL: https://github.com/Gilblaq/evilware/actions/runs/45453046050

## Workspace matrix

| Workspace | Current implementation evidence | Functional validation status |
|---|---|---|
| Dashboard | System information and navigation summary | Not end-to-end tested |
| Build Apps | C++/Python/static-web starter generation and build/run process code | Not end-to-end tested |
| Code Studio | Single-document open/save/editor and basic highlighting | Not end-to-end tested |
| Linux Terminal | QProcess shell selection includes WSL | Not tested on Windows |
| Windows Terminal | PowerShell/CMD QProcess command console | Not tested on Windows |
| Cyber Lab | Basic authorized network diagnostics | Not end-to-end tested |
| Network Tools | Ping, DNS and route actions | Not end-to-end tested |
| Web Security | curl HTTP headers and TLS inspection | Not end-to-end tested |
| Digital Forensics | File metadata and SHA-256 foundations | Not end-to-end tested |
| Reverse Engineering | File metadata, hash and printable strings foundations | Not end-to-end tested |
| Database Studio | SQLite selection and sqlite3 table-list action | Not end-to-end tested |
| AI Lab | Ollama version/list/ps/run command actions | Not end-to-end tested |
| Docker & DevOps | Docker version/containers/images/Compose checks | Not end-to-end tested |
| Cloud Manager | Azure/AWS/gcloud CLI status checks | Not end-to-end tested |
| File Manager | QFileSystemModel home-directory browser | Not end-to-end tested |
| Git Manager | Git status/log/branch commands | Not end-to-end tested |
| Plugin Store | winget package check/install/update/uninstall workflow | Not end-to-end tested |
| System Monitor | Process inspection and system metrics | CI compile only; manual validation pending |
| Settings | QSettings owner profile and appearance persistence | CI compile only; restart test pending |
| Documentation | Module documentation foundation | Content review pending |

## Terminal regression tests

- [ ] PowerShell starts and accepts a simple command.
- [ ] CMD starts and accepts a simple command.
- [ ] WSL missing/present states are reported correctly.
- [ ] Asynchronous stdout does not erase or overwrite text being typed.
- [ ] stderr is displayed.
- [ ] Backspace cannot delete previous output.
- [ ] Up/down command history works.
- [ ] Stop process terminates a running command.
- [ ] Restart shell resets process state cleanly.
- [ ] Clear only clears visible output.
- [ ] Long-running command does not block the UI.
- [ ] Unicode and large output are handled.
- [ ] Interactive TTY applications are explicitly tested only after ConPTY is implemented.

## Settings regression tests

- [ ] Change owner fields and save.
- [ ] Change theme and save.
- [ ] Change background and save.
- [ ] Restart app and verify values are restored.
- [ ] Save repeatedly and check stylesheet does not accumulate.
- [ ] Invalid or unavailable custom settings fail safely.

## Build Apps regression tests

- [ ] Create C++ project and verify source/CMake files.
- [ ] Build with supported compiler and verify executable location.
- [ ] Run generated executable.
- [ ] Create and run Python project.
- [ ] Create web project and open its index page.
- [ ] Missing dependencies produce actionable messages.
- [ ] Declining installation leaves project files intact.
- [ ] Process failure restores UI controls and displays diagnostics.

## Release tests

- [ ] Install on clean Windows 10 x64.
- [ ] Install on clean Windows 11 x64.
- [ ] Launch without development Qt installed.
- [ ] Verify Qt/runtime dependencies are bundled.
- [ ] Verify startup, settings and all navigation pages.
- [ ] Verify installer upgrade behavior.
- [ ] Verify uninstaller removes application files without deleting user projects.
- [ ] Verify portable ZIP after full extraction.
- [ ] Generate and verify SHA-256 checksums.
- [ ] Record manual test machine, OS build, app version and result.

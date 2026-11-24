# EVILWARE — The Unholy Core 3 Roadmap

This is the active engineering checklist. A feature is only marked complete when implementation and its relevant tests have evidence. A successful CI build is not proof of full runtime functionality.

## Current baseline (2026-11-24)

- [x] Public GitHub repository and Windows x64 Qt 6 / C++20 build pipeline.
- [x] Twenty navigation entries mapped to twenty pages in the headless smoke test.
- [x] Windows installer and portable ZIP packaging steps exist in CI.
- [x] Project starter generation for C++/CMake, Python and static web.
- [x] Basic source editor, file browser, Git status/log/branch actions, system monitor and winget catalogue.
- [x] Specialist module foundations and initial local diagnostic actions.
- [x] Owner appearance settings are stored with QSettings and reapplied at startup.
- [x] Removed the animated snake from splash and access screens.
- [x] Added a standard-library asset generator for original offline artwork and a multi-resolution Windows application icon.
- [x] Replaced legacy Qt standard navigation glyphs with a consistent symbol-based navigation treatment.
- [x] Replaced the AI Lab placeholder with the first functional LAMIA AI chat/provider implementation.
- [x] Expanded appearance settings and introduced an application-wide palette/style application path; runtime verification pending.
- [x] Terminal output insertion now preserves text being typed; stop/clear controls and process-start errors added.
- [ ] Terminal remains a QProcess/stdin command console, not a full VT/ConPTY terminal emulator; interactive TTY applications and terminal escape sequences are not yet supported.
- [ ] No end-to-end manual runtime validation on clean Windows 10/11 machines has been recorded.

## Milestone 1 — Repository/build baseline
- [x] Inspect repository structure, CMake target, workflow, existing modules and release documents.
- [x] Confirm latest baseline run #89 passed build, deployment, smoke test and packaging.
- [ ] Add automated unit/functional tests beyond navigation construction.
- [ ] Verify installer and portable ZIP on a clean Windows machine.

## Milestone 2 — Terminal reliability
- [x] Keep user input intact when asynchronous process output arrives.
- [x] Add process stop and clear controls.
- [x] Report shell startup failures.
- [x] Avoid duplicate shell restarts when selecting an already-selected shell.
- [ ] Implement Windows ConPTY for real interactive terminal behavior; current shell sessions still use redirected QProcess streams.
- [x] Add independent terminal tabs with per-tab shell selection and close behavior.
- [ ] Add working-directory controls.
- [ ] Validate PowerShell, CMD and WSL stdin behavior on Windows.
- [ ] Test long-running commands, Unicode, resize, Ctrl+C, history and error output.

## Milestone 3 — Owner settings
- [x] Persist owner profile fields and theme/background choices.
- [x] Reapply saved appearance on application startup.
- [x] Prevent repeated saves from appending duplicate theme stylesheets.
- [x] Add font-size, default-shell, notifications and confirmation preferences to Settings Center.
- [ ] Add reduced-motion and configuration backup/restore.
- [ ] Verify generated assets and Windows executable icon in CI and on a clean Windows machine.
- [ ] Test save/restart/restore behavior on Windows.

## Milestone 4 — Twenty workspace audit
- [x] Confirm all twenty navigation items map to pages.
- [x] Identify real functionality and limitations in specialist modules.
- [ ] Add per-workspace capability/status matrix and operational tests.
- [ ] Replace remaining planned/foundation-only surfaces with tested tools or clearly scoped integrations.

## Milestone 5 — Code Studio and project management
- [x] Basic single-document editor with open/save and syntax highlighting foundation.
- [ ] Add tabs, line numbers, find/replace, unsaved-change prompts and language-aware editing.
- [x] Add project file tree and editor integration; newly created projects open in the Code Studio explorer.
- [ ] Add recent projects and safe file/folder operations.

## Milestone 6 — Build Apps
- [x] Generate C++ console, Python and static web starter projects.
- [x] Provide build/run output and dependency checks.
- [ ] Correctly discover compiler/toolchain and all CMake executable output layouts.
- [ ] Add project cancellation, build/stop state and robust process cleanup.
- [x] Add Node.js starter generation and execution path (CI validation pending).
- [ ] Add tested Qt Widgets template.
- [ ] Add project reopen/import and packaging workflows.

## Milestone 7 — Git/GitHub
- [x] Git status, log, branch inspection, diff, stage-all, commit, fetch, pull and push actions (runtime validation pending).
- [ ] Add unstage, branch creation/switching and clone.
- [ ] Add secure GitHub authentication and repository operations.

## Milestone 8 — Network and web security
- [x] Basic ping, DNS lookup and route diagnostics.
- [x] Basic HTTP header and TLS inspection via curl.
- [ ] Add robust cross-platform dependency detection, reports and authorized target scoping.
- [ ] Validate URL parsing, process failures and output.

## Milestone 9 — Reverse engineering and forensics
- [x] Basic file metadata, SHA-256 and printable strings inspection foundations.
- [ ] Add binary format identification, hex viewer, evidence register and chain-of-custody records.
- [ ] Add optional Ghidra/radare2 integrations with dependency detection.

## Milestone 10 — Database Studio
- [x] SQLite database selection and table-list command foundation.
- [ ] Add embedded SQLite query editor, schema browser, result grid and export.
- [ ] Add safe read-only defaults and explicit write/transaction controls.

## Milestone 11 — AI Lab
- [x] Added LAMIA AI chat workspace with provider selection, configurable endpoint/model and async requests.
- [x] Added Ollama `/api/chat` and OpenAI-compatible `/v1/chat/completions` request paths.
- [x] Added provider connection testing and Ollama/OpenAI-compatible model discovery.
- [x] Added Windows DPAPI protection for saved API keys; secrets are not stored as plaintext in QSettings.
- [ ] Validate live provider requests, model errors and key protection on Windows; no external provider credentials are available for CI.

## Milestone 12 — Docker and DevOps
- [x] Basic Docker version, containers, images and Compose checks.
- [ ] Add safe container inspection/start/stop/logs and Compose workflows with confirmation for destructive operations.

## Milestone 13 — Cloud Manager
- [x] Basic Azure/AWS/gcloud CLI availability and account-status foundations.
- [ ] Add provider-specific account/resource workflows with secure credential handling.

## Milestone 14 — Software manager
- [x] Curated winget catalogue, exact IDs, publisher links, explicit confirmation and tracked install/update/uninstall processes.
- [ ] Validate package IDs/publishers, add cancellation and retry, and test on supported Windows versions.

## Milestone 15 — Security, performance and QA
- [ ] Add automated tests for core logic and process workflows.
- [ ] Audit process argument handling, filesystem operations, secrets, logs and permissions.
- [ ] Test memory/CPU use and shutdown behavior on modest hardware.
- [ ] Complete clean Windows 10/11 manual test matrix.

## Milestone 16 — Release
- [x] CI currently creates an installer and portable ZIP artifact.
- [ ] Verify installer metadata, bundled runtime and uninstall behavior.
- [ ] Add checksums, signing strategy, license/third-party notices and final privacy/legal materials.
- [ ] Perform clean-machine installation, upgrade and uninstall tests.
- [ ] Publish only after all release gates pass.

## Current execution focus

1. Verify terminal and settings commits through CI.
2. Extend test coverage and add the workspace capability matrix.
3. Improve Build Apps and project/editor integration.
4. Continue the specialist workspace audit and implementation.
5. Keep distribution blocked until runtime and clean-install testing are complete.

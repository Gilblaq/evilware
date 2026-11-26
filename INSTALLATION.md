# Installing EVILWARE

## Current CI packages
The Windows x64 workflow creates:
- An Inno Setup installer: `EVILWARE-Unholy-Core-3-x64.exe`.
- A portable ZIP: `EVILWARE-Unholy-Core-3-Windows-x64-Portable.zip`.

Artifacts are attached to successful GitHub Actions runs. Use the artifact from the latest verified run, not an older build.

## Installer
Run the installer and choose an installation folder. The current installer is configured for per-user installation privileges and creates Start Menu and optional desktop shortcuts.

## Portable ZIP
Extract the complete ZIP to a writable folder before launching `EVILWARE.exe`. Do not launch from the compressed-folder preview.

## External tools
The application has optional integrations with third-party tools. Some workspaces will show limited functionality if those tools are not installed or authenticated. Review publisher information and approve package operations yourself.

## Release status
A CI artifact is a development build until it passes clean-machine installation, launch, upgrade and uninstall tests on supported Windows versions. No public release should be described as fully validated until those checks are recorded in [TESTING.md](TESTING.md).


## Premium Workspace setup

The desktop Premium Workspace requires the separate subscription API in `server/`. Until that API is deployed and configured, account sign-in and checkout will not be available. Follow [server/README.md](server/README.md) to deploy it on Render and configure Paystack. Do not publish paid subscriptions until webhook verification, renewal, expiry and revocation have been tested with Paystack test credentials.
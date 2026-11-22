# Security Policy

## Supported product
EVILWARE — The Unholy Core 3 is currently a development build for Windows x64. It is not yet a security-certified or production-supported product.

## Reporting a vulnerability
Do not publish exploit details or sensitive data in a public issue. Contact the repository owner privately through GitHub's security advisory reporting feature, if enabled for this repository. Include the affected version, Windows version, reproduction steps and impact. Do not include passwords, tokens, personal data or customer files.

## Safe use
- Use network and web diagnostics only against systems you own or have explicit permission to test.
- Review commands and package publishers before approval.
- Do not run untrusted binaries or scripts.
- Do not treat the local display-name screen as authentication.
- Do not store secrets in source files or project folders.
- External commands execute with the current user's permissions; EVILWARE does not guarantee the behavior of third-party tools.

## Scope and limitations
The current security-related workspaces provide basic diagnostics, file hashing and static file inspection. They are not a malware sandbox, exploit framework, vulnerability scanner, incident-response platform or forensic chain-of-custody system.

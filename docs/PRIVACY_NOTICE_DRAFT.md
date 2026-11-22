# EVILWARE Privacy Notice — Draft for Review

**This is a drafting aid, not a published legal privacy policy. Do not present it as final until the publisher has reviewed the actual distribution, support, payment, analytics and website services and obtained appropriate legal advice.**

## Current desktop development build
The current EVILWARE desktop application uses a local display name or Guest label to title the workspace. The access screen does not create an online account or send the display name to an EVILWARE authentication server.

The application can launch local command-line programs selected by the user. Depending on the selected feature and command, those programs may communicate with third-party services or networks. Examples include:
- Windows Package Manager (winget) for software installation and updates after user confirmation.
- Ping, DNS, route and HTTP header checks against user-selected targets.
- Local vendor command-line tools such as Azure CLI, AWS CLI, Google Cloud CLI, Ollama, Docker and Git.

Those third-party programs have their own privacy practices and may process account credentials, network addresses, command history or service usage data. Review their documentation and configuration.

## Information handling
Do not assume that the current build provides encrypted storage, cloud backup, telemetry controls, crash-report upload, account synchronization or a secure secrets vault. Do not place credentials or sensitive customer data in project files or command output.

## Before public distribution
The publisher must add accurate details for:
- Legal publisher identity and contact details.
- Website hosting, download delivery, payment processing and customer support providers.
- Any analytics, crash reporting, update checks or telemetry added in future versions.
- Retention periods, user rights, international transfers and a process for privacy requests.
- Applicable jurisdictions and a contact for privacy questions.

Update this notice whenever application behavior or connected services change.

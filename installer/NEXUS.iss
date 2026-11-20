[Setup]
AppId={{6A4C0D13-2A9E-4B3B-9B13-1F0B5B0C2201}
AppName=NEXUS Developer Command Centre
AppVersion=1.0.0
AppPublisher=NEXUS Developer Tools
DefaultDirName={autopf}\NEXUS
DefaultGroupName=NEXUS Developer Command Centre
UninstallDisplayIcon={app}\NEXUS.exe
OutputDir=installer-output
OutputBaseFilename=NEXUS-Setup-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=lowest
DisableProgramGroupPage=yes

[Files]
Source: "dist\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\NEXUS Developer Command Centre"; Filename: "{app}\NEXUS.exe"
Name: "{autodesktop}\NEXUS Developer Command Centre"; Filename: "{app}\NEXUS.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"

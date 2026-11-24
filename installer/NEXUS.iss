[Setup]
AppId={{6A4C0D13-2A9E-4B3B-9B13-1F0B5B0C2201}
AppName=EVILWARE - The Unholy Core 3
AppVersion=3.0.0
AppPublisher=EVILWARE
SetupIconFile=..\assets\evilware_app_icon.ico
DefaultDirName={autopf}\EVILWARE
DefaultGroupName=EVILWARE - The Unholy Core 3
UninstallDisplayIcon={app}\EVILWARE.exe
OutputDir=..\installer-output
OutputBaseFilename=EVILWARE-Unholy-Core-3-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=lowest
DisableProgramGroupPage=yes

[Files]
Source: "..\dist\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\EVILWARE - The Unholy Core 3"; Filename: "{app}\EVILWARE.exe"
Name: "{autodesktop}\EVILWARE - The Unholy Core 3"; Filename: "{app}\EVILWARE.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"

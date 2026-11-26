param([string]$Path = (Get-Location).Path)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $Path).Path
Write-Host "EVILWARE Project Audit" -ForegroundColor Cyan
Write-Host "Project: $root"
Write-Host "Generated: $(Get-Date -Format s)"
$files = Get-ChildItem -LiteralPath $root -File -Recurse -Force -ErrorAction SilentlyContinue | Where-Object { $_.FullName -notmatch '[\\/](\.git|node_modules|\.venv|venv|build|dist)[\\/]' }
Write-Host "File count: $($files.Count)"
$files | Group-Object Extension | Sort-Object Count -Descending | Select-Object -First 12 Name,Count | Format-Table -AutoSize
$markers = foreach ($f in ($files | Where-Object { $_.Length -lt 2MB -and $_.Extension -in ".cpp",".h",".hpp",".c",".py",".js",".ts",".cs",".java",".go",".rs",".html",".css",".ps1" })) {
  Select-String -LiteralPath $f.FullName -Pattern 'TODO|FIXME|XXX' -ErrorAction SilentlyContinue | ForEach-Object { [pscustomobject]@{File=$f.FullName.Substring($root.Length).TrimStart('\');Line=$_.LineNumber;Text=$_.Line.Trim()} }
}
Write-Host "TODO / FIXME markers: $(@($markers).Count)"
$markers | Select-Object -First 50 | Format-Table -AutoSize
if (Get-Command git -ErrorAction SilentlyContinue) { Write-Host "Git status:"; git -C $root status --short }
Write-Host "Audit complete. Read-only: no project files were changed." -ForegroundColor Green
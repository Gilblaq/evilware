Write-Host "EVILWARE Developer Port Inspector" -ForegroundColor Cyan
Write-Host "Listening TCP ports on this computer:"
Get-NetTCPConnection -State Listen -ErrorAction SilentlyContinue | Sort-Object LocalPort | ForEach-Object {
  $proc = Get-Process -Id $_.OwningProcess -ErrorAction SilentlyContinue
  [pscustomobject]@{Address=$_.LocalAddress;Port=$_.LocalPort;PID=$_.OwningProcess;Process=if($proc){$proc.ProcessName}else{"(exited)"}}
} | Format-Table -AutoSize
Write-Host "Read-only inspection of local listening sockets." -ForegroundColor Green
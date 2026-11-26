param([Parameter(Mandatory=$true)][ValidatePattern("^(?=.{1,253}$)[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?(?:\.[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?)*\.?$")][string]$Domain)
Write-Host "EVILWARE DNS Inspector" -ForegroundColor Cyan
Write-Host "Domain: $Domain"
foreach ($type in "A","AAAA","MX","NS","TXT","CNAME") {
  Write-Host "[$type]" -ForegroundColor Yellow
  try { Resolve-DnsName -Name $Domain -Type $type -ErrorAction Stop | Select-Object Name,Type,NameHost,IPAddress,Preference,NameExchange,Strings | Format-Table -AutoSize }
  catch { Write-Host "No $type records returned." -ForegroundColor DarkGray }
}
Write-Host "DNS queries completed." -ForegroundColor Green
Write-Host "EVILWARE Local Network Inventory" -ForegroundColor Cyan
Write-Host "Local adapters and addresses:"
Get-NetIPConfiguration -ErrorAction SilentlyContinue | Select-Object InterfaceAlias,@{Name="IPv4";Expression={($_.IPv4Address | ForEach-Object IPAddress) -join ", "}},@{Name="Gateway";Expression={($_.IPv4DefaultGateway | ForEach-Object NextHop) -join ", "}},@{Name="DNS";Expression={($_.DNSServer.ServerAddresses) -join ", "}} | Format-Table -Wrap -AutoSize
Write-Host "Cached neighbour entries (ARP/ND):"
Get-NetNeighbor -ErrorAction SilentlyContinue | Where-Object { $_.State -notin @("Unreachable","Incomplete") } | Select-Object InterfaceAlias,IPAddress,LinkLayerAddress,State | Format-Table -AutoSize
Write-Host "Local inventory complete. This does not scan other devices." -ForegroundColor Green
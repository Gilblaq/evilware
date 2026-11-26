param([Parameter(Mandatory=$true)][string]$Url)
$uri = $null
if (-not [Uri]::TryCreate($Url,[UriKind]::Absolute,[ref]$uri) -or $uri.Scheme -notin @("http","https")) { throw "Provide a valid http:// or https:// URL." }
Write-Host "EVILWARE HTTP Security Header Audit" -ForegroundColor Cyan
Write-Host "Target: $($uri.GetLeftPart([UriPartial]::Authority))"
Write-Host "Only assess sites you own or are authorized to test." -ForegroundColor Yellow
try {
  $response = Invoke-WebRequest -Uri $uri -Method Get -MaximumRedirection 5 -TimeoutSec 15 -UseBasicParsing
  $headers = $response.Headers
  $names = @("Strict-Transport-Security","Content-Security-Policy","X-Content-Type-Options","X-Frame-Options","Referrer-Policy","Permissions-Policy")
  $results = foreach ($name in $names) { $value=$headers[$name]; [pscustomobject]@{Header=$name;Status=if($value){"Present"}else{"Missing"};Value=if($value){$value}else{"-"}} }\n  $results | Format-Table -Wrap -AutoSize
  Write-Host "HTTP status: $([int]$response.StatusCode)"
} catch { Write-Warning "Request failed: $($_.Exception.Message)" }
Write-Host "Review complete; this tool does not exploit or modify the target." -ForegroundColor Green
param()

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'common.ps1')

$adsHome = Get-AurixStudioHome
$pathsToAdd = @(
    (Join-Path $adsHome 'tools\Compilers\Tasking_1.1r8\ctc\bin'),
    (Join-Path $adsHome 'tools\AurixFlasherSoftwareTool_v1.0.8'),
    'D:\APPS\Infineon\Memtool 2021'
) | Where-Object { Test-Path $_ }

$currentUserPath = [Environment]::GetEnvironmentVariable('Path', 'User')
$parts = @()

if ($currentUserPath) {
    $parts = $currentUserPath -split ';' | Where-Object { $_ -and $_.Trim() }
}

$added = @()
foreach ($pathEntry in $pathsToAdd) {
    if ($parts -notcontains $pathEntry) {
        $parts += $pathEntry
        $added += $pathEntry
    }
}

[Environment]::SetEnvironmentVariable('Path', ($parts -join ';'), 'User')

if ($added.Count -eq 0) {
    Write-Host 'User PATH already contains the required AURIX entries.'
} else {
    Write-Host 'Added the following entries to the user PATH:'
    $added | ForEach-Object { Write-Host "  $_" }
    Write-Host 'Open a new terminal session to pick up the updated PATH.'
}

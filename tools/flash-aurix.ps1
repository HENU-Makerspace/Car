param(
    [string]$ProjectRoot,
    [string]$Config = 'Debug',
    [string]$Artifact,
    [switch]$UseHex,
    [switch]$EraseAll,
    [switch]$NoVerify,
    [switch]$LeaveHalted
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'common.ps1')

if (-not $ProjectRoot) {
    $ProjectRoot = Get-ProjectRoot
}

$projectRoot = (Resolve-Path $ProjectRoot).Path
$projectFile = Join-Path $projectRoot '.project'

if (-not (Test-Path $projectFile)) {
    throw "ADS project file was not found: $projectFile"
}

[xml]$projectXml = Get-Content $projectFile -Raw
$projectName = $projectXml.projectDescription.name
$flasher = Get-AurixFlasherPath

if (-not $Artifact) {
    $defaultExtension = if ($UseHex) { '.hex' } else { '.elf' }
    $Artifact = Join-Path $Config ($projectName + $defaultExtension)
}

$artifactPath = (Resolve-Path (Join-Path $projectRoot $Artifact)).Path
$logFile = Join-Path $projectRoot (Join-Path $Config 'AurixFlasher-log.xml')

$args = @()
if ($UseHex -or [System.IO.Path]::GetExtension($artifactPath).ToLowerInvariant() -eq '.hex') {
    $args += @('-hex', $artifactPath)
} else {
    $args += @('-elf', $artifactPath)
}

$args += @('-erase', ($(if ($EraseAll) { 'all' } else { 'on' })))
$args += @('-ver',   ($(if ($NoVerify) { 'off' } else { 'on' })))
$args += @('-connect', '6')
$args += @('-start', ($(if ($LeaveHalted) { 'off' } else { 'on' })))
$args += @('-log', $logFile)

& $flasher @args
if ($LASTEXITCODE -ne 0) {
    throw 'AurixFlasher returned a non-zero exit code.'
}

Write-Host "Flash completed. Log: $logFile"

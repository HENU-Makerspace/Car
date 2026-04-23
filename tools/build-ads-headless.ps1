param(
    [string]$ProjectRoot,
    [string]$Config = 'Debug',
    [string]$Workspace,
    [switch]$Clean
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
$javaExe = Get-AdsJavaPath
$launcherJar = Get-AdsLauncherJar
$buildDir = Join-Path $projectRoot $Config

if (-not $Workspace) {
    $Workspace = Join-Path (Split-Path $projectRoot -Parent) ("ads-headless-workspace-" + $projectName + "-" + $Config)
}

New-Item -ItemType Directory -Force -Path $Workspace | Out-Null
$workspaceProjectDir = Join-Path $Workspace ".metadata\.plugins\org.eclipse.core.resources\.projects\$projectName"
$shouldImport = -not (Test-Path $workspaceProjectDir)

function Repair-HeadlessProjectFile {
    [xml]$projectXml = Get-Content $projectFile -Raw
    $buildSpec = $projectXml.projectDescription.buildSpec
    if (-not $buildSpec) {
        return
    }

    $commands = @($buildSpec.buildCommand)
    if (-not $commands.Count) {
        return
    }

    $seen = @{}
    $changed = $false

    foreach ($command in @($commands)) {
        $name = [string]$command.name

        if ($name -eq 'com.infineon.aurix.buildsystem.builders.booster') {
            $null = $buildSpec.RemoveChild($command)
            $changed = $true
            continue
        }

        if ($name -eq 'com.infineon.aurix.buildsystem.builders.autodiscovery') {
            if ($seen.ContainsKey($name)) {
                $null = $buildSpec.RemoveChild($command)
                $changed = $true
                continue
            }
        }

        $seen[$name] = $true
    }

    if ($changed) {
        $projectXml.Save($projectFile)
    }
}

function Ensure-ZfDeviceSubdirMk {
    $deviceSourceDir = Join-Path $projectRoot 'libraries\zf_device'
    $deviceBuildDir = Join-Path $buildDir 'libraries\zf_device'
    $deviceSubdirMk = Join-Path $deviceBuildDir 'subdir.mk'
    $includeOptFile = (Join-Path $buildDir 'TASKING_C_C___Compiler-Include_paths__-I_.opt') -replace '\\', '/'

    if (-not (Test-Path $deviceSourceDir)) {
        return
    }

    New-Item -ItemType Directory -Force -Path $deviceBuildDir | Out-Null

    $deviceSources = Get-ChildItem -Path $deviceSourceDir -Filter '*.c' -File |
        Sort-Object Name |
        Select-Object -ExpandProperty Name

    if (-not $deviceSources.Count) {
        return
    }

    $lines = [System.Collections.Generic.List[string]]::new()
    $lines.Add('################################################################################')
    $lines.Add('# Automatically-generated file. Do not edit!')
    $lines.Add('################################################################################')
    $lines.Add('')
    $lines.Add('# Add inputs and outputs from these tool invocations to the build variables ')

    $groups = @(
        @{ Header = 'C_SRCS += \'; Prefix = '../libraries/zf_device/'; Suffix = '.c' },
        @{ Header = 'COMPILED_SRCS += \'; Prefix = 'libraries/zf_device/'; Suffix = '.src' },
        @{ Header = 'C_DEPS += \'; Prefix = 'libraries/zf_device/'; Suffix = '.d' },
        @{ Header = 'OBJS += \'; Prefix = 'libraries/zf_device/'; Suffix = '.o' }
    )

    foreach ($group in $groups) {
        $lines.Add($group.Header)
        for ($i = 0; $i -lt $deviceSources.Count; $i++) {
            $baseName = [System.IO.Path]::GetFileNameWithoutExtension($deviceSources[$i])
            $entryName = if ($group.Suffix -eq '.c') { $deviceSources[$i] } else { $baseName + $group.Suffix }
            $line = $group.Prefix + $entryName
            if ($i -lt ($deviceSources.Count - 1)) {
                $line += ' \'
            }
            else {
                $line += ' '
            }
            $lines.Add($line)
        }
        $lines.Add('')
    }

    $lines.Add('')
    $lines.Add('# Each subdirectory must supply rules for building sources it contributes')

    foreach ($sourceFile in $deviceSources) {
        $baseName = [System.IO.Path]::GetFileNameWithoutExtension($sourceFile)
        $lines.Add("libraries/zf_device/$baseName.src: ../libraries/zf_device/$sourceFile libraries/zf_device/subdir.mk")
        $lines.Add("`tcctc -cs --dep-file=""`$(*F).d"" --misrac-version=2004 -D__CPU__=tc26xb ""-f$includeOptFile"" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o ""`$@"" ""`$<""")
        $lines.Add("libraries/zf_device/$baseName.o: libraries/zf_device/$baseName.src libraries/zf_device/subdir.mk")
        $lines.Add("`tastc -Og -Os --no-warnings= --error-limit=42 -o  ""`$@"" ""`$<""")
    }

    $encoding = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllLines($deviceSubdirMk, $lines, $encoding)
}

function Invoke-AdsHeadlessBuild {
    $args = @(
        '-jar', $launcherJar,
        '-nosplash',
        '-application', 'org.eclipse.cdt.managedbuilder.core.headlessbuild',
        '-data', $Workspace
    )

    if ($shouldImport) {
        $args += @('-import', $projectRoot)
    }

    $args += @(
        '-build', "$projectName/$Config",
        '-verbose',
        '-printErrorMarkers'
    )

    & $javaExe @args
    if ($LASTEXITCODE -ne 0) {
        throw 'ADS headless build failed.'
    }
}

if ($Clean) {
    $resolvedBuildDir = [System.IO.Path]::GetFullPath($buildDir)
    if ($resolvedBuildDir -notlike "$projectRoot*") {
        throw "Refusing to clean unexpected build directory: $resolvedBuildDir"
    }

    if (Test-Path $resolvedBuildDir) {
        $patterns = @('*.o', '*.d', '*.src', '*.elf', '*.hex', '*.map', '*.siz', '*.log', '*.tmp', '*.bak')
        foreach ($pattern in $patterns) {
            Get-ChildItem -Path $resolvedBuildDir -Recurse -File -Filter $pattern -ErrorAction SilentlyContinue |
                Remove-Item -Force
        }
    }
}

Repair-HeadlessProjectFile
Ensure-ZfDeviceSubdirMk
Invoke-AdsHeadlessBuild

Write-Host "Build completed: $(Join-Path $buildDir ($projectName + '.elf'))"

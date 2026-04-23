function Get-AurixStudioHome {
    if ($env:AURIX_STUDIO_HOME -and (Test-Path $env:AURIX_STUDIO_HOME)) {
        return $env:AURIX_STUDIO_HOME
    }

    $roots = @(
        'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*',
        'HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*'
    )

    foreach ($root in $roots) {
        $item = Get-ItemProperty $root -ErrorAction SilentlyContinue |
            Where-Object { $_.DisplayName -like 'AURIX*Development Studio*' } |
            Select-Object -First 1

        if ($item) {
            if ($item.InstallLocation -and (Test-Path $item.InstallLocation)) {
                return $item.InstallLocation.TrimEnd('\')
            }

            if ($item.'Inno Setup: App Path' -and (Test-Path $item.'Inno Setup: App Path')) {
                return $item.'Inno Setup: App Path'.TrimEnd('\')
            }
        }
    }

    throw 'AURIX Development Studio installation was not found. Set AURIX_STUDIO_HOME first.'
}

function Get-ProjectRoot {
    return (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
}

function Get-ProjectName {
    $projectFile = Join-Path (Get-ProjectRoot) '.project'
    [xml]$projectXml = Get-Content $projectFile -Raw
    return $projectXml.projectDescription.name
}

function Get-AdsJavaPath {
    $adsHome = Get-AurixStudioHome
    $javaPath = Get-ChildItem (Join-Path $adsHome 'plugins') -Recurse -Filter java.exe -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -match 'org\.eclipse\.justj\.openjdk' } |
        Select-Object -First 1 -ExpandProperty FullName

    if (-not $javaPath) {
        throw 'Bundled ADS Java runtime was not found.'
    }

    return $javaPath
}

function Get-AdsLauncherJar {
    $adsHome = Get-AurixStudioHome
    $jar = Get-ChildItem (Join-Path $adsHome 'plugins') -Filter 'org.eclipse.equinox.launcher_*.jar' |
        Sort-Object Name -Descending |
        Select-Object -First 1 -ExpandProperty FullName

    if (-not $jar) {
        throw 'ADS Eclipse launcher jar was not found.'
    }

    return $jar
}

function Get-AurixFlasherPath {
    $adsHome = Get-AurixStudioHome
    $flasher = Join-Path $adsHome 'tools\AurixFlasherSoftwareTool_v1.0.8\AurixFlasher.exe'

    if (-not (Test-Path $flasher)) {
        throw 'AurixFlasher.exe was not found in the ADS tools directory.'
    }

    return $flasher
}

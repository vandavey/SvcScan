<#
.SYNOPSIS
    SvcScan application release package publishing script.
.DESCRIPTION
    SvcScan application release package publishing script
    used to build and compress release zip packages.
.LINK
    Application repository: https://github.com/vandavey/SvcScan
#>
[CmdletBinding()]
param ()

$DefaultErrorPreference = $ErrorActionPreference
$DefaultProgressPreference = $ProgressPreference

# Reset the global preference variables.
function Reset-Preferences {
    $ErrorActionPreference = $DefaultErrorPreference
    $ProgressPreference = $DefaultProgressPreference
}

# Write an error message to stderr and exit.
function Show-Error {
    $Symbol = "[x]"
    Reset-Preferences

    if ($PSVersionTable.PSVersion.Major -ge 6) {
        $Symbol = "`e[91m${Symbol}`e[0m"
    }
    [Console]::Error.WriteLine("${Symbol} ${args}`n")
    exit 1
}

# Write a status message to stdout.
function Show-Status {
    $Symbol = "[*]"

    if ($PSVersionTable.PSVersion.Major -ge 6) {
        $Symbol = "`e[96m${Symbol}`e[0m"
    }
    Write-Output "${Symbol} ${args}"
}

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

Show-Status "Publishing application release packages..."

$Solution = "SvcScan.sln"
$RootDir = Resolve-Path "${PSScriptRoot}\.."
$SolutionFile = "${RootDir}\src\${Solution}"

# Invalid solution file path
if (-not (Test-Path $SolutionFile)) {
    Show-Error "Solution not found: '${SolutionFile}'"
}

$VsDir = "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community"
$MsBuild = "${VsDir}\MSBuild\Current\Bin\MSBuild.exe"

# Invalid MSBuild executable path
if (-not (Test-Path $MsBuild)) {
    Show-Error "MSBuild not found: '${MsBuild}'"
}

# Create package directory
if (-not (Test-Path "${AppDir}\bin\Zips")) {
    New-Item "${AppDir}\bin\Zips" -ItemType Directory -Force > $null
}

$AppDir = "${RootDir}\src\SvcScan"
$Platforms = @("Win32", "x64")

# Build application executables
foreach ($Platform in $Platforms) {
    Show-Status "Building '${Solution}' ('Release|${Platform}')..."
    & $MsBuild $SolutionFile -t:Rebuild -p:Configuration=Release -p:Platform=$Platform

    # Build failure occurred
    if ($LASTEXITCODE -ne 0) {
        Show-Error "Failed to build '${Solution}' ('Release|${Platform}')"
    }

    Show-Status "Successfully built '${Solution}' ('Release|${Platform}')"

    if ($Platform -eq "x64") {
        $OutputFile = "${AppDir}\bin\Zips\SvcScan_Win-x64.zip"
    }
    else {
        $OutputFile = "${AppDir}\bin\Zips\SvcScan_Win-x86.zip"
    }

    $PackageFiles = @(
        "${RootDir}\*.md",
        "${RootDir}\tools\svcscan-*install.ps1",
        "${AppDir}\bin\Publish\Release\${Platform}\svcscan.exe"
    )

    # Validate all package files
    foreach ($File in $PackageFiles) {
        if (-not (Test-Path $File)) {
            Show-Error "Package file not found: '${File}'"
        }
    }

    # Remove existing package
    if (Test-Path $OutputFile) {
        Remove-Item $OutputFile -Force
    }

    Show-Status "Compressing package files ('Release|${Platform}')..."
    Compress-Archive $PackageFiles -DestinationPath $OutputFile -Force > $null
}

Reset-Preferences
Show-Status "Successfully published all release packages`n"

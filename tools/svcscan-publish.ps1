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

Show-Status "Publishing release packages..."

$Solution = "SvcScan.sln"
$RootDir = Resolve-Path "${PSScriptRoot}\.."
$SolutionFile = "${RootDir}\src\${Solution}"

# Invalid solution file path
if (-not (Test-Path $SolutionFile)) {
    Show-Error "Solution not found: '${SolutionFile}'"
}

$AppDir = "${RootDir}\src\SvcScan"
$VsDir = "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community"
$MsBuild = "${VsDir}\MSBuild\Current\Bin\MSBuild.exe"

# Invalid MSBuild executable path
if (-not (Test-Path $MsBuild)) {
    Show-Error "MSBuild not found: '${MsBuild}'"
}

# Create release package directory
if (-not (Test-Path "${AppDir}\bin\Zips")) {
    New-Item "${AppDir}\bin\Zips" -ItemType Directory -Force > $null
}

$BuildConfigs = @(
    @{ Arch = "x64"; Config = "Release"; Platform = "x64" },
    @{ Arch = "x86"; Config = "Release"; Platform = "Win32" }
)

# Build and compress release packages
foreach ($BuildConfig in $BuildConfigs) {
    $FullConfigName = "$($BuildConfig.Config)|$($BuildConfig.Platform)"
    Show-Status "Building '${Solution}' for '${FullConfigName}'..."

    & $MsBuild $SolutionFile -t:Rebuild `
                             -p:Configuration="$($BuildConfig.Config)" `
                             -p:Platform="$($BuildConfig.Platform)"

    # Build failure occurred
    if ($LASTEXITCODE -ne 0) {
        Show-Error "Failed to build '${Solution}' for '${FullConfigName}'"
    }
    Show-Status "Successfully built '${Solution}' for '${FullConfigName}'"

    $PackageFiles = @(
        "${RootDir}\*.md",
        "${RootDir}\tools\svcscan-*install.ps1",
        "${AppDir}\bin\$($BuildConfig.Config)\$($BuildConfig.Platform)\svcscan.exe"
    )

    # Validate all package files
    foreach ($PackageFile in $PackageFiles) {
        if (-not (Test-Path $PackageFile)) {
            Show-Error "Package file(s) not found: '${PackageFile}'"
        }
    }

    $ReleaseFile = "${AppDir}\bin\Zips\SvcScan_Win-$($BuildConfig.Arch).zip"
    Show-Status "Compressing release package for '${FullConfigName}'..."

    # Remove existing release package
    if (Test-Path $ReleaseFile) {
        Remove-Item $ReleaseFile -Force
    }
    Compress-Archive $PackageFiles $ReleaseFile -CompressionLevel Optimal -Force > $null
}

Reset-Preferences
Show-Status "Successfully published release packages`n"

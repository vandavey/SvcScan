<#
.SYNOPSIS
    SvcScan installer script for x64 and x86 Windows systems.
.DESCRIPTION
    SvcScan network service scanner application
    installer script for x64 and x86 Windows systems.
.LINK
    Application repository: https://github.com/vandavey/SvcScan
#>
using namespace System.IO
using namespace System.Runtime.InteropServices
using namespace System.Security.Principal

[CmdletBinding()]
param (
    [Parameter(Position=0, ValueFromPipeline)]
    [Alias("a", "p", "Arch", "Platform")]
    [ValidateSet("x64", "x86")]
    [string] $Architecture = "x64"
)

# Print an error message to stderr and exit
function Show-Error {
    $Symbol = "[x]"

    if ($PSVersionTable.PSVersion.Major -ge 7) {
        $Symbol = "`e[91m${Symbol}`e[0m"
    }
    [Console]::Error.WriteLine("${Symbol} ${args}`n")
    exit 1
}

# Print an informational message to stdout
function Show-Status {
    $Symbol = "[*]"

    if ($PSVersionTable.PSVersion.Major -ge 7) {
        $Symbol = "`e[96m${Symbol}`e[0m"
    }
    Write-Output "${Symbol} ${args}"
}

# Only Windows operating systems are supported
if (-not [RuntimeInformation]::IsOSPlatform([OSPlatform]::Windows)) {
    Show-Error "SvcScan only supports Windows operating systems"
}
$User = [WindowsPrincipal]::new([WindowsIdentity]::GetCurrent())

# Admin privileges are required
if (-not $User.IsInRole([WindowsBuiltInRole]::Administrator)) {
    Show-Error "The installer must be run as an administrator"
}

$RawRepoRoot = "https://raw.githubusercontent.com/vandavey/SvcScan/main"

# Determine installation path from given architecture
if ($Architecture -eq "x64") {
    $Location = $env:ProgramFiles
    $ZipUri = "${RawRepoRoot}/src/SvcScan/bin/Zips/SvcScan_Win-x64.zip"
}
else {
    $Location = ${env:ProgramFiles(x86)}
    $ZipUri = "${RawRepoRoot}/src/SvcScan/bin/Zips/SvcScan_Win-x86.zip"
}

$AbsLocation = "${Location}\SvcScan"
$AbsZipLocation = "${AbsLocation}\SvcScan.zip"

# Remove the existing installation directory
if (Test-Path $AbsLocation) {
    Remove-Item $AbsLocation -Recurse -Force 3>&1> $null
    Show-Status "Removed existing installation: '${AbsLocation}'"
}

New-Item $AbsLocation -ItemType Directory 3>&1> $null
Show-Status "Downloading executable zip archive to '${AbsZipLocation}'..."

# Download the executable archive
try {
    Invoke-WebRequest $ZipUri -OutFile $AbsZipLocation -DisableKeepAlive 3>&1> $null
}
catch {
    Show-Error $Error[0].Exception.Message
}

Show-Status "Unpacking zip file contents to '${AbsLocation}'..."
Expand-Archive $AbsZipLocation $AbsLocation 3>&1> $null

Remove-Item $AbsZipLocation
Show-Status "Removed temporary file '$AbsZipLocation'"

$VarTarget = [EnvironmentVariableTarget]::Machine
$EnvPath = [Environment]::GetEnvironmentVariable("PATH", $VarTarget)

# Add the parent directory to the environment path
if (-not $EnvPath.Contains($AbsLocation)) {
    if (-not $EnvPath.EndsWith(";")) {
        $EnvPath += ";"
    }

    $EnvPath += "${AbsLocation}"
    [Environment]::SetEnvironmentVariable("PATH", $EnvPath, $VarTarget)

    if ($?) {
        Show-Status "Added '${AbsLocation}' to the local environment path"
    }
    else {
        Show-Error "Failed to add '${AbsLocation}' to the local environment path"
    }
}

Show-Status "SvcScan installation complete, please restart your console"

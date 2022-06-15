<#
.SYNOPSIS
    SvcScan application installer.
.DESCRIPTION
    Installer script for the SvcScan network scanner application.
.LINK
    Application repository: https://github.com/vandavey/SvcScan
.EXAMPLE
    setup.ps1
    Install SvcScan (64-bit) to the 'Program Files' directory.
.EXAMPLE
    setup.ps1 -Architecture x86
    Install SvcScan (32-bit) to the 'Program Files (x86)' directory.
.EXAMPLE
    setup.ps1 -Architecture x64
    Install SvcScan (64-bit) to the 'Program Files' directory.
#>
using namespace System.IO
using namespace System.Runtime.InteropServices
using namespace System.Security.Principal

[CmdletBinding(DefaultParameterSetName="ByValue")]
param (
    [Parameter(
        Position=0,
        ParameterSetName="ByValue",
        ValueFromPipeline,
        ValueFromRemainingArguments
    )]
    [Alias("a", "p", "Arch", "Platform")]
    [ValidateSet("x64", "x86")]
    [string] $Architecture = "x64"
)

# Print an error message to stderr and exit
function PrintError {
    $Symbol = "[x]"

    if ($PSVersionTable.PSVersion.Major -ge 7) {
        $Symbol = "`e[91m${Symbol}`e[0m"
    }
    [Console]::Error.WriteLine("${Symbol} ${args}`n")
    exit 1
}

# Print an informational message to stdout
function PrintStatus {
    $Symbol = "[*]"

    if ($PSVersionTable.PSVersion.Major -ge 7) {
        $Symbol = "`e[96m${Symbol}`e[0m"
    }
    Write-Output "${Symbol} ${args}"
}

# Only Windows operating systems are supported
if (-not [RuntimeInformation]::IsOSPlatform([OSPlatform]::Windows)) {
    PrintError "SvcScan only supports Windows operating systems"
}

$AdminRole = [WindowsBuiltInRole]::Administrator
$UserPrincipal = New-Object WindowsPrincipal([WindowsIdentity]::GetCurrent())

# Admin privileges are required
if (-not $UserPrincipal.IsInRole($AdminRole)) {
    PrintError "The installer must be run as an administrator"
}

$RawRepoRoot = "https://raw.githubusercontent.com/vandavey/SvcScan/main"

# Determine installation path from given architecture
if ($Architecture -eq "x64") {
    $Location = $env:ProgramFiles
    $ZipUri = "${RawRepoRoot}/SvcScan/bin/Publish/Zips/SvcScan_Win-x64.zip"
}
else {
    $Location = ${env:ProgramFiles(x86)}
    $ZipUri = "${RawRepoRoot}/SvcScan/bin/Publish/Zips/SvcScan_Win-x86.zip"
}

$AbsLocation = "${Location}\SvcScan"
$AbsZipLocation = "${AbsLocation}\SvcScan.zip"

# Remove the existing installation directory
if (Test-Path $AbsLocation) {
    Remove-Item $AbsLocation -Recurse -Force 3>&1> $null
    PrintStatus "Removed existing installation: '${AbsLocation}'"
}

New-Item $AbsLocation -ItemType Directory 3>&1> $null
PrintStatus "Downloading executable zip archive to '${AbsZipLocation}'..."

# Download the executable archive
try {
    Invoke-WebRequest $ZipUri -OutFile $AbsZipLocation 3>&1> $null
}
catch {
    PrintError $Error[0].Exception.Message
}

PrintStatus "Unpacking zip file contents to '$AbsLocation'..."
Expand-Archive $AbsZipLocation $AbsLocation 3>&1> $null

Remove-Item $AbsZipLocation
PrintStatus "Removed temporary file '$AbsZipLocation'"

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
        PrintStatus "Added '${AbsLocation}' to the local environment path"
    }
    else {
        PrintError "Failed to add '${AbsLocation}' to the local environment path"
    }
}

PrintStatus "SvcScan installation complete, please restart your console"

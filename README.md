<div align="center">
    <img src="src/SvcScan/assets/mainicon.ico" width=175px alt="logo" />
</div>

# SvcScan

<div align="left">
    <img src="https://img.shields.io/badge/c%2B%2B-v20-f34b7d" alt="cpp-20-badge" />
    <img src="https://img.shields.io/github/issues-pr/vandavey/SvcScan" alt="pull-requests-badge" />
    <img src="https://img.shields.io/github/contributors/vandavey/SvcScan?color=blue" alt="contributors-badge" />
    <img src="https://img.shields.io/github/license/vandavey/SvcScan" alt="license-badge" />
</div>

Network service scanner application written in C++.

***

## Overview

SvcScan is a network port scanner that uses TCP network
sockets to perform targeted service scanning (C++ 20).

### Features

* Multithreaded port scanning
* TCP socket banner grabbing
* SSL/TLS encrypted communications
* HTTP/HTTPS server probing
* Concurrent network connections
* Plain text and JSON scan reports

***

## Basic Usage

### Primary Syntax

```powershell
svcscan.exe [OPTIONS] TARGET
```

### Secondary Syntax

```powershell
svcscan.exe [OPTIONS] TARGET PORT
```

***

## Command-Line Options

All available SvcScan command-line arguments are listed below:

| Argument           | Type       | Description                         | Default                    |
|:------------------:|:----------:|:-----------------------------------:|:--------------------------:|
| `TARGET`           | *Required* | Target address or host name         | *N/A*                      |
| `-p/--port PORT`   | *Required* | Target ports (*comma delimited*)    | *N/A*                      |
| `-v, --verbose`    | *Optional* | Enable verbose console output       | *False*                    |
| `-s, --ssl`        | *Optional* | Enable SSL/TLS communications       | *False*                    |
| `-j, --json`       | *Optional* | Output the scan results as JSON     | *False*                    |
| `-o/--output PATH` | *Optional* | Write the scan results to a file    | *N/A*                      |
| `-t/--timeout MS`  | *Optional* | Connection timeout in milliseconds  | *3500*                     |
| `-T/--threads NUM` | *Optional* | Scanner thread pool thread count    | *Local thread count or 16* |
| `-c/--curl [URI]`  | *Optional* | Use HTTP method GET instead of HEAD | */*                        |
| `-h/-?, --help`    | *Optional* | Display the help menu and exit      | *False*                    |

> See the [Usage Examples](#usage-examples) section for more information.

***

## Installation

SvcScan can be automatically configured and installed or
updated using the installer in the [tools](tools) directory.

It can be installed manually by building from source or using the precompiled
standalone executables in the [Zips](src/SvcScan/bin/Zips) directory.

### Automatic Setup

Download and execute the [svcscan-install.ps1](tools/svcscan-install.ps1) installer script using PowerShell:

```powershell
irm -d "https://raw.githubusercontent.com/vandavey/SvcScan/main/tools/svcscan-install.ps1" | powershell -
```

> [svcscan-install.ps1](tools/svcscan-install.ps1) only supports *x64*
  and *x86* architectures and must be executed as an administrator.

### Manual Setup

SvcScan can be manually installed using the following precompiled standalone executables:

* [Windows-x64](https://raw.githubusercontent.com/vandavey/SvcScan/main/src/SvcScan/bin/Zips/SvcScan_Win-x64.zip)
* [Windows-x86](https://raw.githubusercontent.com/vandavey/SvcScan/main/src/SvcScan/bin/Zips/SvcScan_Win-x86.zip)

It can be built from source by compiling [SvcScan.vcxproj](src/SvcScan/SvcScan.vcxproj)
after installing the dependencies detailed in the [Dependencies](#dependencies) section.

***

## Uninstallation

SvcScan can be uninstalled automatically using the uninstaller in the [tools](tools) directory.

It can be uninstalled manually by deleting the install
directory and removing it from the local environment path.

### Automatic Removal

Execute the [svcscan-uninstall.ps1](tools/svcscan-uninstall.ps1) uninstaller script using PowerShell:

```powershell
gc "${env:ProgramFiles}\SvcScan\svcscan-uninstall.ps1" | powershell -
```

> [svcscan-uninstall.ps1](tools/svcscan-uninstall.ps1) only supports
  *x64* and *x86* architectures and must be executed as an administrator.

***

## Usage Examples

### Basic Examples

Print the application help menu, then exit:

```powershell
svcscan.exe --help
```

Scan port `80` against `localhost`:

```powershell
svcscan.exe -p 80 localhost
```

Scan ports `22` through `25` against `joe-mama`:

```powershell
svcscan.exe joe-mama 22-25
```

### Advanced Examples

Scan ports `443` and `6667` against `192.168.1.1` using an
SSL/TLS capable scanner and display verbose scan output:

```powershell
svcscan.exe -vsp 443,6667 192.168.1.1
```

Scan ports `80`, `443`, and `20` through `40` against `localhost` using a thread
pool with `8` threads and set the connection timeout to `4000` milliseconds:

```powershell
svcscan.exe -t 4000 -T 8 -p 80,443,20-40 localhost
```

> The default thread pool size will be used if
  the specified thread count is greater than 32.

Scan ports `6667` and `6697` against `192.168.1.100` with verbose output displayed
and save a JSON scan report to file path `svcscan-test.json`:

```powershell
svcscan.exe -vjo svcscan-test.json 192.168.1.100 6667,6697
```

Scan port `80` against `10.0.0.1` and perform HTTP or
HTTPS probing using method `GET` and URI `/admin`:

```powershell
svcscan.exe --ssl --verbose --curl /admin 10.0.0.1 80
```

> `-c`/`--curl` must be passed as the final command-line
  argument when no explicit URI is provided.

***

## Dependencies

To run the prebuilt application executable, no dependencies are required.

To compile SvcScan, the following [Boost](https://www.boost.org/) C++ libraries
and their dependencies must be installed through [vcpkg](https://github.com/Microsoft/vcpkg)
using triplets `x64-windows-static` and `x86-windows-static`:

* [Boost.Asio](https://www.boost.org/doc/libs/1_86_0/doc/html/boost_asio.html)
  > Library for networking and other low level I/O functionality.

* [Boost.Beast](https://github.com/boostorg/beast)
  > Library for HTTP, WebSocket, and networking functionality.

* [Boost.Bind](https://www.boost.org/doc/libs/1_86_0/libs/bind/doc/html/bind.html)
  > Library for generating forwarding call wrappers.

* [Boost.JSON](https://www.boost.org/doc/libs/1_86_0/libs/json/doc/html/index.html)
  > Library for JSON parsing, serialization, and DOM.

Once [vcpkg](https://github.com/Microsoft/vcpkg) is installed, the following command
can be used to install all required [Boost](https://www.boost.org/) libraries:

```powershell
vcpkg.exe install "boost:x64-windows-static" "boost:x86-windows-static"
```

***

## Remarks

* This application currently only supports Windows operating systems.
* Please use discretion as this application is still in development.

***

## Copyright & Licensing

SvcScan is licensed under the [MIT license](LICENSE.md) and officially
hosted in [this](https://github.com/vandavey/SvcScan) repository.

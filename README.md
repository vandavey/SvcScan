<p align="center">
    <img src="SvcScan/assets/mainicon.ico" width=175 alt="logo">
</p>

# SvcScan

Network service scanner application. Supports raw TCP socket banner
grabbing and HTTP service enumeration (C++ 20).

***

## Basic Usage

There are two acceptable usage formats available to pass command-line
arguments to the application.

### Primary Usage Format

```powershell
svcscan.exe [OPTIONS] TARGET
```

### Secondary Usage Format

```powershell
svcscan.exe [OPTIONS] TARGET PORT
```

***

## Available Arguments

All available SvcScan arguments are listed in the following table:

| Argument         | Type       | Description                        | Default |
|:----------------:|:----------:|:----------------------------------:|:-------:|
|`TARGET`          | *Required* | Target address or host name        | *N/A*   |
|`-p/--port PORT`  | *Required* | Target ports (*comma delimited*)   | *N/A*   |
|`-t/--timeout MS` | *Optional* | Connection timeout in milliseconds | *3500*  |
|`-o/--output PATH`| *Optional* | Write scan output to file          | *N/A*   |
|`-v, --verbose`   | *Optional* | Enable verbose console output      | *False* |
|`-h/-?, --help`   | *Optional* | Display the help menu and exit     | *False* |

***

## Usage Examples

### Primary Format Example

Scan ports `22` through `25` against `fake.address`:

```powershell
svcscan.exe -p 22-25 fake.address
```

### Secondary Format Example

Scan port `80` against `localhost` and display verbose scan output:

```powershell
svcscan.exe --verbose localhost 80
```

***

## Download Options

* A prebuilt, standalone executable can be downloaded
  [here](https://raw.githubusercontent.com/vandavey/SvcScan/main/x64/Zips/SvcScan_Win-x64.zip).

* The entire SvcScan source code repository can be downloaded
  [here](https://github.com/vandavey/SvcScan/archive/main.zip).

***

## Dependencies

To run the prebuilt application executable, no dependencies are required.

To compile this application, the following
[Boost](https://www.boost.org/) C++ libraries and their dependencies must be
installed through [vcpkg](https://github.com/Microsoft/vcpkg):

* [Boost.Algorithm](https://www.boost.org/doc/libs/1_79_0/libs/algorithm/doc/html/index.html)
  > Library for various general purpose algorithms.

* [Boost.Asio](https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio.html)
  > Library for networking and other low level I/O functionality.

* [Boost.Beast](https://github.com/boostorg/beast)
  > Library for HTTP, WebSocket, and networking functionality.

***

## Remarks

* This application only supports Windows operating systems.
* Please use discretion, as this application is still in development.

***

## Copyright & Licensing

The SvcScan application source code is available
[here](https://github.com/vandavey/SvcScan) and licensed
under the [MIT license](LICENSE.md).

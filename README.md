# SvcScan

TCP network port scanner and banner grabber (C++ 17).

***

## Basic Usage

There are two acceptable usage formats available to pass command-line
arguments to the application.

* Primary format:

    ```powershell
    svcscan.exe [OPTIONS] TARGET
    ```

* Secondary format:

    ```powershell
    svcscan.exe [OPTIONS] TARGET PORT
    ```

***

## Available Arguments

All available SvcScan arguments are listed in the following table:

| Argument         | Type       | Description                      | Default |
|:----------------:|:----------:|:--------------------------------:|:-------:|
|`TARGET`          | *Required* | Target address or host           | *N/A*   |
|`-p/--port PORT`  | *Required* | Target ports (*comma delimited*) | *N/A*   |
|`-v, --verbose`   | *Optional* | Enable verbose console output    | *False* |
|`-h/-?, --help`   | *Optional* | Display the help menu            | *False* |

***

## Download Options

* A prebuilt, standalone executable can be downloaded
[here](https://raw.githubusercontent.com/vandavey/SvcScan/main/x64/Zips/SvcScan_Win-x64.zip).
* The entire SvcScan source code repository can be downloaded
[here](https://github.com/vandavey/SvcScan/archive/main.zip).

***

## Remarks

* This application only supports Windows operating systems.
* Please use discretion, as this application is still in development.

***

## Copyright & Licensing

The SvcScan application source code is available
[here](https://github.com/vandavey/SvcScan) and licensed
under the [MIT license](LICENSE.md).

# SvcScan

TCP application banner grabber (C++ 17)

***

## Basic Usage

```powershell
svcscan.exe [-h] [-p] PORT TARGET
```

***

## Available Arguments

All available SvcScan arguments are listed in the following table:

| Argument         | Type       | Description                      | Default |
|:----------------:|:----------:|:--------------------------------:|:-------:|
| `TARGET`         | *Required* | Target address or host           | *N/A*   |
| `-p/--port PORT` | *Required* | Target ports (*comma delimited*) | *N/A*   |
| `-h, --help`     | *Optional* | Display the help menu            | *False* |

***

## Remarks

* Please use discretion, as this application is still in development.

***

## Copyright & Licensing

The SvcScan application source code is available
[here](https://github.com/vandavey/SvcScan) and licensed
under the [MIT license](LICENSE.md).

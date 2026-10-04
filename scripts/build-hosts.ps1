# Host build of the three Slowduino targets. Does not upload.
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$pio = Join-Path $env:LOCALAPPDATA "Python\pythoncore-3.14-64\Scripts\pio.exe"
if (-not (Test-Path $pio)) {
  $pio = "pio"
}
Push-Location $root
try {
  & $pio run -e uno -e bluepill -e uno_r4_minima
  exit $LASTEXITCODE
} finally {
  Pop-Location
}

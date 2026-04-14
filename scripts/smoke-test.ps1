# smoke-test.ps1 - Quick dry-run of the VST/Standalone to catch startup crashes.
# Launches the standalone for 3 seconds; non-zero exit that isn't the timeout kill
# indicates a crash. Exits with code 1 on any crash, 0 on success.

$ErrorActionPreference = "Stop"

$StandaloneExe = Join-Path $PSScriptRoot "..\build\GBCSynth_artefacts\Release\Standalone\GBC Synth.exe"

if (-not (Test-Path $StandaloneExe)) {
    Write-Error "Standalone not built at $StandaloneExe. Run cmake --build build --config Release first."
    exit 1
}

Write-Host "=== GBC Synth Smoke Test ===" -ForegroundColor Cyan
Write-Host "Launching: $StandaloneExe" -ForegroundColor Gray

$proc = Start-Process -FilePath $StandaloneExe -PassThru -WindowStyle Hidden
Start-Sleep -Seconds 3

if ($proc.HasExited) {
    $exitCode = $proc.ExitCode
    Write-Host "CRASH: process exited after launch with code 0x$($exitCode.ToString('X8'))" -ForegroundColor Red

    switch ($exitCode) {
        -1073741819 { Write-Host "  Access violation (0xC0000005): null pointer or bad read" -ForegroundColor Red }
        -1073741676 { Write-Host "  Integer divide by zero (0xC0000094)" -ForegroundColor Red }
        -1073741571 { Write-Host "  Stack overflow (0xC00000FD)" -ForegroundColor Red }
    }
    exit 1
}

Write-Host "OK: process is still running after 3 seconds, no startup crash" -ForegroundColor Green

Stop-Process -Id $proc.Id -Force -ErrorAction SilentlyContinue
exit 0

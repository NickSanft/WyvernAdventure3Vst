# install-vst.ps1 — Copy GBC Synth VST3 to FL Studio's plugin directories
# Run as Administrator: powershell -ExecutionPolicy Bypass -File scripts\install-vst.ps1

$ErrorActionPreference = "Stop"

$BuildDir = Join-Path $PSScriptRoot "..\build\GBCSynth_artefacts\Release\VST3"
$Vst3Bundle = Join-Path $BuildDir "GBC Synth.vst3"
$Vst3Dll = Join-Path $BuildDir "GBC Synth.vst3\Contents\x86_64-win\GBC Synth.vst3"

# Standard VST3 install location
$Vst3SystemDir = "C:\Program Files\Common Files\VST3"

# FL Studio custom VST directories (common locations)
$FlStudioVstDirs = @(
    "C:\Program Files\Common Files\VST3",
    "C:\Program Files\VstPlugins",
    "C:\Program Files (x86)\VstPlugins"
)

if (-not (Test-Path $Vst3Bundle)) {
    Write-Error "VST3 bundle not found at $Vst3Bundle. Run cmake --build build --config Release first."
    exit 1
}

Write-Host "=== GBC Synth VST3 Installer ===" -ForegroundColor Green
Write-Host ""

# 1. Install the VST3 bundle to the standard location
Write-Host "Installing VST3 bundle to $Vst3SystemDir ..." -ForegroundColor Yellow
if (-not (Test-Path $Vst3SystemDir)) {
    New-Item -ItemType Directory -Path $Vst3SystemDir -Force | Out-Null
}
$destBundle = Join-Path $Vst3SystemDir "GBC Synth.vst3"
if (Test-Path $destBundle) {
    Remove-Item -Recurse -Force $destBundle
}
Copy-Item -Recurse $Vst3Bundle $destBundle
Write-Host "  -> Installed bundle: $destBundle" -ForegroundColor Green

# 2. Also copy the flat DLL as .dll for legacy FL Studio scanning
$flatDll = Join-Path $Vst3SystemDir "GBC Synth.dll"
Copy-Item $Vst3Dll $flatDll -Force
Write-Host "  -> Installed flat DLL: $flatDll" -ForegroundColor Green

Write-Host ""
Write-Host "Done! Open FL Studio and scan for plugins:" -ForegroundColor Green
Write-Host "  Options -> File Settings -> Manage Plugins -> Find installed plugins -> Start scan"
Write-Host ""
Write-Host "Look for 'GBC Synth' in Browser -> Plugin Database -> Generators"

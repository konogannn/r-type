#Requires -RunAsAdministrator

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "  R-Type Dependencies Installer (Windows) " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

function Print-Ok { param([string]$M) Write-Host "✓ $M" -ForegroundColor Green }
function Print-Err { param([string]$M) Write-Host "✗ $M" -ForegroundColor Red }
function Print-Info { param([string]$M) Write-Host "ℹ $M" -ForegroundColor Yellow }

function Test-Cmd { param([string]$C) try { $null = Get-Command $C -ErrorAction Stop; return $true } catch { return $false } }

try {
    if (-not (Test-Cmd choco)) {
        Print-Info "Installing Chocolatey..."
        Set-ExecutionPolicy Bypass -Scope Process -Force
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        Print-Ok "Chocolatey installed"
    }

    if (-not (Test-Cmd cmake)) {
        Print-Info "Installing CMake..."
        choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        Print-Ok "CMake installed"
    }

    if (-not (Test-Cmd git)) {
        Print-Info "Installing Git..."
        choco install git -y
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        Print-Ok "Git installed"
    }

    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere) -or -not (& $vswhere -latest -property installationPath)) {
        Print-Info "Installing VS Build Tools (this takes time)..."
        choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --passive" -y
        Print-Ok "VS Build Tools installed"
    }

    Write-Host ""
    Print-Ok "Dependencies installed!"
    Print-Info "Restart terminal then build: cmake -S . -B build && cmake --build build --config Release"
    Write-Host ""
}
catch {
    Print-Err "Error: $_"
    exit 1
}

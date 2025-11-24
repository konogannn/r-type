#Requires -RunAsAdministrator

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "  vcpkg Installation Script (Windows)     " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

function Print-Ok { param([string]$M) Write-Host "✓ $M" -ForegroundColor Green }
function Print-Err { param([string]$M) Write-Host "✗ $M" -ForegroundColor Red }
function Print-Info { param([string]$M) Write-Host "ℹ $M" -ForegroundColor Yellow }

$VcpkgDir = "$env:USERPROFILE\.vcpkg"

if (Test-Path $VcpkgDir) {
    Print-Info "vcpkg already at: $VcpkgDir"
    $update = Read-Host "Update? (y/N)"
    if ($update -eq "y") {
        Set-Location $VcpkgDir
        git pull
        .\bootstrap-vcpkg.bat
        Print-Ok "Updated"
    }
}
else {
    try {
        $null = Get-Command git -ErrorAction Stop
        Print-Ok "Git found"
    }
    catch {
        Print-Info "Installing Git..."
        try { $null = Get-Command choco -ErrorAction Stop }
        catch {
            Set-ExecutionPolicy Bypass -Scope Process -Force
            [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
            Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
            $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        }
        choco install git -y
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        Print-Ok "Git installed"
    }

    Print-Info "Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg.git $VcpkgDir

    Print-Info "Bootstrapping..."
    Set-Location $VcpkgDir
    .\bootstrap-vcpkg.bat
    Print-Ok "vcpkg installed"
}

$currentVcpkgRoot = [System.Environment]::GetEnvironmentVariable("VCPKG_ROOT", "User")
if ($currentVcpkgRoot -ne $VcpkgDir) {
    [System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", $VcpkgDir, "User")
    $env:VCPKG_ROOT = $VcpkgDir
    Print-Ok "VCPKG_ROOT set"
}

$currentPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
if ($currentPath -notlike "*$VcpkgDir*") {
    [System.Environment]::SetEnvironmentVariable("Path", "$currentPath;$VcpkgDir", "User")
    $env:Path += ";$VcpkgDir"
    Print-Ok "Added to PATH"
}

Write-Host ""
Print-Ok "Setup complete!"
Print-Info "Restart terminal then build: cmake -S . -B build && cmake --build build --config Release"
Write-Host ""

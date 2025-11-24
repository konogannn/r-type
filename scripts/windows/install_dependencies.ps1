# R-Type Dependencies Installation Script for Windows
# This script installs required dependencies using Chocolatey

# Requires Administrator privileges
#Requires -RunAsAdministrator

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "  R-Type Dependencies Installer (Windows) " -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# Function to print colored messages
function Print-Success {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Print-Error {
    param([string]$Message)
    Write-Host "✗ $Message" -ForegroundColor Red
}

function Print-Info {
    param([string]$Message)
    Write-Host "ℹ $Message" -ForegroundColor Yellow
}

# Check if Chocolatey is installed
function Test-Chocolatey {
    try {
        $null = Get-Command choco -ErrorAction Stop
        return $true
    }
    catch {
        return $false
    }
}

# Install Chocolatey
function Install-Chocolatey {
    Print-Info "Chocolatey not found. Installing Chocolatey..."
    
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    
    try {
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
        
        # Refresh environment variables
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        
        Print-Success "Chocolatey installed successfully"
        return $true
    }
    catch {
        Print-Error "Failed to install Chocolatey: $_"
        return $false
    }
}

# Check if a command exists
function Test-Command {
    param([string]$Command)
    try {
        $null = Get-Command $Command -ErrorAction Stop
        return $true
    }
    catch {
        return $false
    }
}

# Main installation process
try {
    # Install Chocolatey if needed
    if (-not (Test-Chocolatey)) {
        if (-not (Install-Chocolatey)) {
            Print-Error "Cannot proceed without Chocolatey"
            exit 1
        }
    }
    else {
        Print-Success "Chocolatey is already installed"
    }

    Write-Host ""

    # Install CMake
    if (-not (Test-Command cmake)) {
        Print-Info "Installing CMake..."
        choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
        
        # Refresh environment
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        
        Print-Success "CMake installed"
    }
    else {
        $cmakeVersion = (cmake --version | Select-Object -First 1)
        Print-Success "CMake already installed ($cmakeVersion)"
    }

    # Install Git (needed for FetchContent)
    if (-not (Test-Command git)) {
        Print-Info "Installing Git..."
        choco install git -y
        
        # Refresh environment
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
        
        Print-Success "Git installed"
    }
    else {
        Print-Success "Git already installed"
    }

    # Install Visual Studio Build Tools
    Print-Info "Checking for Visual Studio Build Tools..."
    
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    
    if (Test-Path $vswhere) {
        $vsPath = & $vswhere -latest -property installationPath
        if ($vsPath) {
            Print-Success "Visual Studio Build Tools found at: $vsPath"
        }
        else {
            Print-Info "Installing Visual Studio Build Tools..."
            choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --passive" -y
            Print-Success "Visual Studio Build Tools installed"
        }
    }
    else {
        Print-Info "Installing Visual Studio Build Tools..."
        Print-Info "This may take several minutes..."
        choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --passive" -y
        Print-Success "Visual Studio Build Tools installed"
    }

    Write-Host ""
    Write-Host "==========================================" -ForegroundColor Cyan
    Print-Success "Dependencies installed successfully!"
    Write-Host "==========================================" -ForegroundColor Cyan
    Write-Host ""
    
    Print-Info "SFML will be downloaded automatically by CMake when you build."
    Write-Host ""
    Print-Info "To build the project:"
    Write-Host "  cmake -S . -B build" -ForegroundColor White
    Write-Host "  cmake --build build --config Release" -ForegroundColor White
    Write-Host ""
    Print-Info "The project uses FetchContent to download SFML automatically."
    Write-Host ""
    Print-Info "Please restart your terminal to ensure all environment variables are loaded."
    Write-Host ""
}
catch {
    Print-Error "An error occurred during installation: $_"
    exit 1
}

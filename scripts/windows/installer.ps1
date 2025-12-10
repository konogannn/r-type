# installer.ps1
# Main script to set up the environment and build R-Type on Windows
$ErrorActionPreference = "Stop"

# Define colors for output
$MAGENTA = "Magenta"
$GREEN = 'Green'
$RED = 'Red'
$YELLOW = 'Yellow'

function Print-Header {
    param(
        [Parameter(Mandatory=$true)][string]$Title
    )
    Write-Host "==========================================" -ForegroundColor $MAGENTA
    Write-Host "  $Title" -ForegroundColor $MAGENTA
    Write-Host "==========================================" -ForegroundColor $MAGENTA
}

$SCRIPTS_DIR = (Split-Path -Path $MyInvocation.MyCommand.Definition -Parent)

Print-Header "STARTING R-TYPE SETUP AND BUILD (Windows)"

try {
    # 1. Install/check system dependencies (Git, CMake)
    & "$SCRIPTS_DIR\install_dependencies.ps1"
    if (-not $?) {
        throw "Failed to install dependencies"
    }
    
    # 2. Initialize and prepare Vcpkg submodule
    & "$SCRIPTS_DIR\setup_vcpkg.ps1"
    if (-not $?) {
        throw "Failed to set up Vcpkg"
    }
    
    # 3. Configure CMake and build the project
    & "$SCRIPTS_DIR\build_project.ps1"
    if (-not $?) {
        throw "Failed to build the project"
    }
    
    Write-Host ""
    Print-Header "PROCESS COMPLETED SUCCESSFULLY"

} catch {
    Write-Host ""
    Write-Host "==========================================" -ForegroundColor $RED
    Write-Host "  ERROR: The process failed." -ForegroundColor $RED
    Write-Host "  Please check the error messages above." -ForegroundColor $RED
    Write-Host "==========================================" -ForegroundColor $RED
    exit 1
}
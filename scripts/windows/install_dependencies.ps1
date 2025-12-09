# install_system_deps.ps1
# Checks for mandatory system dependencies (Git, CMake).
$ErrorActionPreference = "Stop"

$GREEN = 'Green'
$RED = 'Red'
$YELLOW = 'Yellow'

function Write-OK { Write-Host "✓ $($args[0])" -ForegroundColor $GREEN }
function Write-Err { Write-Host "✗ $($args[0])" -ForegroundColor $RED; exit 1 }
function Write-Info { Write-Host "ℹ $($args[0])" -ForegroundColor $YELLOW }

Write-Host ""
Write-Info "INSTALLING SYSTEM DEPENDENCIES"
Write-Host "=========================================="

Write-Info "PREREQUISITE CHECK: Ensure 'Visual Studio Build Tools' (C++ SDK) are installed."
Write-Host ""

# Check for Git
$gitPath = Get-Command git -ErrorAction SilentlyContinue
if (-not $gitPath) {
    Write-Err "Git is not found. Please install it manually (e.g., https://git-scm.com/download/win)."
} else {
    Write-OK "Git is found."
}

# Check for CMake
$cmakePath = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmakePath) {
    Write-Err "CMake is not found. Please install it manually and ensure it is in the PATH."
} else {
    Write-OK "CMake is found."
}

# install_system_deps.ps1
# Checks for mandatory system dependencies (Git, CMake).
$ErrorActionPreference = "Stop"

$GREEN = 'Green'
$RED = 'Red'
$YELLOW = 'Yellow'

function Print-OK { Write-Host "✓ $($args[0])" -ForegroundColor $GREEN }
function Print-Err { Write-Host "✗ $($args[0])" -ForegroundColor $RED; exit 1 }
function Print-Info { Write-Host "ℹ $($args[0])" -ForegroundColor $YELLOW }

# Function to run a command silently and report status
function Run-Silent-Op {
    param(
        [Parameter(Mandatory=$true)][string]$Command,
        [Parameter(Mandatory=$true)][string]$Message
    )
    Write-Host "  -> $Message... " -NoNewline -ForegroundColor $YELLOW

    # Execute the command, suppressing output, and check the success
    $result = Invoke-Expression -Command "$Command 2>&1 | Out-Null"
    if ($LASTEXITCODE -eq 0) {
        Print-OK "Finished."
    } else {
        Print-Err "Failed."
    }
}

Write-Host ""
Print-Info "INSTALLING SYSTEM DEPENDENCIES"
Write-Host "=========================================="

Print-Info "PREREQUISITE CHECK: Ensure 'Visual Studio Build Tools' (C++ SDK) are installed."
Write-Host ""

# Check for Git
$gitPath = Get-Command git -ErrorAction SilentlyContinue
if (-not $gitPath) {
    Print-Err "Git is not found. Please install it manually (e.g., https://git-scm.com/download/win)."
} else {
    Print-OK "Git is found."
}

# Check for CMake
$cmakePath = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmakePath) {
    Print-Err "CMake is not found. Please install it manually and ensure it is in the PATH."
} else {
    Print-OK "CMake is found."
}
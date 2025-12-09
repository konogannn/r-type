# setup_vcpkg.ps1
# Manages the Vcpkg submodule and performs bootstrapping
$ErrorActionPreference = "Stop"

$GREEN = 'Green'
$RED = 'Red'
$YELLOW = 'Yellow'

function Print-OK { Write-Host "✓ $($args[0])" -ForegroundColor $GREEN }
function Print-Err { Write-Host "✗ $($args[0])" -ForegroundColor $RED; exit 1 }
function Print-Info { Write-Host "ℹ $($args[0])" -ForegroundColor $YELLOW }

function Run-Silent-Op {
    param(
        [Parameter(Mandatory=$true)][string]$Command,
        [Parameter(Mandatory=$true)][string]$Message
    )
    Write-Host "  -> $Message... " -NoNewline -ForegroundColor $YELLOW

    if (Invoke-Expression -Command "$Command 2>&1 | Out-Null") {
        Print-OK "Finished."
    } else {
        Print-Err "Failed."
    }
}

$VCPKG_DIR = "vcpkg"
$VCPKG_BOOTSTRAP_SCRIPT = ".\$VCPKG_DIR\bootstrap-vcpkg.bat"

Write-Host ""
Print-Info "VCPKG SUBMODULE MANAGEMENT"
Write-Host "=========================================="

# 1. Pull/update the submodule
Run-Silent-Op "git submodule update --init --recursive" "Updating/Initializing Vcpkg submodule"

# 2. Bootstrap Vcpkg (builds the vcpkg.exe executable)
if (-not (Test-Path ".\$VCPKG_DIR\vcpkg.exe")) {
    Print-Info "  -> Compiling Vcpkg executable. This may take a moment..." -NoNewline -ForegroundColor $YELLOW
    
    # Execute the .bat file. Cannot use Run-Silent-Op easily due to .bat execution.
    # We rely on the native command and check $LASTEXITCODE
    & $VCPKG_BOOTSTRAP_SCRIPT > $null
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host ""
        Print-Err "Vcpkg bootstrap failed. Exit Code: $LASTEXITCODE"
    }
    Print-OK "Finished."
} else {
    Print-OK "Vcpkg executable already exists."
}
# setup_vcpkg.ps1
# Manages the Vcpkg submodule and performs bootstrapping
$ErrorActionPreference = "Stop"

$GREEN = 'Green'
$RED = 'Red'
$YELLOW = 'Yellow'

function Write-OK { Write-Host "✓ $($args[0])" -ForegroundColor $GREEN }
function Write-Err { Write-Host "✗ $($args[0])" -ForegroundColor $RED; exit 1 }
function Write-Info { Write-Host "ℹ $($args[0])" -ForegroundColor $YELLOW }

$VCPKG_DIR = "vcpkg"
$VCPKG_BOOTSTRAP_SCRIPT = ".\$VCPKG_DIR\bootstrap-vcpkg.bat"

Write-Host ""
Write-Info "VCPKG SUBMODULE MANAGEMENT"
Write-Host "=========================================="

# 1. Pull/update the submodule
Write-Info "Updating/Initializing Vcpkg submodule"
git submodule update --init --recursive

# 2. Bootstrap Vcpkg (builds the vcpkg.exe executable)
if (-not (Test-Path ".\$VCPKG_DIR\vcpkg.exe")) {
    Write-Info "Compiling Vcpkg executable. This may take a moment..."
    Write-Host ""
    
    # Execute the .bat file with visible output
    & $VCPKG_BOOTSTRAP_SCRIPT
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host ""
        Write-Err "Vcpkg bootstrap failed. Exit Code: $LASTEXITCODE"
    }
    Write-Host ""
    Write-OK "Vcpkg executable compiled successfully"
} else {
    Write-OK "Vcpkg executable already exists."
}

# build_project.ps1
# Configures and builds the R-Type project
$ErrorActionPreference = "Stop"

$GREEN = 'Green'
$RED = 'Red'
$YELLOW = 'Yellow'

function Write-OK { Write-Host "✓ $($args[0])" -ForegroundColor $GREEN }
function Write-Err { Write-Host "✗ $($args[0])" -ForegroundColor $RED; exit 1 }
function Write-Info { Write-Host "ℹ $($args[0])" -ForegroundColor $YELLOW }

$VCPKG_DIR = "vcpkg"
$BUILD_DIR = "build"

Write-Host ""
Write-Info "CONFIGURATION AND BUILDING THE PROJECT"
Write-Host "=========================================="

# 1. Configure CMake with the Vcpkg toolchain file
# This step triggers the installation of C++ libraries via vcpkg.json.
Write-Info "Step 1/2: Configuring CMake and installing/linking C++ dependencies (Release)"
cmake -S . -G "Ninja" -B "$BUILD_DIR" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_TOOLCHAIN_FILE="$VCPKG_DIR\scripts\buildsystems\vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release

if ($LASTEXITCODE -ne 0) {
    Write-Err "CMake configuration failed."
} else {
    Write-OK "CMake configured successfully."
}

# 2. Build the project
# Using /m for parallel building on MSBuild (which CMake will use by default on Windows)
Write-Info "Step 2/2: Building the project executables (Release)"
cmake --build "$BUILD_DIR" --config Release --target r-type_server r-type_client
if ($LASTEXITCODE -ne 0) {
    Write-Err "Build failed."
} else {
    Write-OK "Project built successfully."
}

Write-Host ""
Write-OK "R-Type project built successfully!"
Write-Info "Server: .\r-type_server.exe"
Write-Info "Client: .\r-type_client.exe"
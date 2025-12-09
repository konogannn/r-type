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
$config_command = "cmake -S . -B `"$BUILD_DIR`" -DCMAKE_TOOLCHAIN_FILE=`"$VCPKG_DIR\scripts\buildsystems\vcpkg.cmake`" -DCMAKE_BUILD_TYPE=Release"
Write-Info "Step 1/2: Configuring CMake and installing/linking C++ dependencies (Release)"
cmake -S . -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$VCPKG_DIR\scripts\buildsystems\vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) {
    Write-Err "CMake configuration failed."
} else {
    Write-OK "CMake configured successfully."
}

# 2. Build the project
# Using /m for parallel building on MSBuild (which CMake will use by default on Windows)
Write-Info "Step 2/2: Building the project executables (Release)"
cmake --build "$BUILD_DIR" --config Release --target r-type_server r-type_client

Write-Host ""
Write-OK "R-Type project built successfully!"
Write-Info "Executables are located in the 'Release' directory."
Write-Info "Example Server: .\Release\r-type_server.exe"
Write-Info "Example Client: .\Release\r-type_client.exe"
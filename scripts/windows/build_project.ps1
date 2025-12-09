# build_project.ps1
# Configures and builds the R-Type project
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

    # Execute the command, suppressing output, and check the success
    if (Invoke-Expression -Command "$Command 2>&1 | Out-Null") {
        Print-OK "Finished."
    } else {
        Print-Err "Failed."
    }
}

$VCPKG_DIR = "vcpkg"
$BUILD_DIR = "build"

Write-Host ""
Print-Info "CONFIGURATION AND BUILDING THE PROJECT"
Write-Host "=========================================="

# 1. Configure CMake with the Vcpkg toolchain file
# This step triggers the installation of C++ libraries via vcpkg.json.
$config_command = "cmake -S . -B `"$BUILD_DIR`" -DCMAKE_TOOLCHAIN_FILE=`"$VCPKG_DIR\scripts\buildsystems\vcpkg.cmake`" -DCMAKE_BUILD_TYPE=Release"
Run-Silent-Op $config_command "Configuring CMake and installing/linking C++ dependencies (Release)"

# 2. Build the project
# Using /m for parallel building on MSBuild (which CMake will use by default on Windows)
$build_command = "cmake --build `"$BUILD_DIR`" --config Release -- /m" 
Run-Silent-Op $build_command "Compiling executables (Release configuration)"

Write-Host ""
Print-OK "R-Type project built successfully!"
Print-Info "Executables are located in the '$BUILD_DIR\Release' directory."
Print-Info "Example Server: .\$BUILD_DIR\Release\r-type_server.exe"
Print-Info "Example Client: .\$BUILD_DIR\Release\r-type_client.exe"
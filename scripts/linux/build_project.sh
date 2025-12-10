#!/bin/bash

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_header() {
    echo "=========================================="
    printf "%*s%s\n" $(((44 - ${#1}) / 2)) "" "$1"
    echo "=========================================="
}
print_ok() { echo -e "${GREEN}✓ $1${NC}"; }
print_err() { echo -e "${RED}✗ $1${NC}"; }
print_info() { echo -e "${YELLOW}ℹ $1${NC}"; }

VCPKG_DIR="vcpkg"
BUILD_DIR="build"

print_header "Build Project"

# 1. Configure CMake with the Vcpkg toolchain file
# Vcpkg reads the vcpkg.json and automatically installs dependencies
print_info "Configuring CMake (may install C++ dependencies via Vcpkg)."
cmake -S . -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"

# 2. Build the project
print_info "Building the project."
cmake --build "$BUILD_DIR" --config Release --target r-type_server r-type_client

print_ok "R-Type project built successfully!"
echo ""
print_info "Server: ./r-type_server"
print_info "Client: ./r-type_client"

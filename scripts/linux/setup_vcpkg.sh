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
VCPKG_BOOTSTRAP_SCRIPT="$VCPKG_DIR/bootstrap-vcpkg.sh"

print_header "Vcpkg Setup"

print_info "Managing Vcpkg submodule..."

# 1. Pull/update the submodule (requires Git, installed in the previous step)
if [ ! -d "$VCPKG_DIR" ]; then
    print_info "Vcpkg submodule not found. Adding it now..."
    git submodule add -f https://github.com/Microsoft/vcpkg.git vcpkg
fi
 
git submodule update --init --recursive
print_ok "Vcpkg submodule updated/initialized."
 
# 2. Bootstrap Vcpkg (builds the vcpkg executable)
if [ ! -f "$VCPKG_DIR/vcpkg" ]; then
    print_info "Starting Vcpkg bootstrap. This may take a moment."
    "$VCPKG_BOOTSTRAP_SCRIPT"
    print_ok "Vcpkg executable created."
else
    print_ok "Vcpkg executable already exists."
fi

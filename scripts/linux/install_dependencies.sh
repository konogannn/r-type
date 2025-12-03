#!/bin/bash
# This script requires Bash. Do not run with sh or other shells.
set -e

echo "=========================================="
echo "  R-Type Dependencies Installer (Linux)   "
echo "=========================================="

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_ok() { echo -e "${GREEN}✓ $1${NC}"; }
print_err() { echo -e "${RED}✗ $1${NC}"; }
print_info() { echo -e "${YELLOW}ℹ $1${NC}"; }

if [ "$EUID" -eq 0 ]; then
    print_err "Do not run as root"
    exit 1
fi

if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
else
    print_err "Cannot detect distribution"
    exit 1
fi

print_info "Distribution: $DISTRO"

cmd_exists() { command -v "$1" >/dev/null 2>&1; }

case "$DISTRO" in
    ubuntu|debian|linuxmint|pop)
        sudo apt update
        cmd_exists cmake || sudo apt install -y cmake
        sudo apt install -y build-essential g++ libsfml-dev lcov
        ;;
    fedora|rhel|centos|rocky|almalinux)
        cmd_exists cmake || sudo dnf install -y cmake
        sudo dnf install -y gcc-c++ make SFML-devel lcov
        ;;
    arch|manjaro)
        sudo pacman -Sy
        cmd_exists cmake || sudo pacman -S --noconfirm cmake
        sudo pacman -S --noconfirm base-devel sfml lcov
        ;;
    opensuse*|suse)
        cmd_exists cmake || sudo zypper install -y cmake
        sudo zypper install -y gcc-c++ make libsfml2 sfml2-devel lcov
        ;;
    *)
        print_err "Unsupported: $DISTRO"
        print_info "Install manually: CMake 3.11+, GCC/G++, SFML 2.5+, lcov"
        exit 1
        ;;
esac

echo ""
print_ok "Dependencies installed!"
print_info "Build: cmake -S . -B build && cmake --build build"
echo ""

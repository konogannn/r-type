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
cmd_exists() { command -v "$1" >/dev/null 2>&1; }

print_header "Installing System Dependencies"

if [ "$EUID" -eq 0 ]; then
    print_err "Please run this script as a non-root user."
    exit 1
fi

if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
    print_info "Distribution: $DISTRO"
else
    print_err "Unable to determine Linux distribution."
    exit 1
fi

case "$DISTRO" in
    ubuntu|debian|linuxmint|pop)
        sudo apt update
        cmd_exists cmake || sudo apt install -y cmake
        sudo apt install -y build-essential git
        ;;
    fedora|rhel|centos|rocky|almalinux)
        cmd_exists cmake || sudo dnf install -y cmake
        sudo dnf install -y gcc-c++ make git
        ;;
    arch|manjaro)
        sudo pacman -Sy
        cmd_exists cmake || sudo pacman -S --noconfirm cmake
        sudo pacman -S --noconfirm base-devel git
        ;;
    *)
        print_err "Unsupported distribution: $DISTRO"
        print_info "Please install manually: Git, CMake 3.11+, GCC/G++"
        exit 1
        ;;
esac

print_ok "System dependencies installed."
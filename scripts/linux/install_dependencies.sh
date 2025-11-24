#!/bin/bash

# R-Type Dependencies Installation Script for Linux
# This script detects your distribution and installs required dependencies

set -e  # Exit on error

echo "=========================================="
echo "  R-Type Dependencies Installer (Linux)  "
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored messages
print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ $1${NC}"
}

# Check if running as root
if [ "$EUID" -eq 0 ]; then 
    print_error "Please do not run this script as root or with sudo"
    echo "The script will ask for sudo password when needed"
    exit 1
fi

# Detect distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
else
    print_error "Cannot detect Linux distribution"
    exit 1
fi

print_info "Detected distribution: $DISTRO"
echo ""

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install dependencies based on distribution
case "$DISTRO" in
    ubuntu|debian|linuxmint|pop)
        print_info "Installing dependencies for Debian/Ubuntu-based system..."
        
        sudo apt update
        
        # Install CMake
        if ! command_exists cmake; then
            print_info "Installing CMake..."
            sudo apt install -y cmake
            print_success "CMake installed"
        else
            print_success "CMake already installed ($(cmake --version | head -n1))"
        fi
        
        # Install build tools
        print_info "Installing build tools..."
        sudo apt install -y build-essential g++
        print_success "Build tools installed"
        
        # Install SFML
        print_info "Installing SFML..."
        sudo apt install -y libsfml-dev
        print_success "SFML installed"
        ;;
        
    fedora|rhel|centos|rocky|almalinux)
        print_info "Installing dependencies for Fedora/RHEL-based system..."
        
        # Install CMake
        if ! command_exists cmake; then
            print_info "Installing CMake..."
            sudo dnf install -y cmake
            print_success "CMake installed"
        else
            print_success "CMake already installed ($(cmake --version | head -n1))"
        fi
        
        # Install build tools
        print_info "Installing build tools..."
        sudo dnf install -y gcc-c++ make
        print_success "Build tools installed"
        
        # Install SFML
        print_info "Installing SFML..."
        sudo dnf install -y SFML-devel
        print_success "SFML installed"
        ;;
        
    arch|manjaro)
        print_info "Installing dependencies for Arch-based system..."
        
        sudo pacman -Sy
        
        # Install CMake
        if ! command_exists cmake; then
            print_info "Installing CMake..."
            sudo pacman -S --noconfirm cmake
            print_success "CMake installed"
        else
            print_success "CMake already installed ($(cmake --version | head -n1))"
        fi
        
        # Install build tools
        print_info "Installing build tools..."
        sudo pacman -S --noconfirm base-devel
        print_success "Build tools installed"
        
        # Install SFML
        print_info "Installing SFML..."
        sudo pacman -S --noconfirm sfml
        print_success "SFML installed"
        ;;
        
    opensuse*|suse)
        print_info "Installing dependencies for openSUSE..."
        
        # Install CMake
        if ! command_exists cmake; then
            print_info "Installing CMake..."
            sudo zypper install -y cmake
            print_success "CMake installed"
        else
            print_success "CMake already installed ($(cmake --version | head -n1))"
        fi
        
        # Install build tools
        print_info "Installing build tools..."
        sudo zypper install -y gcc-c++ make
        print_success "Build tools installed"
        
        # Install SFML
        print_info "Installing SFML..."
        sudo zypper install -y libsfml2 sfml2-devel
        print_success "SFML installed"
        ;;
        
    *)
        print_error "Unsupported distribution: $DISTRO"
        echo ""
        print_info "Please install the following packages manually:"
        echo "  - CMake (3.11 or higher)"
        echo "  - GCC/G++ with C++17 support"
        echo "  - SFML 2.5 or higher"
        echo ""
        print_info "Or use FetchContent by building with:"
        echo "  cmake -S . -B build -DUSE_SYSTEM_SFML=OFF"
        exit 1
        ;;
esac

echo ""
echo "=========================================="
print_success "All dependencies installed successfully!"
echo "=========================================="
echo ""
print_info "You can now build the project with:"
echo "  cmake -S . -B build"
echo "  cmake --build build"
echo ""
print_info "Or use the automatic SFML download:"
echo "  cmake -S . -B build -DUSE_SYSTEM_SFML=OFF"
echo ""

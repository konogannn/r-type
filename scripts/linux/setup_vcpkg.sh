#!/bin/bash
set -e

echo "=========================================="
echo "  vcpkg Installation Script (Linux)       "
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

VCPKG_DIR="${HOME}/.vcpkg"

if [ -d "$VCPKG_DIR" ]; then
    print_info "vcpkg already installed at: $VCPKG_DIR"
    read -p "Update it? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        cd "$VCPKG_DIR" && git pull && ./bootstrap-vcpkg.sh
        print_ok "Updated"
    fi
else
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
    else
        print_err "Cannot detect distribution"
        exit 1
    fi

    print_info "Installing system dependencies..."
    case "$DISTRO" in
        ubuntu|debian|linuxmint|pop)
            sudo apt update && sudo apt install -y curl zip unzip tar git build-essential pkg-config
            ;;
        fedora|rhel|centos|rocky|almalinux)
            sudo dnf install -y curl zip unzip tar git gcc-c++ make
            ;;
        arch|manjaro)
            sudo pacman -Sy --noconfirm curl zip unzip tar git base-devel
            ;;
        opensuse*|suse)
            sudo zypper install -y curl zip unzip tar git gcc-c++ make
            ;;
        *)
            print_err "Unsupported: $DISTRO"
            exit 1
            ;;
    esac
    print_ok "Dependencies installed"

    print_info "Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg.git "$VCPKG_DIR"

    print_info "Bootstrapping vcpkg..."
    cd "$VCPKG_DIR" && ./bootstrap-vcpkg.sh
    print_ok "vcpkg installed"
fi

# Detect user's default shell
case "$(basename "$SHELL")" in
    zsh) SHELL_RC="$HOME/.zshrc" ;;
    bash) SHELL_RC="$HOME/.bashrc" ;;
    fish) SHELL_RC="$HOME/.config/fish/config.fish" ;;
    *) SHELL_RC="$HOME/.profile" ;;
esac

if ! grep -q "VCPKG_ROOT" "$SHELL_RC"; then
    echo "" >> "$SHELL_RC"
    echo "export VCPKG_ROOT=\"$VCPKG_DIR\"" >> "$SHELL_RC"
    echo "export PATH=\"\$VCPKG_ROOT:\$PATH\"" >> "$SHELL_RC"
    print_ok "Added to $SHELL_RC"
fi

export VCPKG_ROOT="$VCPKG_DIR"
export PATH="$VCPKG_ROOT:$PATH"

echo ""
print_ok "Setup complete!"
print_info "Restart terminal or run: source $SHELL_RC"
print_info "Then build: cmake -S . -B build && cmake --build build"
echo ""

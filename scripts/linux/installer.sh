#!/bin/bash

set -e

MAGENTABOLD='\033[1;35m'
NC='\033[0m'
print_header() {
    echo -e "${MAGENTABOLD}==========================================${NC}"
    printf "${MAGENTABOLD}%*s%s${NC}\n" $(((44 - ${#1}) / 2)) "" "$1"
    echo -e "${MAGENTABOLD}==========================================${NC}"
}
SCRIPTS_DIR="$(dirname "$0")"

print_header "Starting the Setup and Build process"

# 1. Installation of system dependencies
"$SCRIPTS_DIR/install_dependencies.sh"

# 2. Initialization and preparation of Vcpkg
"$SCRIPTS_DIR/setup_vcpkg.sh"

# 3. Configuration of CMake and building the project
"$SCRIPTS_DIR/build_project.sh"

print_header "PROCESS COMPLETED SUCCESSFULLY"

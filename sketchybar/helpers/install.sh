#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Variables
TEMP_DIR=$(mktemp -d)
FONT_URL="https://github.com/kvndrsslr/sketchybar-app-font/releases/download/v2.0.5/sketchybar-app-font.ttf"
FONT_DEST="$HOME/Library/Fonts/sketchybar-app-font.ttf"
SBARLUA_REPO="https://github.com/FelixKratz/SbarLua.git"
SBARLUA_DIR="/tmp/SbarLua"

# Logging Functions
log_info() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

log_success() {
    echo -e "\033[1;32m[SUCCESS]\033[0m $1"
}

log_error() {
    echo -e "\033[1;31m[ERROR]\033[0m $1" >&2
}

# Cleanup Function
cleanup() {
    rm -rf "$TEMP_DIR"
    rm -rf "$SBARLUA_DIR"
}
trap cleanup EXIT

# Check if Homebrew is installed
check_homebrew() {
    if ! command -v brew &>/dev/null; then
        log_error "Homebrew is not installed. Please install Homebrew and rerun the script."
        exit 1
    fi
    log_info "Homebrew is installed."
}

# Update Homebrew
update_brew() {
    log_info "Updating Homebrew..."
    brew update
    log_success "Homebrew updated."
}

# Install Homebrew Packages
install_packages() {
    local packages=(
        lua
        switchaudio-osx
        nowplaying-cli
    )
    
    log_info "Installing Homebrew packages..."
    for pkg in "${packages[@]}"; do
        if brew list "$pkg" &>/dev/null; then
            log_info "Package '$pkg' is already installed."
        else
            brew install "$pkg"
            log_success "Installed package '$pkg'."
        fi
    done
}

# Tap Additional Repositories and Install Packages
tap_and_install() {
    local tap="FelixKratz/formulae"
    local tap_exists=$(brew tap | grep -c "^$tap\$")

    if [ "$tap_exists" -eq 0 ]; then
        log_info "Tapping '$tap' repository..."
        brew tap "$tap"
        log_success "Tapped '$tap'."
    else
        log_info "Tap '$tap' already exists."
    fi

    log_info "Installing 'sketchybar'..."
    if brew list sketchybar &>/dev/null; then
        log_info "'sketchybar' is already installed."
    else
        brew install sketchybar
        log_success "Installed 'sketchybar'."
    fi
}

# Install Fonts via Homebrew Cask
install_fonts_cask() {
    local fonts=(
        sf-symbols
        homebrew/cask-fonts/font-sf-mono
        homebrew/cask-fonts/font-sf-pro
    )
    
    log_info "Installing fonts via Homebrew Cask..."
    for font in "${fonts[@]}"; do
        if brew list --cask "$font" &>/dev/null; then
            log_info "Font '$font' is already installed."
        else
            brew install --cask "$font"
            log_success "Installed font '$font'."
        fi
    done
}

# Download and Install Sketchybar App Font
install_sketchybar_font() {
    if [ -f "$FONT_DEST" ]; then
        log_info "Sketchybar app font is already installed."
        return
    fi

    log_info "Downloading Sketchybar app font..."
    curl -L "$FONT_URL" -o "$TEMP_DIR/sketchybar-app-font.ttf"
    log_success "Downloaded Sketchybar app font."

    log_info "Installing Sketchybar app font..."
    mv "$TEMP_DIR/sketchybar-app-font.ttf" "$FONT_DEST"
    log_success "Installed Sketchybar app font to '$FONT_DEST'."
}

# Clone, Install, and Cleanup SbarLua
install_sbarlua() {
    if [ -d "$SBARLUA_DIR" ]; then
        log_info "SbarLua directory already exists. Removing..."
        rm -rf "$SBARLUA_DIR"
    fi

    log_info "Cloning SbarLua repository..."
    git clone "$SBARLUA_REPO" "$SBARLUA_DIR"
    log_success "Cloned SbarLua."

    log_info "Installing SbarLua..."
    cd "$SBARLUA_DIR"
    make install
    log_success "Installed SbarLua."
}

# Main Execution Flow
main() {
    check_homebrew
    update_brew
    install_packages
    tap_and_install
    install_fonts_cask
    install_sketchybar_font
    install_sbarlua
    log_success "All installations completed successfully."
}

main "$@"
#!/usr/bin/env zsh

set -euo pipefail
IFS=$'\n\t'

# ----------------------------
# sketchybar.sh - Setup SketchyBar and Dependencies
# Project: macOS-config
# Author: Imnoobincoding
# ----------------------------

# Function to print headers
print_header() {
    echo "\n===== $1 =====\n"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install a Homebrew package if not already installed
install_brew_package() {
    local package=$1
    if ! brew list "$package" >/dev/null 2>&1; then
        brew install "$package"
        echo "$package installed successfully."
    else
        echo "$package is already installed."
    fi
}

# Function to install a Homebrew cask if not already installed
install_brew_cask() {
    local cask=$1
    if ! brew list --cask "$cask" >/dev/null 2>&1; then
        brew install --cask "$cask"
        echo "$cask installed successfully."
    else
        echo "$cask is already installed."
    fi
}

# Function to clone and install SbarLua
install_sbarlua() {
    local repo="https://github.com/FelixKratz/SbarLua.git"
    local temp_dir="/tmp/SbarLua"
    
    if [ ! -d "$temp_dir" ]; then
        git clone "$repo" "$temp_dir"
        cd "$temp_dir"
        make install
        cd ~
        rm -rf "$temp_dir"
        echo "SbarLua installed successfully."
    else
        echo "SbarLua is already cloned in $temp_dir."
    fi
}

# Check for Homebrew, Git, and Curl
print_header "Checking Dependencies"
for cmd in brew git curl; do
    if ! command_exists "$cmd"; then
        echo "Error: $cmd is not installed. Please install it and rerun the script."
        exit 1
    fi
done

# Update Homebrew
print_header "Updating Homebrew"
brew update

# Install SketchyBar and Dependencies
print_header "Installing SketchyBar and Dependencies"
brew tap FelixKratz/formulae || echo "FelixKratz/formulae tap already added."
install_brew_cask "sketchybar"
install_brew_package "jq"
install_brew_package "lua"
install_brew_package "switchaudio-osx"
install_brew_package "nowplaying-cli"

# Install SbarLua
print_header "Installing SbarLua"
install_sbarlua

# Install Fonts
print_header "Installing Fonts"
brew tap homebrew/cask-fonts || echo "homebrew/cask-fonts tap already added."
install_brew_cask "sf-symbols"
install_brew_cask "font-sf-mono"
install_brew_cask "font-sf-pro"

# Download and install sketchybar-app-font.ttf
FONT_URL="https://github.com/kvndrsslr/sketchybar-app-font/releases/download/v2.0.28/sketchybar-app-font.ttf"
FONT_PATH="$HOME/Library/Fonts/sketchybar-app-font.ttf"

if [ ! -f "$FONT_PATH" ]; then
    curl -fsSL "$FONT_URL" -o "$FONT_PATH"
    echo "sketchybar-app-font.ttf downloaded and installed."
else
    echo "sketchybar-app-font.ttf is already installed."
fi

# Move config files with backup
print_header "Configuring SketchyBar"
CONFIG_SOURCE="$HOME/Downloads/macOS-config/sketchybar"
CONFIG_DEST="$HOME/.config/sketchybar"

if [ -d "$CONFIG_DEST" ]; then
    cp -r "$CONFIG_DEST" "${CONFIG_DEST}.backup.$(date +%F)"
    echo "Backup of existing SketchyBar configuration created at ${CONFIG_DEST}.backup.$(date +%F)"
fi

if [ -d "$CONFIG_SOURCE" ]; then
    mv "$CONFIG_SOURCE" "$CONFIG_DEST"
    echo "SketchyBar configuration files moved to $CONFIG_DEST."
else
    echo "Configuration source $CONFIG_SOURCE does not exist. Skipping move."
fi

# Start and Restart SketchyBar
print_header "Starting SketchyBar Services"
brew services start sketchybar || echo "SketchyBar service already started."
brew services restart sketchybar || echo "SketchyBar service restarted."

# Final Instructions
print_header "Setup Complete"
echo "SketchyBar and all dependencies have been installed and configured."
echo "If you encounter any issues, please check the configuration files in $CONFIG_DEST or consult the SketchyBar documentation."

# ----------------------------
# End of sketchybar.sh
# ----------------------------

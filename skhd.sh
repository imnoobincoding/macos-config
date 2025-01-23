#!/usr/bin/env zsh

set -euo pipefail
IFS=$'\n\t'

# ----------------------------
# skhd.sh - Setup skhd Hotkey Daemon
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

# Function to install a Homebrew tap if not already tapped
tap_brew_formula() {
    local tap=$1
    if ! brew tap | grep -q "^$tap\$"; then
        brew tap "$tap"
        echo "Tapped $tap successfully."
    else
        echo "Tap $tap is already added."
    fi
}

# Function to move configuration files with backup
move_config_files() {
    local source_dir=$1
    local dest_dir=$2

    if [ -d "$dest_dir" ]; then
        cp -r "$dest_dir" "${dest_dir}.backup.$(date +%F)"
        echo "Backup of existing configuration at $dest_dir.backup.$(date +%F)"
    fi

    mv "$source_dir" "$dest_dir"
    echo "Configuration files moved to $dest_dir."
}

# Check for Homebrew
print_header "Checking Homebrew Installation"
if ! command_exists brew; then
    echo "Homebrew is not installed. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    echo "Homebrew installed successfully."
else
    echo "Homebrew is already installed."
fi

# Update Homebrew
print_header "Updating Homebrew"
brew update

# Tap the required Homebrew formula
print_header "Tapping koekeishiya/formulae"
tap_brew_formula "koekeishiya/formulae"

# Install skhd
print_header "Installing skhd"
install_brew_package "skhd"

# Install dependencies if any (none in this script, but placeholder for future)

# Move configuration files with backup
print_header "Configuring skhd"
CONFIG_SOURCE="$HOME/Downloads/macOS-config/skhd"
CONFIG_DEST="$HOME/.config/skhd"

if [ -d "$CONFIG_SOURCE" ]; then
    move_config_files "$CONFIG_SOURCE" "$CONFIG_DEST"
else
    echo "Configuration source $CONFIG_SOURCE does not exist. Skipping move."
fi

# Start and Restart skhd using Homebrew services for better management
print_header "Managing skhd Service"
if brew services list | grep -q "skhd.*started"; then
    brew services restart skhd
    echo "skhd service restarted successfully."
else
    brew services start skhd
    echo "skhd service started successfully."
fi

# Final Instructions
print_header "Setup Complete"
echo "skhd has been installed and configured successfully."
echo "If you encounter any issues, please check the configuration files in $CONFIG_DEST or consult the skhd documentation."

# ----------------------------
# End of skhd.sh
# ----------------------------

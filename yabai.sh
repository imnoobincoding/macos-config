#!/usr/bin/env zsh

set -euo pipefail
IFS=$'\n\t'

# ----------------------------
# yabai.sh - Setup yabai Window Manager
# Project: macOS-config
# Author: Your Name
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

    if [ -f "$dest_dir/yabairc" ]; then
        cp "$dest_dir/yabairc" "${dest_dir}/yabairc.backup.$(date +%F)"
        echo "Backup of existing yabai configuration created at ${dest_dir}/yabairc.backup.$(date +%F)"
    fi

    if [ -d "$source_dir" ] || [ -f "$source_dir/yabairc" ]; then
        mkdir -p "$dest_dir"
        mv "$source_dir/yabairc" "$dest_dir/"
        echo "yabai configuration files moved to $dest_dir."
    else
        echo "Configuration source $source_dir/yabairc does not exist. Skipping move."
    fi
}

# Function to start or restart yabai service
manage_yabai_service() {
    local action=$1
    case $action in
        start)
            brew services start yabai || echo "Failed to start yabai service."
            ;;
        restart)
            brew services restart yabai || echo "Failed to restart yabai service."
            ;;
        *)
            echo "Invalid action: $action"
            ;;
    esac
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

# Install yabai
print_header "Installing yabai"
install_brew_package "yabai"

# Install scripting addition for yabai (required for window management)
print_header "Installing yabai Scripting Addition"
brew install koekeishiya/formulae/yabai --with-scripting-addition

# Move configuration files with backup
print_header "Configuring yabai"
CONFIG_SOURCE="$HOME/Downloads/macOS-config/yabai"
CONFIG_DEST="$HOME/.config/yabai"

move_config_files "$CONFIG_SOURCE" "$CONFIG_DEST"

# Set appropriate permissions for yabairc
chmod 600 "$CONFIG_DEST/yabairc" || echo "Failed to set permissions for yabairc."

# Start and Restart yabai
print_header "Starting yabai Service"
manage_yabai_service "start"

print_header "Restarting yabai Service"
manage_yabai_service "restart"

# Post-Installation Instructions
print_header "Post-Installation Steps"
echo "yabai has been installed and configured."
echo "Ensure that the following permissions are granted for yabai to function correctly:"
echo "1. Accessibility: System Preferences -> Security & Privacy -> Privacy -> Accessibility -> Add yabai"
echo "2. Scripting Additions: System Preferences -> Security & Privacy -> General -> Allow apps downloaded from: App Store and identified developers"

echo "Please reboot your system to apply all changes."

# ----------------------------
# End of yabai.sh
# ----------------------------

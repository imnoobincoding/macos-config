#!/usr/bin/env zsh

set -euo pipefail
IFS=$'\n\t'

# ----------------------------
# raycast.sh - Setup Raycast Application
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

# Install Raycast
print_header "Installing Raycast"
if ! brew list --cask raycast >/dev/null 2>&1; then
    brew install --cask raycast
    echo "Raycast installed successfully."
else
    echo "Raycast is already installed."
fi

# Open Raycast
print_header "Launching Raycast"
open -a Raycast

# Configure Raycast Settings
print_header "Configuring Raycast Settings"

# Note: Automating application-specific settings can be challenging.
# If Raycast supports command-line configuration or APIs, integrate them here.
# Otherwise, provide instructions to the user.

echo "Please configure Raycast settings manually as follows:"
echo "1. Click 'Start Setup' -> Continue."
echo "2. Set a new hotkey:"
echo "   - Record New Hotkey -> Assign Cmd + Space."
echo "3. Click 'Continue' to finalize."

# Optional: Automate settings using AppleScript if Raycast supports it
# Uncomment and modify the script below if applicable

# print_header "Automating Raycast Settings with AppleScript"
# osascript <<EOF
# tell application "Raycast"
#     -- Example: Set Hotkey to Cmd + Space
#     set hotkey to {command down, space}
#     -- Add more configuration commands as needed
# end tell
# EOF

# Final Instructions
print_header "Setup Complete"
echo "Raycast has been installed and launched."
echo "Please complete the manual configuration steps to finalize the setup."
echo "Remember to assign a hotkey that doesn't conflict with other system shortcuts."

# ----------------------------
# End of raycast.sh
# ----------------------------

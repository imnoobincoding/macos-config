#!/usr/bin/env zsh

set -euo pipefail
IFS=$'\n\t'

# ----------------------------
# loop.sh - Setup Loop Application
# Project: macOS-config
# Author: Imnoobincoding
# ----------------------------

# Function to print headers
print_header() {
    echo "\n===== $1 =====\n"
}

# Check for Homebrew
print_header "Checking Homebrew Installation"
if ! command -v brew >/dev/null 2>&1; then
    echo "Homebrew is not installed. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
else
    echo "Homebrew is already installed."
fi

# Update Homebrew
print_header "Updating Homebrew"
brew update

# Install Loop
print_header "Installing Loop"
if ! brew list --cask loop >/dev/null 2>&1; then
    brew install --cask loop
else
    echo "Loop is already installed."
fi

# Open Loop
print_header "Launching Loop"
open -a Loop

# Configure Loop Settings
print_header "Configuring Loop Settings"

# Note: Automating application-specific settings can be challenging.
# If Loop supports command-line configuration or APIs, integrate them here.
# Otherwise, provide instructions to the user.

echo "Please configure Loop settings manually as follows:"
echo "1. Accent Colour -> Wallpaper -> Gradient -> On"
echo "2. Behaviour -> Launch at login -> On"
echo "3. Behaviour -> Animation speed -> Smooth"
echo "4. Behaviour -> Include padding -> On -> Configure padding -> 7px"
echo "5. Behaviour -> Use screen with cursor -> Off"
echo "6. Keybindings -> Centre -> Remove"
echo "7. Keybindings -> Add -> General -> MacOS Centre -> Fn + Enter"

# Optional: Automate settings using AppleScript if Loop supports it
# Uncomment and modify the script below if applicable

# print_header "Automating Loop Settings with AppleScript"
# osascript <<EOF
# tell application "Loop"
#     -- Example: Set Accent Colour to Gradient
#     set accentColor to "Gradient"
#     -- Add more configuration commands as needed
# end tell
# EOF

# Final Instructions
print_header "Setup Complete"
echo "Loop has been installed and launched."
echo "Please ensure that all settings are configured as per your preferences."

# ----------------------------
# End of loop.sh
# ----------------------------

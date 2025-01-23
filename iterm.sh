#!/usr/bin/env zsh

set -euo pipefail
IFS=$'\n\t'

# ----------------------------
# iterm.sh - Setup iTerm2 and Zsh
# Project: macOS-config
# Author: Imnoobincoding
# ----------------------------

# Function to print headers
print_header() {
    echo "\n===== $1 =====\n"
}

# Check for Homebrew
if ! command -v brew >/dev/null 2>&1; then
    echo "Homebrew is not installed. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Update Homebrew
brew update

# Install iTerm2
print_header "Installing iTerm2"
if ! brew list --cask iterm2 >/dev/null 2>&1; then
    brew install --cask iterm2
else
    echo "iTerm2 is already installed."
fi

# Install Oh My Zsh if not installed
print_header "Installing Oh My Zsh"
if [ ! -d "$HOME/.oh-my-zsh" ]; then
    sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
else
    echo "Oh My Zsh is already installed."
fi

# Install Zsh Syntax Highlighting Plugin
print_header "Installing Zsh Syntax Highlighting Plugin"
ZSH_SYNTAX_HIGHLIGHTING_DIR="${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting"
if [ ! -d "$ZSH_SYNTAX_HIGHLIGHTING_DIR" ]; then
    git clone https://github.com/zsh-users/zsh-syntax-highlighting.git "$ZSH_SYNTAX_HIGHLIGHTING_DIR"
else
    echo "Zsh Syntax Highlighting is already installed."
fi

# Install Powerlevel10k Theme
print_header "Installing Powerlevel10k Theme"
if [ ! -d "${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k" ]; then
    git clone --depth=1 https://github.com/romkatv/powerlevel10k.git "${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k"
else
    echo "Powerlevel10k is already installed."
fi

# Install Color Scheme
print_header "Installing Rose Pine Color Scheme"
COLOR_SCHEME_URL="https://raw.githubusercontent.com/rose-pine/iterm/main/rose-pine.itermcolors"
COLOR_SCHEME_PATH="$HOME/Downloads/rose-pine.itermcolors"
if [ ! -f "$COLOR_SCHEME_PATH" ]; then
    curl -fsSL -o "$COLOR_SCHEME_PATH" "$COLOR_SCHEME_URL"
    # Optionally, add code to import the color scheme into iTerm2
else
    echo "Rose Pine color scheme already downloaded."
fi

# Install MesloLGS NF Font
print_header "Installing MesloLGS NF Font"
FONT_URL="https://github.com/romkatv/dotfiles-public/raw/master/.local/share/fonts/NerdFonts/MesloLGS%20NF%20Regular.ttf"
FONT_PATH="$HOME/Library/Fonts/MesloLGS_NF_Regular.ttf"
if [ ! -f "$FONT_PATH" ]; then
    curl -fsSL -o "$FONT_PATH" "$FONT_URL"
    echo "Font installed. Please restart iTerm2 to apply."
else
    echo "MesloLGS NF Font is already installed."
fi

# Backup existing .zshrc
print_header "Backing up existing .zshrc"
if [ -f "$HOME/.zshrc" ]; then
    cp "$HOME/.zshrc" "$HOME/.zshrc.backup.$(date +%F)"
    echo "Backup created at ~/.zshrc.backup.$(date +%F)"
fi

# Rewrite zshrc
print_header "Configuring .zshrc"
cat << EOF > ~/.zshrc
export ZSH="\$HOME/.oh-my-zsh"

export PATH="\$HOME/bin:\$HOME/.local/bin:/usr/local/bin:\$HOME/.rvm/bin:/opt/homebrew/bin:\$PATH"

ZSH_THEME="powerlevel10k/powerlevel10k"

plugins=(git zsh-syntax-highlighting)

source \$ZSH/oh-my-zsh.sh

# Enable Powerlevel10k instant prompt. Should stay close to the top of ~/.zshrc.
[[ ! -f "\$HOME/.p10k.zsh" ]] || source "\$HOME/.p10k.zsh"
EOF

# Install Powerlevel10k Configuration
print_header "Configuring Powerlevel10k"
if [ ! -f "$HOME/.p10k.zsh" ]; then
    git clone --depth=1 https://github.com/romkatv/powerlevel10k.git "${ZSH_CUSTOM:-$HOME/.oh-my-zsh/custom}/themes/powerlevel10k"
    # Automatically generate a basic config or prompt user to run p10k configure
    echo "Please run 'p10k configure' to set up Powerlevel10k."
else
    echo "Powerlevel10k is already configured."
fi

# Final Instructions
print_header "Setup Complete"
echo "Please restart your terminal or run 'source ~/.zshrc' to apply the changes."
echo "Remember to run 'p10k configure' to customize your Powerlevel10k prompt."

# ----------------------------
# End of iterm.sh
# ----------------------------

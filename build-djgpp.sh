#!/bin/zsh
# Build script for DJGPP project
# Runs setenv before building
# Usage: ./build-djgpp.sh [clean|rebuild]
# Requires: DJGPP_ROOT and DOS_INSTALL_DIR environment variables

set -e

# Check required environment variables
if [[ -z "$DJGPP_ROOT" ]]; then
    echo "Error: DJGPP_ROOT environment variable not set"
    exit 1
fi

if [[ -z "$DOS_INSTALL_DIR" ]]; then
    echo "Error: DOS_INSTALL_DIR environment variable not set"
    exit 1
fi

# Function to clean build artifacts
clean() {
    echo "Cleaning build artifacts..."
    rm -f obj/*.o
    rm -f bin/dmain.exe
    echo "Clean complete."
}

# Handle clean command
if [[ "$1" == "clean" ]]; then
    clean
    exit 0
fi

# Handle rebuild command
if [[ "$1" == "rebuild" ]]; then
    clean
fi

# Build
if [[ -z "$DJDIR" ]]; then
    echo "Setting up DJGPP environment..."
    source "$DJGPP_ROOT/setenv"
else
    echo "Re-using existing DJGPP environment..."
fi

if [[ -z "$DJDIR" ]]; then
    echo "Error: DJGPP environment not set up correctly"
    exit 1
fi

# Create directories if they don't exist
mkdir -p obj
mkdir -p bin

echo "Compiling source files..."
COMPILE_ERROR=0
for file in src/*.c; do
    if [[ -f "$file" ]]; then
        echo "  Compiling $file..."
        basename="${file:t:r}"
        if ! gcc -O3 -Wall -c "$file" -o "obj/${basename}.o"; then
        # if ! gcc -g -Wall -Wextra -c "$file" -o "obj/${basename}.o"; then
            COMPILE_ERROR=1
        fi
    fi
done

if [[ $COMPILE_ERROR -eq 1 ]]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Linking..."
if ! gcc -o bin/dmain.exe obj/*.o -lm; then
    echo "Linking failed!"
    exit 1
fi

echo "Build successful! Output: bin/dmain.exe"

# Install to target directory
mkdir -p "$DOS_INSTALL_DIR"
echo "Installing bin/dmain.exe to $DOS_INSTALL_DIR..."
cp bin/dmain.exe "$DOS_INSTALL_DIR/"
echo "Copying assets to $DOS_INSTALL_DIR..."
cp -r assets/* "$DOS_INSTALL_DIR/"
echo "Installation complete."

#!/bin/bash
# Version increment script for BG Reader project
# Usage: ./increment_version.sh [description]

CURRENT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VERSION_FILE="$CURRENT_DIR/version.h"
CMAKE_FILE="$CURRENT_DIR/CMakeLists.txt"

# Read current version
CURRENT_VERSION=$(grep '#define PROJECT_VERSION_STRING' "$VERSION_FILE" | cut -d'"' -f2)
MAJOR=$(echo "$CURRENT_VERSION" | cut -d'.' -f1)
MINOR=$(echo "$CURRENT_VERSION" | cut -d'.' -f2)

# Increment minor version by 1 (0.01)
NEW_MINOR=$((MINOR + 1))
NEW_VERSION="$MAJOR.$(printf "%02d" $NEW_MINOR)"

echo "Current version: $CURRENT_VERSION"
echo "New version: $NEW_VERSION"

# Update version.h
sed -i "s/#define PROJECT_VERSION_MINOR [0-9]*/#define PROJECT_VERSION_MINOR $NEW_MINOR/" "$VERSION_FILE"
sed -i "s/#define PROJECT_VERSION_STRING \".*\"/#define PROJECT_VERSION_STRING \"$NEW_VERSION\"/" "$VERSION_FILE"

# Update CMakeLists.txt
sed -i "s/pico_set_program_version(Bg_Reader \".*\")/pico_set_program_version(Bg_Reader \"$NEW_VERSION\")/" "$CMAKE_FILE"

# Add version comment if description provided
if [ ! -z "$1" ]; then
    TIMESTAMP=$(date "+%Y-%m-%d")
    COMMENT="// $NEW_VERSION - $TIMESTAMP - $1"
    sed -i "/\/\/ Version History (for reference)/a $COMMENT" "$VERSION_FILE"
fi

echo "Version updated to $NEW_VERSION"
echo "Don't forget to commit with: git add . && git commit -m \"v$NEW_VERSION: [description]\""

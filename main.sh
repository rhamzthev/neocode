#!/bin/bash
# Build and run script for C text editor project
# Usage: ./main.sh [file_to_open]

# Default file to open if not provided
FILE_TO_OPEN=${1:-"./src/main.c"}

echo "🧹 Cleaning previous build..."
if make clean; then
    echo "✅ Clean successful"
else
    echo "❌ Clean failed with exit code $?"
    exit 1
fi

echo "🔨 Building project..."
if make; then
    echo "✅ Build successful"
else
    echo "❌ Build failed with exit code $?"
    exit 2
fi

echo "🚀 Running editor with file: $FILE_TO_OPEN"
if make run ARGS="$FILE_TO_OPEN"; then
    echo "✅ Program exited successfully"
else
    EXIT_CODE=$?
    echo "⚠️ Program exited with code: $EXIT_CODE"
    exit $EXIT_CODE
fi

echo "✨ All operations completed successfully"
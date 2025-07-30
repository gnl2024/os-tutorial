#!/bin/bash

# WISP-BB Repository Setup Script
# This script helps create and push to the wisp-bb repository

echo "=== WISP-BB Repository Setup ==="
echo ""

# Check if we're in the right directory
if [ ! -d "21-shell" ]; then
    echo "❌ Error: Please run this script from the wisp-bb root directory"
    exit 1
fi

echo "✅ Current directory: $(pwd)"
echo "✅ Found 21-shell directory"
echo ""

# Check git status
echo "=== Git Status ==="
git status --porcelain
echo ""

# Show current remotes
echo "=== Current Remotes ==="
git remote -v
echo ""

echo "=== Repository Creation Instructions ==="
echo ""
echo "To create the wisp-bb repository, please:"
echo ""
echo "1. Go to https://github.com/gnl2024"
echo "2. Click 'New' to create a new repository"
echo "3. Set repository name: wisp-bb"
echo "4. Set description: 'WISP-BB Operating System - Complete 32-bit x86 OS with C migration'"
echo "5. Set visibility: Private"
echo "6. DO NOT initialize with README, .gitignore, or license (we'll push existing code)"
echo "7. Click 'Create repository'"
echo ""
echo "After creating the repository, run:"
echo "  git push wisp-bb master"
echo ""
echo "Or if you want to set wisp-bb as the new origin:"
echo "  git remote remove origin"
echo "  git remote rename wisp-bb origin"
echo "  git push origin master"
echo ""

# Check if wisp-bb remote exists
if git remote get-url wisp-bb >/dev/null 2>&1; then
    echo "✅ wisp-bb remote is configured"
    echo "Repository URL: $(git remote get-url wisp-bb)"
    echo ""
    echo "Ready to push when repository is created!"
else
    echo "❌ wisp-bb remote not found"
    echo "Adding wisp-bb remote..."
    git remote add wisp-bb https://github.com/gnl2024/wisp-bb.git
    echo "✅ wisp-bb remote added"
fi

echo ""
echo "=== Project Summary ==="
echo "Repository: https://github.com/gnl2024/wisp-bb.git"
echo "Status: Ready to push"
echo "Features: Complete OS with C migration"
echo "Documentation: Comprehensive documentation suite"
echo "" 
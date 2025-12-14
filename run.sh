#!/bin/bash

# GitGrade Setup and Launch Script
# This script helps you set up and run GitGrade easily

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Banner
echo -e "${BLUE}"
cat << "EOF"
   _____ _ _   _____               _      
  / ____(_) | / ____|             | |     
 | |  __ _| || |  __ _ __ __ _  __| | ___ 
 | | |_ | | || | |_ | '__/ _` |/ _` |/ _ \
 | |__| | | || |__| | | | (_| | (_| |  __/
  \_____|_|_| \_____|_|  \__,_|\__,_|\___|
                                           
EOF
echo -e "${NC}"
echo -e "${GREEN}🚀 GitGrade Setup & Launch Script${NC}"
echo ""

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to print status
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

# Check prerequisites
echo -e "${YELLOW}Checking prerequisites...${NC}"
echo ""

# Check for g++
if command_exists g++; then
    print_success "g++ compiler found"
    g++ --version | head -n 1
else
    print_error "g++ not found. Please install g++:"
    echo "  Ubuntu/Debian: sudo apt-get install g++"
    echo "  macOS: xcode-select --install"
    exit 1
fi

# Check for git
if command_exists git; then
    print_success "git found"
else
    print_error "git not found. Please install git."
    exit 1
fi

# Check for curl library
if [ -f "/usr/include/curl/curl.h" ] || [ -f "/usr/local/include/curl/curl.h" ]; then
    print_success "libcurl found"
else
    print_warning "libcurl not found. Installing..."
    if command_exists apt-get; then
        sudo apt-get update && sudo apt-get install -y libcurl4-openssl-dev
    elif command_exists brew; then
        brew install curl
    else
        print_error "Cannot install libcurl automatically. Please install manually."
        exit 1
    fi
fi

# Check for Python 3 (for frontend server)
if command_exists python3; then
    print_success "Python 3 found"
else
    print_warning "Python 3 not found. You'll need to open HTML manually."
fi

echo ""

# Check if Gemini API key is set
print_status "Checking for Gemini API key..."
if grep -q "YOUR_GEMINI_API_KEY_HERE" gitgrade_gemini.cpp 2>/dev/null; then
    print_warning "Gemini API key not configured!"
    echo ""
    echo "  To enable AI features:"
    echo "  1. Get API key from: https://makersuite.google.com/app/apikey"
    echo "  2. Open gitgrade_gemini.cpp"
    echo "  3. Replace 'YOUR_GEMINI_API_KEY_HERE' with your key"
    echo ""
    read -p "  Continue without AI features? (y/n) " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
else
    print_success "Gemini API key configured"
fi

echo ""

# Check if vcpkg is available
VCPKG_PATH=""
if [ -d "$HOME/vcpkg" ]; then
    VCPKG_PATH="$HOME/vcpkg/installed/x64-linux"
    print_success "vcpkg found at $HOME/vcpkg"
elif [ -d "/usr/local/vcpkg" ]; then
    VCPKG_PATH="/usr/local/vcpkg/installed/x64-linux"
    print_success "vcpkg found at /usr/local/vcpkg"
else
    print_warning "vcpkg not found - will try system libraries"
fi

echo ""

# Compile the application
print_status "Compiling GitGrade..."
echo ""

if [ -n "$VCPKG_PATH" ]; then
    # Compile with vcpkg
    g++ -std=c++17 gitgrade_gemini.cpp -o gitgrade \
        -I"$VCPKG_PATH/include" \
        -L"$VCPKG_PATH/lib" \
        -lcurl \
        -lpthread \
        -lstdc++fs \
        -O2
else
    # Compile with system libraries
    g++ -std=c++17 gitgrade_gemini.cpp -o gitgrade \
        -lcurl \
        -lpthread \
        -lstdc++fs \
        -O2
fi

if [ $? -eq 0 ]; then
    print_success "Compilation successful!"
else
    print_error "Compilation failed!"
    echo ""
    echo "Troubleshooting:"
    echo "  1. Make sure all dependencies are installed"
    echo "  2. Check INTEGRATION_GUIDE.md for detailed instructions"
    echo "  3. Try manual compilation from the guide"
    exit 1
fi

echo ""

# Create a cleanup trap
cleanup() {
    echo ""
    print_status "Shutting down servers..."
    if [ -n "$BACKEND_PID" ]; then
        kill $BACKEND_PID 2>/dev/null
    fi
    if [ -n "$FRONTEND_PID" ]; then
        kill $FRONTEND_PID 2>/dev/null
    fi
    # Clean up any temporary repositories
    rm -rf temp_repo_* 2>/dev/null
    print_success "Cleanup complete"
    exit 0
}

trap cleanup INT TERM

# Start the backend server
print_status "Starting backend server..."
./gitgrade &
BACKEND_PID=$!

# Wait for backend to start
sleep 3

# Check if backend is running
if ! kill -0 $BACKEND_PID 2>/dev/null; then
    print_error "Backend failed to start!"
    exit 1
fi

# Test backend health
if command_exists curl; then
    sleep 1
    HEALTH_CHECK=$(curl -s http://localhost:18080/health 2>/dev/null)
    if [ $? -eq 0 ]; then
        print_success "Backend is healthy!"
    else
        print_warning "Backend health check failed (may still be starting...)"
    fi
fi

echo ""

# Start frontend server if Python is available
if command_exists python3; then
    print_status "Starting frontend server..."
    python3 -m http.server 8000 >/dev/null 2>&1 &
    FRONTEND_PID=$!
    sleep 2
    
    print_success "Frontend server started!"
    echo ""
    echo -e "${GREEN}==============================================
    echo -e "  🎉 GitGrade is now running!"
    echo -e "==============================================${NC}"
    echo ""
    echo -e "${BLUE}Backend API:${NC}  http://localhost:18080"
    echo -e "${BLUE}Frontend UI:${NC}  http://localhost:8000/gitgrade-pro-ui.html"
    echo ""
    echo -e "${YELLOW}Opening browser...${NC}"
    sleep 2
    
    # Try to open browser
    if command_exists xdg-open; then
        xdg-open "http://localhost:8000/gitgrade-pro-ui.html" >/dev/null 2>&1
    elif command_exists open; then
        open "http://localhost:8000/gitgrade-pro-ui.html" >/dev/null 2>&1
    elif command_exists start; then
        start "http://localhost:8000/gitgrade-pro-ui.html" >/dev/null 2>&1
    else
        echo "Please open: http://localhost:8000/gitgrade-pro-ui.html"
    fi
else
    echo ""
    echo -e "${GREEN}==============================================
    echo -e "  🎉 GitGrade backend is running!"
    echo -e "==============================================${NC}"
    echo ""
    echo -e "${BLUE}Backend API:${NC}  http://localhost:18080"
    echo ""
    echo -e "${YELLOW}To use the frontend:${NC}"
    echo "  1. Open gitgrade-pro-ui.html in your browser, or"
    echo "  2. Run: python3 -m http.server 8000"
    echo "     Then open: http://localhost:8000/gitgrade-pro-ui.html"
fi

echo ""
echo -e "${YELLOW}Press Ctrl+C to stop all servers${NC}"
echo ""
echo -e "${BLUE}Tip:${NC} Check backend logs below for analysis progress"
echo ""
echo "=============================="
echo ""

# Wait for user to stop
wait $BACKEND_PID

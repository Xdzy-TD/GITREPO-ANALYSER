# 🚀 Complete Integration Guide: C++ Backend + HTML Frontend + Google Gemini

## 📋 Table of Contents
1. [Prerequisites](#prerequisites)
2. [Getting Gemini API Key](#getting-gemini-api-key)
3. [Installing Dependencies](#installing-dependencies)
4. [Setting Up the Project](#setting-up-the-project)
5. [Compiling the C++ Backend](#compiling-the-c-backend)
6. [Running the Application](#running-the-application)
7. [Testing the Integration](#testing-the-integration)
8. [Troubleshooting](#troubleshooting)

---

## 1️⃣ Prerequisites

### Required Tools
- **C++ Compiler**: g++ (with C++17 support) or clang++
- **Git**: For cloning repositories
- **CMake**: (Optional but recommended) Version 3.10+
- **vcpkg**: C++ package manager
- **Web Browser**: Chrome, Firefox, or Safari

### System Requirements
- Linux, macOS, or Windows (with WSL2)
- At least 2GB RAM
- Internet connection

---

## 2️⃣ Getting Gemini API Key

### Step-by-Step:

1. **Visit Google AI Studio**
   ```
   https://makersuite.google.com/app/apikey
   ```

2. **Sign in** with your Google account

3. **Click "Get API Key"** or "Create API Key"

4. **Copy your API key** - it looks like:
   ```
   AIzaSyDxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   ```

5. **Keep it safe!** Don't commit it to GitHub

### Free Tier Limits:
- ✅ 60 requests per minute
- ✅ Free for testing and development
- ✅ Perfect for hackathons!

---

## 3️⃣ Installing Dependencies

### Option A: Using vcpkg (Recommended)

```bash
# 1. Install vcpkg
cd ~
git clone https://github.com/microsoft/vcpkg
cd vcpkg
./bootstrap-vcpkg.sh  # On Windows: .\bootstrap-vcpkg.bat

# 2. Install required libraries
./vcpkg install crow
./vcpkg install cpr
./vcpkg install nlohmann-json

# 3. Note the vcpkg path (you'll need this)
pwd  # Copy this path
```

### Option B: Manual Installation (Advanced)

```bash
# Install libcurl (for HTTP requests)
sudo apt-get install libcurl4-openssl-dev  # Ubuntu/Debian
brew install curl  # macOS

# Install Crow (header-only, just download)
cd ~/your-project
git clone https://github.com/CrowCpp/Crow.git

# Install nlohmann/json (header-only)
wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
mkdir -p include/nlohmann
mv json.hpp include/nlohmann/

# Install cpr
git clone https://github.com/libcpr/cpr.git
cd cpr && mkdir build && cd build
cmake .. && make && sudo make install
```

---

## 4️⃣ Setting Up the Project

### Project Structure
```
GitGrade/
├── gitgrade_gemini.cpp      # Your C++ backend
├── gitgrade-pro-ui.html      # Your HTML frontend
├── CMakeLists.txt            # Build configuration (optional)
└── README.md
```

### Step 1: Add Your Gemini API Key

Open `gitgrade_gemini.cpp` and find line 13:
```cpp
const std::string GEMINI_API_KEY = "YOUR_GEMINI_API_KEY_HERE";
```

Replace with your actual key:
```cpp
const std::string GEMINI_API_KEY = "AIzaSyDxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
```

⚠️ **Important**: Never commit this file to public GitHub!

### Step 2: Create a .gitignore
```bash
cat > .gitignore << EOF
# Compiled files
*.o
*.out
gitgrade
gitgrade.exe

# Temporary repos
temp_repo_*

# API keys (create a config file instead)
config.h
secrets.h

# Build directories
build/
cmake-build-*/
EOF
```

---

## 5️⃣ Compiling the C++ Backend

### Method 1: Using CMakeLists.txt (Recommended)

Create `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.10)
project(GitGrade)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Add executable
add_executable(gitgrade gitgrade_gemini.cpp)

# Include directories (adjust vcpkg path)
target_include_directories(gitgrade PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ~/vcpkg/installed/x64-linux/include
)

# Link libraries
target_link_libraries(gitgrade 
    CURL::libcurl
    Threads::Threads
)

# Link vcpkg libraries
target_link_directories(gitgrade PRIVATE 
    ~/vcpkg/installed/x64-linux/lib
)
```

Then compile:
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
./gitgrade
```

### Method 2: Direct g++ Compilation

```bash
# Replace /path/to/vcpkg with your actual vcpkg path

g++ -std=c++17 gitgrade_gemini.cpp -o gitgrade \
    -I/path/to/vcpkg/installed/x64-linux/include \
    -L/path/to/vcpkg/installed/x64-linux/lib \
    -lcurl \
    -lpthread \
    -lstdc++fs

# Example with actual path:
g++ -std=c++17 gitgrade_gemini.cpp -o gitgrade \
    -I$HOME/vcpkg/installed/x64-linux/include \
    -L$HOME/vcpkg/installed/x64-linux/lib \
    -lcurl \
    -lpthread \
    -lstdc++fs
```

### Method 3: Simple Compilation (If libraries are system-installed)

```bash
g++ -std=c++17 gitgrade_gemini.cpp -o gitgrade -lcurl -lpthread
```

---

## 6️⃣ Running the Application

### Step 1: Start the C++ Backend

```bash
# Make sure you're in the project directory
./gitgrade

# You should see:
# ==============================================
# 🚀 GitGrade Server Starting...
# ==============================================
# Server URL: http://localhost:18080
# Health Check: http://localhost:18080/health
# ⚡ Ready to analyze repositories!
# ==============================================
```

### Step 2: Open the HTML Frontend

**Option A: Direct File Opening**
```bash
# Just double-click the HTML file, or:
open gitgrade-pro-ui.html  # macOS
xdg-open gitgrade-pro-ui.html  # Linux
start gitgrade-pro-ui.html  # Windows
```

**Option B: Local Web Server (Better for CORS)**
```bash
# Python 3
python3 -m http.server 8000

# Then open: http://localhost:8000/gitgrade-pro-ui.html
```

**Option C: VS Code Live Server**
- Install "Live Server" extension
- Right-click on `gitgrade-pro-ui.html`
- Select "Open with Live Server"

---

## 7️⃣ Testing the Integration

### Test 1: Health Check
```bash
curl http://localhost:18080/health
# Should return: {"status":"healthy"}
```

### Test 2: Simple Analysis
```bash
curl "http://localhost:18080/analyze?repo=https://github.com/torvalds/linux"
```

### Test 3: Frontend Integration

1. Open the HTML page
2. Enter a GitHub repo URL: `https://github.com/octocat/Hello-World`
3. Click "Analyze"
4. You should see:
   - Loading animation with progress steps
   - After ~10-30 seconds: Complete analysis results
   - AI-generated roadmap from Gemini
   - Resume bullets

### Test 4: Verify Gemini Integration

Check the terminal where your C++ server is running. You should see:
```
Analyzing repository: https://github.com/...
Cloning repository...
Running security scan...
Analyzing code complexity...
...
Generating AI insights...
Analysis complete!
```

If you see errors about Gemini API, check your API key!

---

## 8️⃣ Troubleshooting

### Problem: "Cannot find crow.h"
**Solution:**
```bash
# Make sure vcpkg is installed correctly
cd ~/vcpkg
./vcpkg list
# You should see: crow, cpr, nlohmann-json

# If not, install them:
./vcpkg install crow cpr nlohmann-json
```

### Problem: "undefined reference to curl_*"
**Solution:**
```bash
# Install libcurl development package
sudo apt-get install libcurl4-openssl-dev  # Ubuntu
brew install curl  # macOS

# Make sure to link with -lcurl
```

### Problem: "Address already in use" (Port 18080)
**Solution:**
```bash
# Find what's using the port
lsof -i :18080  # macOS/Linux
netstat -ano | findstr :18080  # Windows

# Kill the process or change the port in code:
# Line ~620 in gitgrade_gemini.cpp
app.port(18081).multithreaded().run();  // Change to 18081
```

### Problem: CORS errors in browser console
**Solution:**
The C++ code already includes CORS headers. If you still see errors:

1. **Use a local web server** instead of `file://`
   ```bash
   python3 -m http.server 8000
   ```

2. **Or disable CORS in Chrome** (development only!)
   ```bash
   # macOS
   open -na "Google Chrome" --args --disable-web-security --user-data-dir=/tmp/chrome

   # Linux
   google-chrome --disable-web-security --user-data-dir=/tmp/chrome

   # Windows
   chrome.exe --disable-web-security --user-data-dir=C:\tmp\chrome
   ```

### Problem: "Failed to clone repository"
**Solution:**
```bash
# Make sure git is installed
git --version

# Test cloning manually
git clone https://github.com/octocat/Hello-World test_repo
rm -rf test_repo  # Clean up
```

### Problem: Gemini API returns errors
**Solutions:**

1. **Invalid API Key**
   ```
   Error 400: API key not valid
   ```
   - Double-check your API key
   - Make sure there are no extra spaces
   - Generate a new key if needed

2. **Rate Limit Exceeded**
   ```
   Error 429: Resource exhausted
   ```
   - Wait 60 seconds and try again
   - Reduce analysis frequency

3. **Quota Exceeded**
   ```
   Error 429: Quota exceeded
   ```
   - Check your quotas at: https://makersuite.google.com/app/apikey
   - Free tier: 60 requests/minute

### Problem: Compilation errors
**Common fixes:**

```bash
# If you see "filesystem not found":
g++ -std=c++17 ... -lstdc++fs

# If you see "thread not found":
g++ -std=c++17 ... -lpthread

# If you see "json.hpp not found":
# Download it:
wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
mkdir -p include/nlohmann
mv json.hpp include/nlohmann/
```

---

## 🎯 Quick Start Script

Save this as `run.sh`:

```bash
#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}🚀 GitGrade Quick Start${NC}"
echo ""

# Check if binary exists
if [ ! -f "./gitgrade" ]; then
    echo -e "${YELLOW}⚠️  Compiling gitgrade...${NC}"
    g++ -std=c++17 gitgrade_gemini.cpp -o gitgrade \
        -I$HOME/vcpkg/installed/x64-linux/include \
        -L$HOME/vcpkg/installed/x64-linux/lib \
        -lcurl -lpthread -lstdc++fs
    
    if [ $? -ne 0 ]; then
        echo "❌ Compilation failed!"
        exit 1
    fi
    echo -e "${GREEN}✅ Compilation successful!${NC}"
fi

# Start backend in background
echo -e "${YELLOW}Starting backend server...${NC}"
./gitgrade &
BACKEND_PID=$!

# Wait for server to start
sleep 2

# Start frontend server
echo -e "${YELLOW}Starting frontend server...${NC}"
python3 -m http.server 8000 &
FRONTEND_PID=$!

sleep 1

echo ""
echo -e "${GREEN}✅ GitGrade is running!${NC}"
echo ""
echo "Backend:  http://localhost:18080"
echo "Frontend: http://localhost:8000/gitgrade-pro-ui.html"
echo ""
echo "Press Ctrl+C to stop both servers"
echo ""

# Wait for Ctrl+C
trap "kill $BACKEND_PID $FRONTEND_PID; exit" INT
wait
```

Make it executable and run:
```bash
chmod +x run.sh
./run.sh
```

---

## 📊 Expected Performance

- **Small repos** (< 100 files): 5-10 seconds
- **Medium repos** (100-1000 files): 10-30 seconds
- **Large repos** (1000+ files): 30-60 seconds

The slowest part is usually:
1. Git cloning (depends on repo size)
2. Gemini API call (2-5 seconds)
3. File system scanning (depends on number of files)

---

## 🎉 Success Checklist

- ✅ C++ backend compiles without errors
- ✅ Server starts on port 18080
- ✅ `/health` endpoint returns `{"status":"healthy"}`
- ✅ HTML page loads in browser
- ✅ Can enter GitHub URL
- ✅ Analysis completes successfully
- ✅ See AI-generated roadmap from Gemini
- ✅ Resume bullets appear
- ✅ No CORS errors in browser console

---

## 🚀 Next Steps

1. **Add error handling** to the frontend
2. **Cache results** to avoid re-analyzing
3. **Add more repo sources** (GitLab, Bitbucket)
4. **Create Docker container** for easy deployment
5. **Add database** to store analysis history

---

## 🆘 Still Having Issues?

1. Check all terminal outputs for error messages
2. Verify your Gemini API key is correct
3. Make sure port 18080 is not blocked by firewall
4. Try compiling with verbose output: `g++ -v ...`
5. Test each component separately (backend, frontend, API)

Good luck! 🎉

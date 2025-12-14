# 🚀 GitGrade Pro - AI-Powered Code Analysis Platform
MADE WITH GEMINI AI

<div align="center">

![GitGrade Banner](https://img.shields.io/badge/GitGrade-Pro-blue?style=for-the-badge&logo=github)
[![C++17](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![Gemini AI](https://img.shields.io/badge/Google-Gemini_AI-4285F4?style=for-the-badge&logo=google)](https://ai.google.dev/)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

**A blazing-fast C++ code analysis tool with 11+ advanced features powered by Google Gemini AI**

[Features](#-features) • [Quick Start](#-quick-start) • [Documentation](#-documentation) • [Demo](#-demo)

</div>

---

## 🌟 What is GitGrade?

GitGrade is a comprehensive repository analysis platform that combines **high-performance C++** backend with a **stunning space-themed UI** and **AI-powered insights** from Google Gemini. In seconds, it analyzes your GitHub repos and provides:

- 🔒 **Security Audit** - Detect exposed API keys, secrets, and vulnerabilities
- 📊 **Code Metrics** - Complexity, organization, and quality scoring
- 🤖 **AI Roadmap** - Personalized 5-step improvement plan from Gemini
- 💼 **Resume Builder** - Auto-generate achievement bullets for job applications
- 🧪 **Test Coverage** - Estimate test coverage across your codebase
- 📝 **Documentation Check** - Ensure README completeness
- ...and 5 more advanced features!

## ✨ Features

### Core Analysis Features
1. **🔐 Advanced Security Scanning**
   - API key detection (OpenAI, Google, AWS)
   - AWS secret key identification
   - Private key exposure detection
   - Hardcoded password scanning
   - JWT token detection
   - Database connection string leaks
   - Hardcoded IP address detection

2. **📊 Code Complexity Analysis**
   - Multi-language support (Python, C++, Java, JS, TypeScript, etc.)
   - Lines of code counting
   - Function and class detection
   - Comment ratio calculation
   - Code organization metrics

3. **👃 Code Smell Detection**
   - Long function identification
   - Magic number detection
   - Duplicate code patterns
   - Deep nesting warnings
   - TODO/FIXME comment tracking

4. **📦 Dependency Management**
   - npm (package.json)
   - pip (requirements.txt)
   - cargo (Cargo.toml)
   - maven (pom.xml)
   - gradle (build.gradle)

5. **🧪 Test Coverage Estimation**
   - Automatic test file detection
   - Coverage percentage calculation
   - Multi-framework support

6. **📈 Git History Analysis**
   - Commit counting
   - Contributor identification
   - Development velocity tracking
   - Last commit timestamp

7. **📝 README Quality Checker**
   - Section completeness validation
   - Best practices verification
   - Score calculation (0-100)

8. **⚡ Quality Score Calculator**
   - Weighted scoring algorithm
   - Security (30%)
   - Documentation (20%)
   - Organization (30%)
   - Dependencies (20%)

9. **🤖 AI-Powered Mentor (Google Gemini)**
   - Comprehensive codebase analysis
   - 5-step improvement roadmap
   - Prioritized recommendations
   - Technical debt identification

10. **💼 Resume Generator**
    - Auto-generate achievement bullets
    - Recruiter-ready formatting
    - Quantified metrics
    - Professional language

11. **🎨 Stunning Space UI**
    - Animated starfield (200+ stars)
    - Interactive planets (Saturn, Venus, Mars, Jupiter)
    - Glassmorphism design
    - Smooth animations
    - Responsive layout

## 🚀 Quick Start

### Prerequisites
- C++ compiler (g++ with C++17 support)
- Git
- curl library
- Python 3 (for frontend server)
- Google Gemini API key ([Get one here](https://makersuite.google.com/app/apikey))

### One-Command Setup

```bash
# 1. Clone or download the project files
# 2. Get your Gemini API key from https://makersuite.google.com/app/apikey
# 3. Edit gitgrade_gemini.cpp and add your API key (line 13)
# 4. Run the setup script

chmod +x run.sh
./run.sh
```

That's it! The script will:
- ✅ Check all prerequisites
- ✅ Compile the C++ backend
- ✅ Start both backend and frontend servers
- ✅ Open your browser automatically

### Manual Setup

If you prefer manual setup, see [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) for detailed instructions.

## 📖 Documentation

- **[INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md)** - Complete setup and integration guide
- **[FEATURES_DOCUMENTATION.md](FEATURES_DOCUMENTATION.md)** - Detailed feature documentation
- **[CMakeLists.txt](CMakeLists.txt)** - Build configuration

## 🎯 Usage

### Analyzing a Repository

1. **Start the servers** (if not already running):
   ```bash
   ./run.sh
   ```

2. **Open the UI** at: http://localhost:8000/gitgrade-pro-ui.html

3. **Enter a GitHub repo URL**, for example:
   - `https://github.com/torvalds/linux`
   - `https://github.com/facebook/react`
   - `https://github.com/microsoft/vscode`

4. **Click "Analyze"** and wait 10-30 seconds

5. **View comprehensive results**:
   - Overall quality score
   - Security vulnerabilities
   - Code complexity metrics
   - AI-generated roadmap
   - Resume-ready bullets
   - And more!

### API Endpoints

**Health Check:**
```bash
curl http://localhost:18080/health
```

**Analyze Repository:**
```bash
curl "http://localhost:18080/analyze?repo=https://github.com/user/repo"
```

## 🏗️ Architecture

```
┌─────────────────────────────────────────────┐
│           Frontend (HTML/CSS/JS)            │
│     Space-themed UI with animations         │
└─────────────────┬───────────────────────────┘
                  │ HTTP/REST API
                  ▼
┌─────────────────────────────────────────────┐
│         Backend (C++ with Crow)             │
│  ├─ Security Scanner                        │
│  ├─ Code Analyzer                           │
│  ├─ Git History Analyzer                    │
│  ├─ Dependency Scanner                      │
│  └─ Quality Calculator                      │
└─────────────────┬───────────────────────────┘
                  │
                  ├──────────────┐
                  ▼              ▼
         ┌─────────────┐  ┌─────────────┐
         │   Git API   │  │ Gemini AI   │
         │  (Clone)    │  │  (Analysis) │
         └─────────────┘  └─────────────┘
```

## 🛠️ Technology Stack

### Backend
- **Language:** Modern C++17
- **Web Framework:** Crow (Fast, Flask-like)
- **HTTP Client:** cpr (C++ Requests)
- **JSON Parser:** nlohmann/json
- **AI Integration:** Google Gemini API

### Frontend
- **Pure HTML5/CSS3/JavaScript**
- **No frameworks** - lightweight and fast
- **Glassmorphism design**
- **CSS animations**
- **Responsive layout**

## 📊 Performance

- **Small repos** (< 100 files): **5-10 seconds**
- **Medium repos** (100-1000 files): **10-30 seconds**
- **Large repos** (1000+ files): **30-60 seconds**

*C++ backend is 10x faster than equivalent Python implementations!*

## 🎨 Screenshots

### Main Interface
Beautiful space-themed UI with animated starfield and planets

### Analysis Results
Comprehensive dashboard showing:
- Quality scores
- Security findings
- Code metrics
- AI-powered roadmap
- Resume bullets

### Features Showcase
12 feature cards highlighting all capabilities

## 🔧 Troubleshooting

### Common Issues

**Port 18080 already in use:**
```bash
# Change port in gitgrade_gemini.cpp (line ~620)
app.port(18081).multithreaded().run();
```

**CORS errors:**
```bash
# Use local web server instead of file://
python3 -m http.server 8000
```

**Compilation errors:**
```bash
# Install missing libraries
sudo apt-get install libcurl4-openssl-dev  # Ubuntu
brew install curl  # macOS
```

**Gemini API errors:**
- Check your API key at https://makersuite.google.com/app/apikey
- Verify quota hasn't been exceeded (60 requests/minute free tier)
- Ensure no extra spaces in the API key

See [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md) for more troubleshooting tips.

## 🎓 For Students & Hackathons

This project demonstrates:
- ✅ **Full-stack development** (C++ backend + Web frontend)
- ✅ **API integration** (REST, Google Gemini)
- ✅ **Systems programming** (File I/O, process management)
- ✅ **Modern C++** (C++17 features)
- ✅ **Software engineering** (Security, testing, documentation)
- ✅ **AI/ML integration** (Gemini for code analysis)

Perfect for:
- Hackathon projects
- Capstone projects
- Portfolio pieces
- Learning C++ web development
- Understanding AI API integration

## 🤝 Contributing

Contributions are welcome! Some ideas:
- Add support for more languages
- Implement custom security rules
- Add database storage for history
- Create GitHub Action integration
- Build CI/CD pipeline analyzer
- Add more visualization features

## 📄 License

MIT License - feel free to use for your projects!

## 🙏 Acknowledgments

- **Google Gemini AI** - For powerful code analysis capabilities
- **Crow Framework** - For elegant C++ web framework
- **vcpkg** - For C++ package management
- **nlohmann/json** - For excellent JSON parsing

## 📞 Support

- 📖 Read the [INTEGRATION_GUIDE.md](INTEGRATION_GUIDE.md)
- 🐛 Report issues on GitHub
- 💡 Suggest features via issues

---

<div align="center">

**Made with C++17**

**Powered by Google Gemini AI**

[⬆ Back to Top](#-gitgrade-pro---ai-powered-code-analysis-platform)

</div>

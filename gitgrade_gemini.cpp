#include <Crow/crow.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>
#include <regex>
#include <fstream>
#include <map>
#include <set>
#include <algorithm>
#include <ctime>

using json = nlohmann::json;
namespace fs = std::filesystem;

// ==================== CONFIGURATION ====================
const std::string GEMINI_API_KEY = "AIzaSyBPvSjGACz3O7mRnHfZOLKgukexkrsrcn0";
const std::string GEMINI_API_URL = "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent";

// ==================== FEATURE 1: REPO FETCHING ====================
void clone_repo(std::string url, std::string local_path) {
    std::string command = "git clone " + url + " " + local_path + " 2>&1";
    int result = system(command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to clone repository");
    }
}

// ==================== FEATURE 2: SECURITY AUDIT ====================
json run_security_scan(std::string path) {
    json findings = json::array();
    
    std::map<std::string, std::regex> security_patterns = {
        {"API Key", std::regex("(AIza|sk-proj-|sk-|AKIA)[a-zA-Z0-9_\\-]{20,}")},
        {"AWS Secret", std::regex("aws_secret_access_key\\s*=\\s*['\"]?[A-Za-z0-9/+=]{40}['\"]?")},
        {"Private Key", std::regex("-----BEGIN (RSA |EC |DSA )?PRIVATE KEY-----")},
        {"Password in Code", std::regex("(password|passwd|pwd)\\s*=\\s*['\"][^'\"]{3,}['\"]", std::regex::icase)},
        {"JWT Token", std::regex("eyJ[A-Za-z0-9_-]*\\.eyJ[A-Za-z0-9_-]*\\.[A-Za-z0-9_-]*")},
        {"Database Connection", std::regex("(mongodb|mysql|postgresql)://[^\\s]+", std::regex::icase)},
        {"Hardcoded IP", std::regex("\\b(?:[0-9]{1,3}\\.){3}[0-9]{1,3}\\b")}
    };

    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path());
            std::string line;
            int line_num = 0;
            
            while (std::getline(file, line)) {
                line_num++;
                for (const auto& [issue_type, pattern] : security_patterns) {
                    if (std::regex_search(line, pattern)) {
                        findings.push_back({
                            {"file", entry.path().filename().string()},
                            {"line", line_num},
                            {"issue", issue_type + " Detected"},
                            {"severity", "HIGH"}
                        });
                    }
                }
            }
        }
    }
    return findings;
}

// ==================== FEATURE 3: CODE COMPLEXITY ANALYZER ====================
struct FileStats {
    int lines = 0;
    int blank_lines = 0;
    int comment_lines = 0;
    int functions = 0;
    int classes = 0;
};

json analyze_code_complexity(std::string path) {
    std::map<std::string, FileStats> language_stats;
    int total_files = 0;
    
    std::map<std::string, std::vector<std::regex>> patterns = {
        {"function", {std::regex("\\bdef\\s+\\w+"), std::regex("\\w+\\s+\\w+\\s*\\([^)]*\\)\\s*\\{")}},
        {"class", {std::regex("\\bclass\\s+\\w+"), std::regex("\\bstruct\\s+\\w+")}}
    };

    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext.empty()) continue;
            
            total_files++;
            FileStats& stats = language_stats[ext];
            
            std::ifstream file(entry.path());
            std::string line;
            
            while (std::getline(file, line)) {
                stats.lines++;
                
                if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
                    stats.blank_lines++;
                    continue;
                }
                
                if (line.find("//") != std::string::npos || 
                    line.find("#") != std::string::npos ||
                    line.find("/*") != std::string::npos) {
                    stats.comment_lines++;
                }
                
                for (const auto& pattern : patterns["function"]) {
                    if (std::regex_search(line, pattern)) stats.functions++;
                }
                for (const auto& pattern : patterns["class"]) {
                    if (std::regex_search(line, pattern)) stats.classes++;
                }
            }
        }
    }

    json result = {
        {"total_files", total_files},
        {"languages", json::object()}
    };

    for (const auto& [ext, stats] : language_stats) {
        result["languages"][ext] = {
            {"total_lines", stats.lines},
            {"code_lines", stats.lines - stats.blank_lines - stats.comment_lines},
            {"blank_lines", stats.blank_lines},
            {"comment_lines", stats.comment_lines},
            {"functions", stats.functions},
            {"classes", stats.classes},
            {"comment_ratio", stats.lines > 0 ? (float)stats.comment_lines / stats.lines : 0.0}
        };
    }

    return result;
}

// ==================== FEATURE 4: DEPENDENCY ANALYZER ====================
json analyze_dependencies(std::string path) {
    json dependencies = {
        {"package_managers", json::object()},
        {"total_dependencies", 0}
    };

    std::map<std::string, std::string> package_files = {
        {"npm", "package.json"},
        {"pip", "requirements.txt"},
        {"cargo", "Cargo.toml"},
        {"maven", "pom.xml"},
        {"gradle", "build.gradle"}
    };

    for (const auto& [manager, filename] : package_files) {
        std::string file_path = path + "/" + filename;
        if (fs::exists(file_path)) {
            std::ifstream file(file_path);
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            
            int dep_count = std::count(content.begin(), content.end(), '\n');
            dependencies["package_managers"][manager] = {
                {"found", true},
                {"file", filename},
                {"estimated_dependencies", dep_count}
            };
            dependencies["total_dependencies"] = dependencies["total_dependencies"].get<int>() + dep_count;
        }
    }

    return dependencies;
}

// ==================== FEATURE 5: CODE QUALITY METRICS ====================
json calculate_quality_score(json complexity, json security, json dependencies) {
    int score = 100;
    json breakdown = json::object();

    int security_issues = security.size();
    int security_penalty = std::min(30, security_issues * 5);
    score -= security_penalty;
    breakdown["security"] = {{"score", 100 - security_penalty}, {"issues", security_issues}};

    float avg_comment_ratio = 0.0;
    int lang_count = 0;
    if (complexity.contains("languages")) {
        for (auto& [lang, stats] : complexity["languages"].items()) {
            avg_comment_ratio += stats["comment_ratio"].get<float>();
            lang_count++;
        }
        if (lang_count > 0) avg_comment_ratio /= lang_count;
    }
    
    int doc_score = std::min(100, (int)(avg_comment_ratio * 500));
    breakdown["documentation"] = {{"score", doc_score}, {"comment_ratio", avg_comment_ratio}};

    int total_files = complexity["total_files"].get<int>();
    int org_score = total_files > 50 ? 70 : (total_files > 20 ? 85 : 100);
    breakdown["organization"] = {{"score", org_score}, {"files", total_files}};

    bool has_deps = dependencies["total_dependencies"].get<int>() > 0;
    int dep_score = has_deps ? 100 : 80;
    breakdown["dependency_management"] = {{"score", dep_score}};

    int final_score = (score * 0.3) + (doc_score * 0.2) + (org_score * 0.3) + (dep_score * 0.2);

    return {
        {"overall_score", std::min(100, std::max(0, final_score))},
        {"breakdown", breakdown}
    };
}

// ==================== FEATURE 6: GIT HISTORY ANALYSIS ====================
json analyze_git_history(std::string path) {
    json history = json::object();
    
    std::string cmd = "cd " + path + " && git rev-list --count HEAD 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, 128, pipe)) {
            history["total_commits"] = std::stoi(buffer);
        }
        pclose(pipe);
    }

    cmd = "cd " + path + " && git shortlog -sn --all 2>/dev/null | wc -l";
    pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, 128, pipe)) {
            history["contributors"] = std::stoi(buffer);
        }
        pclose(pipe);
    }

    cmd = "cd " + path + " && git log -1 --format=%cd 2>/dev/null";
    pipe = popen(cmd.c_str(), "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, 256, pipe)) {
            std::string date(buffer);
            date.erase(date.find_last_not_of("\n\r") + 1);
            history["last_commit"] = date;
        }
        pclose(pipe);
    }

    return history;
}

// ==================== FEATURE 7: CODE SMELL DETECTOR ====================
json detect_code_smells(std::string path) {
    json smells = json::array();

    std::map<std::string, std::regex> smell_patterns = {
        {"Long Function", std::regex("\\{[^}]{2000,}\\}")},
        {"Magic Numbers", std::regex("\\b(\\d{3,})\\b")},
        {"Deep Nesting", std::regex("\\{[^}]*\\{[^}]*\\{[^}]*\\{[^}]*\\{")},
        {"TODO Comments", std::regex("(TODO|FIXME|HACK|XXX)", std::regex::icase)}
    };

    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path());
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            
            for (const auto& [smell_name, pattern] : smell_patterns) {
                std::sregex_iterator iter(content.begin(), content.end(), pattern);
                std::sregex_iterator end;
                int count = std::distance(iter, end);
                
                if (count > 0) {
                    smells.push_back({
                        {"type", smell_name},
                        {"file", entry.path().filename().string()},
                        {"occurrences", count}
                    });
                }
            }
        }
    }

    return smells;
}

// ==================== FEATURE 8: README QUALITY CHECKER ====================
json check_readme_quality(std::string path) {
    json readme_score = {
        {"exists", false},
        {"score", 0},
        {"missing_sections", json::array()}
    };

    std::vector<std::string> readme_names = {"README.md", "README.MD", "readme.md", "README.txt"};
    std::string readme_path;
    
    for (const auto& name : readme_names) {
        std::string test_path = path + "/" + name;
        if (fs::exists(test_path)) {
            readme_path = test_path;
            readme_score["exists"] = true;
            break;
        }
    }

    if (!readme_score["exists"].get<bool>()) {
        readme_score["missing_sections"] = {"README file not found"};
        return readme_score;
    }

    std::ifstream file(readme_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    std::vector<std::pair<std::string, std::regex>> sections = {
        {"Title/Description", std::regex("#\\s+\\w+", std::regex::icase)},
        {"Installation", std::regex("install", std::regex::icase)},
        {"Usage", std::regex("usage|example", std::regex::icase)},
        {"Contributing", std::regex("contribut", std::regex::icase)},
        {"License", std::regex("license", std::regex::icase)},
        {"Dependencies", std::regex("depend|requirement", std::regex::icase)}
    };

    int found_sections = 0;
    for (const auto& [section, pattern] : sections) {
        if (std::regex_search(content, pattern)) {
            found_sections++;
        } else {
            readme_score["missing_sections"].push_back(section);
        }
    }

    readme_score["score"] = (found_sections * 100) / sections.size();
    return readme_score;
}

// ==================== FEATURE 9: TEST COVERAGE ESTIMATOR ====================
json estimate_test_coverage(std::string path) {
    int test_files = 0;
    int total_code_files = 0;
    
    std::vector<std::regex> test_patterns = {
        std::regex("test_.*\\.(py|js|cpp|java)"),
        std::regex(".*_test\\.(py|js|cpp|java)"),
        std::regex(".*\\.test\\.(js|ts)"),
        std::regex(".*\\.spec\\.(js|ts)")
    };

    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::string ext = entry.path().extension().string();
            
            if (ext == ".py" || ext == ".js" || ext == ".cpp" || ext == ".java" || ext == ".ts") {
                total_code_files++;
                
                for (const auto& pattern : test_patterns) {
                    if (std::regex_search(filename, pattern)) {
                        test_files++;
                        break;
                    }
                }
            }
        }
    }

    float coverage = total_code_files > 0 ? (float)test_files / total_code_files * 100 : 0;
    
    return {
        {"test_files", test_files},
        {"code_files", total_code_files},
        {"estimated_coverage", coverage},
        {"has_tests", test_files > 0}
    };
}

// ==================== FEATURE 10: GOOGLE GEMINI AI MENTOR ====================
std::string get_gemini_analysis(json full_analysis) {
    // Build the prompt
    std::stringstream prompt;
    prompt << "You are an expert code reviewer. Analyze this repository data and provide a detailed 5-step improvement roadmap.\n\n";
    prompt << "Repository Analysis:\n";
    prompt << "- Total Files: " << full_analysis["complexity"]["total_files"] << "\n";
    prompt << "- Security Issues: " << full_analysis["security"].size() << "\n";
    prompt << "- Test Coverage: " << full_analysis["test_coverage"]["estimated_coverage"] << "%\n";
    prompt << "- README Score: " << full_analysis["readme_quality"]["score"] << "/100\n";
    prompt << "- Code Smells: " << full_analysis["code_smells"].size() << "\n\n";
    prompt << "Provide 5 specific, actionable steps to improve this codebase. Format each step clearly.";

    // Build Gemini API request
    json request_body = {
        {"contents", json::array({
            {
                {"parts", json::array({
                    {{"text", prompt.str()}}
                })}
            }
        })}
    };

    try {
        auto response = cpr::Post(
            cpr::Url{GEMINI_API_URL + "?key=" + GEMINI_API_KEY},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{request_body.dump()},
            cpr::Timeout{30000}
        );

        if (response.status_code == 200) {
            auto json_response = json::parse(response.text);
            
            if (json_response.contains("candidates") && !json_response["candidates"].empty()) {
                auto candidate = json_response["candidates"][0];
                if (candidate.contains("content") && candidate["content"].contains("parts")) {
                    return candidate["content"]["parts"][0]["text"];
                }
            }
        }
        
        std::cerr << "Gemini API Error: " << response.status_code << " - " << response.text << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception calling Gemini: " << e.what() << std::endl;
    }
    
    return "AI analysis unavailable. Please check your Gemini API key and internet connection.";
}

// ==================== FEATURE 11: RESUME GENERATOR ====================
std::string generate_resume_bullets(json analysis) {
    std::stringstream resume;
    
    auto complexity = analysis["complexity"];
    int total_lines = 0;
    std::vector<std::string> languages;
    
    if (complexity.contains("languages")) {
        for (auto& [lang, stats] : complexity["languages"].items()) {
            total_lines += stats["code_lines"].get<int>();
            languages.push_back(lang);
        }
    }
    
    resume << "• Developed production-grade software with " << total_lines << "+ lines of code";
    if (!languages.empty()) {
        resume << " across " << languages.size() << " languages\n";
    } else {
        resume << "\n";
    }
    
    int security_count = analysis["security"].size();
    if (security_count == 0) {
        resume << "• Implemented secure coding practices with zero security vulnerabilities detected\n";
    } else {
        resume << "• Conducted comprehensive security audit identifying " << security_count << " areas for improvement\n";
    }
    
    resume << "• Maintained code quality score of " << analysis["quality_score"]["overall_score"] << "/100 through best practices\n";
    
    if (analysis.contains("git_history") && analysis["git_history"].contains("total_commits")) {
        resume << "• Contributed " << analysis["git_history"]["total_commits"] << " commits";
        if (analysis["git_history"].contains("contributors")) {
            resume << " with " << analysis["git_history"]["contributors"] << " collaborators\n";
        } else {
            resume << "\n";
        }
    }
    
    if (analysis.contains("test_coverage") && analysis["test_coverage"]["has_tests"].get<bool>()) {
        resume << "• Achieved " << (int)analysis["test_coverage"]["estimated_coverage"].get<float>() 
               << "% test coverage with automated testing\n";
    }
    
    return resume.str();
}

// ==================== MAIN SERVER ====================
int main() {
    crow::SimpleApp app;

    // CORS middleware for all routes
    CROW_ROUTE(app, "/<path>").methods("OPTIONS"_method)
    ([](const crow::request& req, std::string path){
        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        return res;
    });

    // Health check endpoint
    CROW_ROUTE(app, "/health")
    ([](){
        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Content-Type", "application/json");
        res.write(json({{"status", "healthy"}}).dump());
        return res;
    });

    // Main analysis endpoint
    CROW_ROUTE(app, "/analyze")
    ([](const crow::request& req){
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Content-Type", "application/json");
        
        auto repo_url = req.url_params.get("repo");
        if (!repo_url) {
            res.code = 400;
            res.write(json({{"error", "Missing repo URL"}}).dump());
            return res;
        }

        std::string local_id = "temp_repo_" + std::to_string(std::time(0));
        
        try {
            std::cout << "Analyzing repository: " << repo_url << std::endl;
            
            // 1. Clone repository
            std::cout << "Cloning repository..." << std::endl;
            clone_repo(repo_url, local_id);

            // 2. Run all analyses
            std::cout << "Running security scan..." << std::endl;
            json security = run_security_scan(local_id);
            
            std::cout << "Analyzing code complexity..." << std::endl;
            json complexity = analyze_code_complexity(local_id);
            
            std::cout << "Analyzing dependencies..." << std::endl;
            json dependencies = analyze_dependencies(local_id);
            
            std::cout << "Analyzing git history..." << std::endl;
            json git_history = analyze_git_history(local_id);
            
            std::cout << "Detecting code smells..." << std::endl;
            json code_smells = detect_code_smells(local_id);
            
            std::cout << "Checking README quality..." << std::endl;
            json readme = check_readme_quality(local_id);
            
            std::cout << "Estimating test coverage..." << std::endl;
            json tests = estimate_test_coverage(local_id);
            
            std::cout << "Calculating quality score..." << std::endl;
            json quality = calculate_quality_score(complexity, security, dependencies);

            // 3. Build comprehensive report
            json full_analysis = {
                {"security", security},
                {"complexity", complexity},
                {"dependencies", dependencies},
                {"git_history", git_history},
                {"code_smells", code_smells},
                {"readme_quality", readme},
                {"test_coverage", tests},
                {"quality_score", quality}
            };

            // 4. Get AI insights (if API key is set)
            std::cout << "Generating AI insights..." << std::endl;
            std::string ai_roadmap = get_gemini_analysis(full_analysis);
            full_analysis["ai_roadmap"] = ai_roadmap;

            // 5. Generate resume bullets
            std::cout << "Generating resume bullets..." << std::endl;
            std::string resume = generate_resume_bullets(full_analysis);
            full_analysis["resume_bullets"] = resume;

            // 6. Cleanup
            std::cout << "Cleaning up..." << std::endl;
            std::filesystem::remove_all(local_id);

            std::cout << "Analysis complete!" << std::endl;
            res.code = 200;
            res.write(full_analysis.dump());
            return res;

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::filesystem::remove_all(local_id);
            res.code = 500;
            res.write(json({{"error", e.what()}}).dump());
            return res;
        }
    });

    std::cout << "\n";
    std::cout << "==============================================\n";
    std::cout << "🚀 GitGrade Server Starting...\n";
    std::cout << "==============================================\n";
    std::cout << "Server URL: http://localhost:18080\n";
    std::cout << "Health Check: http://localhost:18080/health\n";
    std::cout << "Analyze Endpoint: http://localhost:18080/analyze?repo=<url>\n";
    std::cout << "\n";
    std::cout << "⚡ Ready to analyze repositories!\n";
    std::cout << "==============================================\n\n";

    app.port(18080).multithreaded().run();
    
    return 0;
}

#include "http_utils.hpp"

namespace Utils {
    std::string get_date() {
        std::time_t time = std::time(nullptr);
        std::tm tm = *std::gmtime(&time);
        char buffer[100];
        if (std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &tm)) {
            return std::string(buffer);
        }
        return "Thu, 01 Jan 1970 00:00:00 GMT";
    }
    
    std::string get_content_type(const std::string& file_name) {
        std::filesystem::path filePath(file_name);
        std::string ext = filePath.extension();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
        if (ext == ".html" || ext == "htm") return "text/html";
        if (ext == ".txt") return "text/plain";
        if (ext == ".css") return "text/css";
        if (ext == ".js") return "application/javascript";
        if (ext == ".json") return "application/json";
        if (ext == ".xml") return "application/xml";
        if (ext == ".jpg" || ext == "jpeg") return "image/jpeg";
        if (ext == ".png") return "image/png";
        if (ext == ".gif") return "image/gif";
        if (ext == ".svg") return "image/svg+xml";
        if (ext == ".ico") return "image/x-icon";
        //default
        return "text/plain";
    }
    
    std::string convert_to_string(std::vector<uint8_t>& data) {
        return std::string(data.begin(), data.end());
    }
    std::vector<uint8_t> convert_to_vector(const std::string& str) {
        return std::vector<uint8_t>(str.begin(), str.end());
    }
}

namespace FileIO {
    void write_to_file(const std::string& filename, const std::vector<uint8_t>& data) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::ios_base::failure("Failed to open file for writing: " + filename);
        }
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
    
    bool read_file_content(const std::string& file_path, std::vector<uint8_t>& content) {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return false;
        }
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
    
        content.resize(static_cast<size_t>(size));
        if (file.read(reinterpret_cast<char*>(content.data()), size)) {
            return true; 
        } else {
            return false;
        }
    }
    
}

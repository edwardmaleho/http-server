#pragma once

#include <string>
#include <filesystem>
#include <functional>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>

namespace Utils {
    std::string get_date();
    std::string get_content_type(const std::string& file_name);
    
    std::string convert_to_string(std::vector<uint8_t>& data);
    std::vector<uint8_t> convert_to_vector(const std::string& str);
}

namespace FileIO {
    void write_to_file(const std::string& filename, const std::vector<uint8_t>& data);
    bool read_file_content(const std::string& file_path, std::vector<uint8_t>& content);
    
}

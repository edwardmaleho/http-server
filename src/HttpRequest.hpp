#pragma once

#include <string>
#include <sstream>
#include <map>
#include <iostream>

struct HttpRequest {
    std::string http_version, request_type, target_url;
    std::map<std::string, std::string> headers;

    bool parse(const std::string& raw_data)  {
        std::stringstream stream (raw_data);
        std::string line;
        auto remove_carriage = [](std::string& str) {
            if (!str.empty() && str.back() == '\r') {
                str.pop_back();
            }
        };
        stream >> request_type >> target_url >> http_version;
        remove_carriage(http_version);
        
        while (std::getline(stream, line)) {
            remove_carriage(line);
            auto pos = line.find(':');
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            std::cout << key << " " << value << std::endl;
        }
        return true;
    }

};
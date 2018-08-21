#include <iostream>
#include <fstream>
#include <sstream>

namespace util {
    std::string readFileToString(const std::string &path) {
        std::ifstream stream(path);
        std::stringstream buffer;
        buffer << stream.rdbuf();
        buffer.flush();

        return buffer.str();
    }

    void appendToFile(const std::string &filePath, const std::string &data) {
        std::ofstream outfile;

        outfile.open(filePath);
        outfile << data;
    }
}
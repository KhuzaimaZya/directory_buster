#include "LoadWordlist.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string.h>


std::vector<std::string> LoadWordlist(const std::string& filename, int n_lines) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return std::vector<std::string>();
    }

    std::string word;
    std::vector<std::string> words;

    if (n_lines > 0) {
        while (n_lines-- > 0 && file >> word) {
            words.push_back(word);
        }
    } else {
        std::copy(std::istream_iterator<std::string>(file), std::istream_iterator<std::string>(), std::back_inserter(words));
    }

    file.close();
    return words;
}


std::vector<MyStruct> LoadWordlistStruct(const std::string& filename, int n_lines) {
    std::vector<std::string> words = LoadWordlist(filename, n_lines);
    std::vector<MyStruct> send_strings;
    send_strings.reserve(words.size());
    for (const auto& word : words) {
        send_strings.emplace_back();
        strcpy(send_strings.back().str, word.c_str());
    }
    return send_strings;
}
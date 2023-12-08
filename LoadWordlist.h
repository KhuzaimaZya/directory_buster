#pragma once
#include <vector>
#include <string>

#define MAX_STRING_LENGTH 256

struct MyStruct {
    char str[MAX_STRING_LENGTH] = {};
    operator char*() { return str; }
};

std::vector<std::string> LoadWordlist(const std::string& filename, int n_lines = -1);
std::vector<MyStruct> LoadWordlistStruct(const std::string& filename, int n_lines = -1);
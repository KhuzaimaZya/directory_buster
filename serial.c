#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <vector>
#include <chrono>

std::vector<std::string> LoadWordlist(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return std::vector<std::string>();
    }

    std::string word;
    std::vector<std::string> words;

    // Read words from the file (one word per line)
    while (std::getline(file, word)) {
        words.push_back(word);
    }

    file.close();
    return words;
}

void d_buster(CURL* curl, const std::string& baseUrl) {
    const auto& wordlist = LoadWordlist("wordlist.txt");

    // Append each word to the base URL and print the resulting URLs
    for (int i = 0; i < wordlist.size(); i++) {
        std::string url = baseUrl + "/" + wordlist[i];

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Perform the HTTP HEAD request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            continue;
        }

        // Get and print the HTTP response code
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        // if (response_code != 404) {
        //     std::cout << response_code << " " << url << std::endl;
        // }
    }
}

int main() {
    // Initialize libcurl
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize libcurl" << std::endl;
        return 1;
    }

    // Disable body data retrieval to get only headers
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    auto start = std::chrono::steady_clock::now();
    d_buster(curl, "https://flexstudent.nu.edu.pk");
    auto end = std::chrono::steady_clock::now();

    // Clean up libcurl
    curl_easy_cleanup(curl);

	// std::cout << "Time: " << std::chrono::duration<double>(end - start).count() << std::endl;
	std::cout << std::chrono::duration<double>(end - start).count() << std::endl;
	
    return 0;
}

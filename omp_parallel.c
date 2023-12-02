#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <omp.h>

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

void d_buster(CURL* curl[], const std::string& baseUrl, int num_threads) {
    const auto& wordlist = LoadWordlist("wordlist.txt");

    // Append each word to the base URL and print the resulting URLs
    #pragma omp parallel for schedule(dynamic, 1) num_threads(num_threads)
    for (int i = 0; i < wordlist.size(); i++) {
        const int thread_id = omp_get_thread_num();
        std::string url = baseUrl + "/" + wordlist[i];
        // std::cout << url << std::endl; 

        // Set the URL
        curl_easy_setopt(curl[thread_id], CURLOPT_URL, url.c_str());

        // Perform the HTTP HEAD request
        CURLcode res = curl_easy_perform(curl[thread_id]);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            continue;
        }

        // Get and print the HTTP response code
        long response_code;
        curl_easy_getinfo(curl[thread_id], CURLINFO_RESPONSE_CODE, &response_code);
        // if (response_code != 404) {
        //    std::cout << "[" << thread_id << "] " << response_code << " " << url << std::endl;
        // }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <num_threads>" << std::endl;
        return 1;
    }

    int num_threads = std::stoi(argv[1]);

    // Initialize libcurl
    CURL* curl[num_threads];

    for (int i = 0; i < num_threads; i++) {
        curl[i] = curl_easy_init();
        if (!curl[i]) {
            std::cerr << "Failed to initialize libcurl" << std::endl;
            return 1;
        }
        // Disable body data retrieval to get only headers
        curl_easy_setopt(curl[i], CURLOPT_NOBODY, 1L);
    }

    auto start = std::chrono::steady_clock::now();
    d_buster(curl, "https://flexstudent.nu.edu.pk", num_threads);
    auto end = std::chrono::steady_clock::now();

    // Clean up libcurl
    for (int i = 0; i < num_threads; i++) {
        curl_easy_cleanup(curl[i]);
    }

    //std::cout << "Time: " << std::chrono::duration<double>(end - start).count() << " seconds" << std::endl;
	std::cout << std::chrono::duration<double>(end - start).count() << std::endl;

    return 0;
}

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <mpi.h>
#include <curl/curl.h>
#include <chrono>
#include "LoadWordlist.h"


#define MAX_STRING_LENGTH 256


void d_buster(CURL* curl, const std::string& base_url, int rank, int size, int num_words) {

    std::vector<MyStruct> send_strings;
    int num_strings_recv = 0;
    if (rank == 0) {
        send_strings = LoadWordlistStruct("wordlist.txt", num_words);
        num_strings_recv = send_strings.size() / size;
    }

    MPI_Bcast(&num_strings_recv, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<MyStruct> recv_strings(num_strings_recv);
    MPI_Scatter(send_strings.data(), num_strings_recv * sizeof(MyStruct), MPI_CHAR, recv_strings.data(), num_strings_recv * sizeof(MyStruct), MPI_CHAR, 0, MPI_COMM_WORLD);

    for (int i = 0; i < num_strings_recv; i++) {
        std::ostringstream oss;
        oss << recv_strings[i] << std::endl;

        // Escape special characters in the URL
        char* escaped_str = curl_easy_escape(curl, oss.str().c_str(), 0);
        std::string url = base_url + "/" + escaped_str;

        curl_free(escaped_str); // Free the memory allocated by curl_easy_escape

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            continue;
        }

        long response_code = 0.0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        // if (response_code != 404) {
        // 	 std::cout << "[" << rank << "] " << response_code << " " << url << std::endl;
        // }
    }
}


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <num_words>" << std::endl;
        return 1;
    }

    const int num_words = std::stoi(argv[1]);
    const std::string base_url = "https://flexstudent.nu.edu.pk";

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize libcurl" << std::endl;
        return 1;
    }
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    auto start = std::chrono::steady_clock::now();
    d_buster(curl, base_url, rank, size, num_words);    
    auto end = std::chrono::steady_clock::now();

    if (rank == 0)
        //std::cout << "Time: " << std::chrono::duration<double>(end - start).count() << std::endl;
        std::cout << std::chrono::duration<double>(end - start).count() << std::endl;

    curl_easy_cleanup(curl);
    MPI_Finalize();

    return 0;
}

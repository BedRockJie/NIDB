#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <cstring>

const uint8_t lookup_table[256] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

std::mutex file_mutex;

void process_file_chunk(const std::string& inputFilePath, const std::string& outputFilePath, size_t start, size_t end, bool is_reverse) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath, std::ios::binary | std::ios::app);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return;
    }

    inputFile.seekg(start);
    std::vector<uint8_t> buffer(end - start);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    if (is_reverse) {
        for (auto& byte : buffer) {
            byte = lookup_table[byte];
        }
    }

    {
        std::lock_guard<std::mutex> lock(file_mutex);
        outputFile.seekp(start);
        outputFile.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }
}

void worker_thread(std::queue<std::pair<size_t, size_t>>& chunks, std::mutex& queue_mutex, const std::string& inputFilePath, const std::string& outputFilePath, bool is_reverse) {
    while (true) {
        std::pair<size_t, size_t> chunk;
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (chunks.empty()) {
                break;
            }
            chunk = chunks.front();
            chunks.pop();
        }
        process_file_chunk(inputFilePath, outputFilePath, chunk.first, chunk.second, is_reverse);
    }
}

void show_help() {
    std::cout << "Usage: nidb [OPTIONS] input_file -o output_file\n"
              << "Options:\n"
              << "  -r2b      Convert RPD to BIN file with byte reverse\n"
              << "  -b2r      Convert BIN to RPD file\n"
              << "  --help    Show this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        show_help();
        return 1;
    }

    std::string mode = argv[1];
    std::string inputFilePath = argv[2];
    std::string outputFilePath;
    bool is_reverse = false;

    for (int i = 3; i < argc; ++i) {
        if (std::strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            outputFilePath = argv[++i];
        } else if (std::strcmp(argv[i], "-r2b") == 0) {
            is_reverse = true;
        } else if (std::strcmp(argv[i], "-b2r") == 0) {
            is_reverse = false;
        } else if (std::strcmp(argv[i], "--help") == 0) {
            show_help();
            return 0;
        }
    }

    if (outputFilePath.empty()) {
        std::cerr << "Output file not specified.\n";
        show_help();
        return 1;
    }

    // 在每次运行程序时覆盖输出文件
    std::ofstream clearFile(outputFilePath, std::ios::binary | std::ios::trunc);
    if (!clearFile.is_open()) {
        std::cerr << "Error opening output file for writing.\n";
        return 1;
    }
    clearFile.close();

    std::ifstream inputFile(inputFilePath, std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file.\n";
        return 1;
    }

    size_t fileSize = inputFile.tellg();
    inputFile.close();

    const size_t chunkSize = 1024 * 1024; // 1 MB
    size_t numChunks = (fileSize + chunkSize - 1) / chunkSize;

    std::queue<std::pair<size_t, size_t>> chunks;
    for (size_t i = 0; i < numChunks; ++i) {
        size_t start = i * chunkSize;
        size_t end = std::min(start + chunkSize, fileSize);
        chunks.emplace(start, end);
    }

    std::mutex queue_mutex;
    size_t maxThreads = std::min(static_cast<size_t>(8), numChunks); // 最大线程数为 8 或者 chunks 数量

    std::vector<std::thread> threads;
    for (size_t i = 0; i < maxThreads; ++i) {
        threads.emplace_back(worker_thread, std::ref(chunks), std::ref(queue_mutex), std::ref(inputFilePath), std::ref(outputFilePath), is_reverse);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Processing complete.\n";
    return 0;
}

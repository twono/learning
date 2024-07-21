#include <cassert>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
using namespace std;

constexpr int MEMORY_SIZE{1024};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "<memory_mapped_file> <offset>\n";
        return 1;
    }

    int descriptor{open("memory_mapped_file.cpp", O_RDONLY)};
    if (descriptor == -1)
    {
        perror("open");
        return 1;
    }

    struct stat file_stats;
    if (stat("memory_mapped_file.cpp", &file_stats) == -1)
    {
        perror("stat");
        return 1;
    }
    cout << "File size: " << file_stats.st_size << '\n';
    
    off_t offset{stoi(argv[1])};
    assert(offset >= 0 && offset < file_stats.st_size);

    char* data = static_cast<char*>(mmap(nullptr, file_stats.st_size, PROT_READ, MAP_SHARED, descriptor, 0));
    if (data == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    cout << "At offset " << offset << " is: " << data[offset] << '\n';

    return 0;
}
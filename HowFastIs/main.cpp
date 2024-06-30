#include <iostream>
#include <chrono>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <algorithm>

void printHelp()
{
    std::cout << "\nHowFastIs 1.0 (x64) : (c) TechWhizKid - All rights reserved." << std::endl;
    std::cout << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"" << std::endl
              << std::endl;
    std::cout << "Usage: HowFastIS [--file/-f <filename>] [--nobanner/-n] [--help/-h]\n" << std::endl;
    std::cout << "Options:\n" << std::endl;
    std::cout << "  -n, --nobanner           Suppresses the banner." << std::endl;
    std::cout << "  -f, --file <file_name>   Specify the file to execute." << std::endl;
    std::cout << "  -h, --help               Display this help menu." << std::endl;
}

int main(int argc, char *argv[])
{
    // Check if the -h or --help flag is passed
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help")
        {
            printHelp();
            return 0;
        }
    }

    // Check if the -n or --nobanner flag is passed
    bool showBanner = true;
    std::string file_name;
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-n" || std::string(argv[i]) == "--nobanner")
        {
            showBanner = false;
        }
        else if (std::string(argv[i]) == "-f" || std::string(argv[i]) == "--file")
        {
            if (i + 1 < argc)
            {
                file_name = argv[i + 1];
            }
            else
            {
                std::cerr << "Error: No file name provided after -f/--file flag." << std::endl;
                return 1;
            }
        }
    }

    // Display the banner if showBanner is true
    if (showBanner)
    {
        std::cout << "\nHowFastIs 1.0 (x64) : (c) TechWhizKid - All rights reserved." << std::endl;
        std::cout << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"" << std::endl;
    }

    // Check if the file name is provided
    if (file_name.empty())
    {
        std::cerr << "\nUsage: HowFastIs [--file/-f <filename>] [--nobanner/-n] [--help/-h]" << std::endl;
        return 1;
    }

    // Check if the file exists
    if (!std::filesystem::exists(file_name))
    {
        std::cerr << "File not found: " << file_name << std::endl;
        return 1;
    }

    // Measure the execution time
    auto start = std::chrono::high_resolution_clock::now();
    int exit_code = std::system((file_name).c_str());
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the execution time in seconds with millisecond precision
    auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start).count() / 1000.0;

    // Print the results
    std::cout << "\nExecuted file: " << file_name << std::endl;
    std::cout << "Execution time: " << std::fixed << std::setprecision(3) << duration << " seconds" << std::endl;

    return exit_code;
}

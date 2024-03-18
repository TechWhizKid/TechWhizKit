#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Function to split a string by a delimiter into a vector of strings
std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to trim quotes from a string
std::string trimQuotes(std::string str)
{
    size_t start = str.find_first_of("\"");
    size_t end = str.find_last_of("\"");
    if (start != std::string::npos && end != std::string::npos)
    {
        str = str.substr(start + 1, end - start - 1);
    }
    return str;
}

// Function to execute the command and print the PID
void findPID(const std::string &filename)
{
    std::string command = "tasklist /FI \"IMAGENAME eq " + filename + "\" /FO CSV";
    std::string data;
    char buffer[128];
    FILE *pipe = _popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Failed to run command" << std::endl;
        return;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        data += buffer;
    }
    _pclose(pipe);

    // Parse the CSV data
    std::vector<std::string> lines = split(data, '\n');
    for (const auto &line : lines)
    {
        std::vector<std::string> columns = split(line, ',');
        if (columns.size() > 1 && line.find("Image Name") == std::string::npos)
        {
            // Print the table rows
            std::cout << std::left << std::setw(20) << trimQuotes(columns[0])
                      << std::setw(10) << trimQuotes(columns[1])
                      << std::setw(15) << trimQuotes(columns[4]) << std::endl;
        }
    }
}

// The main function to process command line arguments
int main(int argc, char *argv[])
{
    bool show_banner = true;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [<filename>...] [--help/-h] [--nobanner/-n]\n";
        return 1;
    }

    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if (arg == "-n" || arg == "--nobanner")
        {
            show_banner = false;
        }
        else if (arg == "--help" || arg == "-h")
        {
            std::cout << "\nPidof 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n"
                      << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n"
                      << "\nUsage: " << argv[0] << " [<filename>...] [--help/-h] [--nobanner/-n]\n"
                      << "\nOptions:\n"
                      << " -h, --help         Display this help menu.\n"
                      << " -n, --nobanner     Suppresses the banner.\n"
                      << "   <filename>       Specify the filename(s) of the process(es) to find the PID.\n"
                      << "                    Multiple filenames can be specified separated by space.\n";
            return 0;
        }
    }

    if (show_banner)
    {
        std::cout << "\nLnkScout 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n"
                  << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n";
    }

    // Print the table header once
    std::cout << "\n"
              << std::left << std::setw(20) << "Image Name"
              << std::setw(10) << "PID"
              << std::setw(15) << "Memory Usage" << std::endl;

    // Loop through each filename provided in the command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string filename(argv[i]);
        findPID(filename);
    }

    return 0;
}

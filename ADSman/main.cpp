#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

// Forward declarations
void DisplayHelp();
void FindADS(const std::string &filename);
void AddADS(const std::string &source, const std::string &target);
void RemoveADS(const std::string &adsName, const std::string &target);
void ExtractADS(const std::string &adsName, const std::string &source);

int main(int argc, char *argv[])
{
    std::string source, target, fileToView, adsToRemove, adsName;
    bool ShowBanner = true, addFlag = false, toFlag = false, fileFlag = false, removeFlag = false, extractFlag = false, helpFlag = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--nobanner" || arg == "-n")
        {
            ShowBanner = false;
        }
        else if ((arg == "--add" || arg == "-a") && i + 1 < argc)
        {
            addFlag = true;
            source = argv[++i];
        }
        else if ((arg == "--to" || arg == "-t") && i + 1 < argc)
        {
            toFlag = true;
            target = argv[++i];
        }
        else if ((arg == "--file" || arg == "-f") && i + 1 < argc)
        {
            fileFlag = true;
            fileToView = argv[++i];
        }
        else if ((arg == "--remove" || arg == "-rm") && i + 1 < argc)
        {
            removeFlag = true;
            adsToRemove = argv[++i];
        }
        else if ((arg == "--extract" || arg == "-e") && i + 1 < argc)
        {
            extractFlag = true;
            adsName = argv[++i];
        }
        else if ((arg == "--from-file" || arg == "-ff") && i + 1 < argc)
        {
            target, source = argv[++i];
        }
        else if (arg == "--help" || arg == "-h")
        {
            helpFlag = true;
        }
    }

    if (helpFlag)
    {
        std::cout << "\nADSman 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n"
                  << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n"
                  << "\nUsage: " << argv[0] << " [--file/-f <file>] [--add/-a <source> --to/-t <target>] [[--remove/-rm | --extract/-e <adsName>] --from-file/-ff <filename>] [--nobanner/-n] [--help/-h]\n"
                  << "\nOptions:\n"
                  << "  -h, --help               Display this help menu and exit.\n"
                  << "  -n, --nobanner           Suppresses the banner.\n"
                  << "  -a, --add <source>       Add an alternate data stream (ADS) from <source> file.\n"
                  << "  -t, --to <target>        Specify the <target> file to add the ADS to.\n"
                  << "  -f, --file <filename>    Find ADS within the specified <filename>.\n"
                  << "  -e, --extract <adsName>  Extract the specified ADS <adsName> from the <file>.\n"
                  << "  -rm, --remove <adsName>  Remove the specified ADS <adsName>.\n"
                  << "  -ff, --from-file <file>  Specify the <file> from which to remove the ADS.\n";
        return 0;
    }
    else if (ShowBanner)
    {
        std::cout << "\nADSman 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n"
                  << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n";
    }

    // Perform actions based on flags
    if (addFlag && toFlag)
    {
        AddADS(source, target);
    }
    else if (fileFlag)
    {
        FindADS(fileToView);
    }
    else if (removeFlag && !target.empty())
    {
        RemoveADS(adsToRemove, target);
    }
    else if (extractFlag && !source.empty())
    {
        ExtractADS(adsName, source);
    }
    else
    {
        std::cerr << "\nUsage: " << argv[0] << " [--file/-f <file>] [--add/- <source> --to/- <target>] [--remove/-rm <adsName> --from-file/-ff <file>] [--nobanner/-n] [--help/-h]"
                  << "\nUse --help or -h for usage information." << std::endl;
        return 1;
    }

    return 0;
}

// Function to find and list Alternate Data Streams of a file
void FindADS(const std::string &filename)
{
    // Vector to store the names of ADS found
    std::vector<std::string> adsNames;

    // Construct the search pattern
    std::string searchPattern = filename + ":$DATA";
    WIN32_FIND_STREAM_DATA streamData;
    HANDLE streamHandle = FindFirstStreamW(std::wstring(filename.begin(), filename.end()).c_str(), FindStreamInfoStandard, &streamData, 0);

    if (streamHandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::wstring ws(streamData.cStreamName);
            std::string streamName(ws.begin(), ws.end());
            // Skip the default data stream
            if (streamName != "::$DATA")
            {
                adsNames.push_back(streamName.substr(1, streamName.length() - 7)); // Remove ':' and '$DATA'
            }
        } while (FindNextStreamW(streamHandle, &streamData));

        FindClose(streamHandle);
    }

    // Output the results
    std::cout << "\nFile: " << filename << std::endl;
    if (!adsNames.empty())
    {
        std::cout << "ADS Found:" << std::endl;
        for (const auto &name : adsNames)
        {
            std::cout << "    " << name << std::endl;
        }
    }
    else
    {
        std::cout << "No ADS found." << std::endl;
    }
}

// Function to add an Alternate Data Stream to a file
void AddADS(const std::string &source, const std::string &target)
{
    std::string adsPath = target + ":" + source;
    std::ifstream srcFile(source, std::ios::binary);
    std::ofstream destFile(adsPath, std::ios::binary);

    if (!srcFile.is_open() || !destFile.is_open())
    {
        std::cerr << "\nError opening files." << std::endl;
        return;
    }

    destFile << srcFile.rdbuf();

    std::cout << "\nAdded ADS: " << source << " to " << target << std::endl;
}

// Function to remove an Alternate Data Stream from a file
void RemoveADS(const std::string &adsName, const std::string &target)
{
    // Convert std::string to wide string
    std::wstring wideAdsName = std::wstring(adsName.begin(), adsName.end());
    std::wstring wideTarget = std::wstring(target.begin(), target.end());

    // Construct the wide character ADS path
    std::wstring wideAdsPath = wideTarget + L":" + wideAdsName;

    // Use DeleteFileW for wide character support
    if (DeleteFileW(wideAdsPath.c_str()))
    {
        std::wcout << L"\nRemoved ADS: " << wideAdsName << L" from " << wideTarget << std::endl;
    }
    else
    {
        std::wcerr << L"\nFailed to remove ADS: " << wideAdsName << L" from " << wideTarget << std::endl;
    }
}

void ExtractADS(const std::string &adsName, const std::string &source)
{
    // Construct the ADS path
    std::string adsPath = source + ":" + adsName;

    // Open the ADS for reading
    std::ifstream adsFile(adsPath, std::ios::binary);
    if (!adsFile.is_open())
    {
        std::cerr << "\nError opening ADS for reading." << std::endl;
        return;
    }

    // Create a file to write the extracted data
    std::ofstream outputFile(adsName, std::ios::binary);
    if (!outputFile.is_open())
    {
        std::cerr << "\nError creating file for extracted data." << std::endl;
        return;
    }

    // Copy the data from the ADS to the new file
    outputFile << adsFile.rdbuf();

    std::cout << "\nExtracted ADS: " << adsName << " from " << source << std::endl;
}

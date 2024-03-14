#include <iostream>
#include <windows.h>
#include <shobjidl.h>
#include <filesystem>

using namespace std; // Use standard namespace to simplify code
namespace fs = std::filesystem; // Create a shorthand namespace for filesystem

// Function to check if a .lnk file is valid
bool is_lnk_file_valid(const fs::path &lnk_path)
{
    // Initialize COM library for use by the calling thread
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool isValid = false; // Flag to track link validity
    // Declare pointers for IShellLink and IPersistFile interfaces
    IShellLinkW *psl;
    IPersistFile *ppf;
    WCHAR szGotPath[MAX_PATH]; // Buffer to store retrieved path
    WIN32_FIND_DATAW wfd; // Structure to store file data

    // Check if COM library was initialized successfully
    if (SUCCEEDED(hr))
    {
        // Create a ShellLink object and retrieve its IShellLink interface
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, reinterpret_cast<void **>(&psl));
        if (SUCCEEDED(hr))
        {
            // Get the IPersistFile interface for the ShellLink object
            hr = psl->QueryInterface(IID_IPersistFile, reinterpret_cast<void **>(&ppf));
            if (SUCCEEDED(hr))
            {
                // Load the .lnk file
                hr = ppf->Load(lnk_path.c_str(), STGM_READ);
                if (SUCCEEDED(hr))
                {
                    // Resolve the link, including searching for it
                    hr = psl->Resolve(NULL, SLR_NO_UI);
                    if (SUCCEEDED(hr))
                    {
                        // Get the path to the link target
                        hr = psl->GetPath(szGotPath, MAX_PATH, &wfd, SLGP_SHORTPATH);
                        if (SUCCEEDED(hr))
                        {
                            // Check if the file exists at the path
                            isValid = fs::exists(szGotPath);
                        }
                    }
                }
                // Release the IPersistFile interface
                ppf->Release();
            }
            // Release the IShellLink interface
            psl->Release();
        }
        // Uninitialize the COM library
        CoUninitialize();
    }
    // Return the validity of the link file
    return isValid;
}

// Function to list and optionally remove invalid .lnk files
void list_lnk_files(const fs::path &path, bool show_valid, bool show_invalid, bool remove_invalid)
{
    // Iterate over files in the directory recursively
    for (auto it = fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied); it != fs::end(it);)
    {
        const auto &entry = *it;
        // Skip system directories
        if (entry.path().filename() == "System Volume Information" || entry.path().filename() == "$RECYCLE.BIN")
        {
            it.disable_recursion_pending();
        }
        else if (entry.is_regular_file() && entry.path().extension() == ".lnk")
        {
            // Check if the .lnk file is valid
            bool valid = is_lnk_file_valid(entry.path());
            // Display and/or remove files based on validity and user options
            if ((valid && show_valid) || (!valid && show_invalid))
            {
                std::cout << (valid ? "Valid" : "Invalid") << " - " << entry.path() << std::endl;

                if (!valid && remove_invalid)
                {
                    // Remove the invalid .lnk file
                    fs::remove(entry.path());
                }
            }
        }
        // Handle errors during directory traversal
        std::error_code ec;
        it.increment(ec);
        if (ec)
        {
            std::cerr << "Error traversing directory: " << ec.message() << std::endl;
            break;
        }
    }
}

// Entry point of the program
int main(int argc, char *argv[])
{
    // Flags for command-line options
    bool show_banner = true;
    bool show_valid = true;
    bool show_invalid = true;
    bool remove_invalid = false;

    // Path to the directory to be scanned
    fs::path path;
    bool path_set = false; // Flag to check if path is set

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        // Handle option to suppress the banner
        if (arg == "-n" || arg == "--nobanner")
        {
            show_banner = false;
        }
        // Handle help option
        else if (arg == "-h" || arg == "--help")
        {
            // Display help message and exit
            std::cout << "\nLnkScout 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n"
                      << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n"
                      << "\nUsage: " << argv[0] << " <directory path> [--valid/-v | --invalid/-i [--remove/-r]] [--nobanner/-n]\n"
                      << "\nOptions:\n"
                      << " -n, --nobanner     Suppresses the banner.\n"
                      << " -v, --valid        Display only valid links.\n"
                      << " -i, --invalid      Display only invalid links.\n"
                      << " -r, --remove       Remove invalid links.\n"
                      << " -h, --help         Show this help message.\n";
            return 0;
        }
        // Handle options to display valid or invalid links
        else if (arg == "-v" || arg == "--valid")
        {
            show_invalid = false;
        }
        else if (arg == "-i" || arg == "--invalid")
        {
            show_valid = false;
        }
        // Handle option to remove invalid links
        else if (arg == "-r" || arg == "--remove")
        {
            remove_invalid = true;
        }
        // Set the path to the directory to be scanned
        else
        {
            if (!path_set)
            {
                path = arg;
                path_set = true;
            }
        }
    }

    // Validate the provided path
    if (!path_set || !fs::exists(path) || !fs::is_directory(path))
    {
        // Display error message and exit if path is invalid
        std::cerr << "\nError: Path does not exist, path is not a directory, or was not specified.\n"
                  << "Reminder: Make sure to not use backslash at the end of path. For instance, \"E:\\New Folder\\Example\\\".\n"
                  << "Use --help/-h to see the help menu.\n";
        return 1;
    }

    // Display the banner if not suppressed
    if (show_banner)
    {
        std::cout << "\nLnkScout 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n"
                  << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n";
    }
    // Start the scanning process
    std::cout << "\nLooking for shortcut files . . ." << std::endl;
    // Notify if invalid links will be removed
    if (remove_invalid)
    {
        std::cout << "Any invalid 'lnk' files will be remove . . ." << std::endl;
    }
    // Call the function to list and handle .lnk files
    list_lnk_files(path, show_valid, show_invalid, remove_invalid);
    return 0;
}

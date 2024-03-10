#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <process.h>

using namespace std;

// A function that executes a command and returns its output as a string
string exec(const char *cmd)
{
    char buffer[128];
    string result = "";
    FILE *pipe = _popen(cmd, "r"); // Create a pipe to run the command and read its output

    if (!pipe)
        throw runtime_error("_popen() failed!"); // Check if the pipe is valid
    try
    {
        // Read the output line by line and append it to the result string
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            result += buffer;
        }
    }
    catch (...)
    {
        // Close the pipe and rethrow the exception
        _pclose(pipe);
        throw;
    }
    // Close the pipe and return the result string
    _pclose(pipe);
    return result;
}

// A function that returns a vector of wifi network names
vector<string> getWifiList()
{
    // Execute the command to show the wifi profiles and store its output in a stringstream
    stringstream ss(exec("netsh wlan show profile").data());
    string input;
    vector<string> wifi;

    // Loop through the output line by line
    while (getline(ss, input))
    {
        // If the line contains the phrase "All User Profile", extract the wifi name and add it to the vector
        if (input.find("All User Profile") != string::npos)
        {
            wifi.push_back(input.substr(27, input.length()));
        }
    }

    // Return the vector of wifi names
    return wifi;
}

// A function that returns the password of a given wifi network name
string getPassword(string ssid)
{
    // Construct the command to show the wifi profile with the key in clear text
    string command = "netsh wlan show profile \"" + ssid + "\" key=clear";

    // Execute the command and store its output in a stringstream
    stringstream ss(exec(command.data()).data());
    string input;

    // Loop through the output line by line
    while (getline(ss, input))
    {
        // If the line contains the phrase "Key Content", extract the password and return it
        if (input.find("Key Content") != string::npos)
        {
            return input.substr(29, input.length());
        }
    }

    // If no password is found, return a placeholder string
    return "< NULL >";
}

// The main function that prints the wifi network names and passwords
int main(int argc, char *argv[])
{
    // Initialize a flag to indicate whether to show the banner or not
    int show_banner = 1;

    // Loop through the command-line arguments
    for (int i = 1; i < argc; i++)
    {
        // If the argument is "--nobanner" or "-n", set the flag to 0
        if (string(argv[i]).compare("--nobanner") == 0 || string(argv[i]).compare("-n") == 0)
        {
            show_banner = 0;
        }

        // If the argument is "--help" or "-h", print the help menu and exit
        if (string(argv[i]).compare("--help") == 0 || string(argv[i]).compare("-h") == 0)
        {
            printf("\nWiFiRe 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n");
            printf("Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n");
            printf("\nUsage: %s [--nobanner/-n] [--help/-h]\n", argv[0]);
            printf("\nOptions:\n");
            printf("-n, --nobanner\t\tSuppresses the banner.\n");
            printf("-h, --help\t\tShows this help menu.\n");
            return 0;
        }
    }

    // If the flag is 1, print the banner
    if (show_banner)
    {
        printf("\nWiFiRe 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n");
        printf("Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n");
    }

    // Print a message indicating the start of the process
    cout << "\nGetting list of known wifi networks...\n\n";

    // Call the function to get the wifi names and store them in a vector
    vector<string> wifi = getWifiList();

    // Print the table header with fixed width and alignment
    cout << left << setw(30) << setfill(' ') << "SSID";
    cout << left << setw(30) << setfill(' ') << "Password" << "\n";

    // Print a horizontal line
    cout << string(60, '-') << "\n";

    // Loop through the wifi names
    for (string ssid : wifi)
    {
        // Print the ssid and password with fixed width and alignment
        cout << left << setw(30) << setfill(' ') << ssid;
        cout << left << setw(30) << setfill(' ') << getPassword(ssid) << "\n";
    }

    // Return 0 to indicate successful execution
    return 0;
}

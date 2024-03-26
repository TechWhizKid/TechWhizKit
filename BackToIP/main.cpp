#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class NetworkUtility
{
public:
    NetworkUtility() = default;
    ~NetworkUtility() { WSACleanup(); }

    bool Initialize()
    {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0)
        {
            std::cerr << "\nWSAStartup failed: " << result << '\n';
            return false;
        }
        return true;
    }

    std::vector<std::string> GetIPAddresses(const std::string &url)
    {
        struct addrinfo hints
        {
        }, *res = nullptr;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC; // Both IPv4 and IPv6
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int result = getaddrinfo(url.c_str(), nullptr, &hints, &res);
        if (result != 0)
        {
            std::cerr << "\ngetaddrinfo failed: " << result << '\n';
            return {};
        }

        std::vector<std::string> ipAddresses;
        for (auto ptr = res; ptr != nullptr; ptr = ptr->ai_next)
        {
            ipAddresses.push_back(IPToString(ptr));
        }

        freeaddrinfo(res);
        return ipAddresses;
    }

private:
    std::string IPToString(struct addrinfo *ptr)
    {
        char ipstr[INET6_ADDRSTRLEN];
        void *addr;
        std::string ipver;

        if (ptr->ai_family == AF_INET) // IPv4
        {
            auto *ipv4 = reinterpret_cast<struct sockaddr_in *>(ptr->ai_addr);
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else // IPv6
        {
            auto *ipv6 = reinterpret_cast<struct sockaddr_in6 *>(ptr->ai_addr);
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        inet_ntop(ptr->ai_family, addr, ipstr, sizeof(ipstr));
        return ipver + ": " + ipstr;
    }
};

int main(int argc, char *argv[])
{
    std::map<std::string, std::string> cmdArgs;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "-n") || (arg == "--nobanner") || (arg == "-h") || (arg == "--help"))
        {
            cmdArgs[arg] = "";
        }
        else if ((arg == "-u") || (arg == "--url"))
        {
            if (i + 1 < argc) // Make sure we aren't at the end of argv!
            {
                cmdArgs[arg] = argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
            }
            else
            {
                std::cerr << "\n--url/-u option requires one argument.\n";
                return 1;
            }
        }
    }

    if (cmdArgs.find("-h") != cmdArgs.end() || cmdArgs.find("--help") != cmdArgs.end())
    {
        std::cout << "\nBackToIP 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n"
                  << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n\n"
                  << "Usage: " << argv[0] << " -u/--url <url> [-n/--nobanner] [-h/--help]\n"
                  << "\nOptions:\n"
                  << " -h, --help         Display this help menu.\n"
                  << " -n, --nobanner     Suppresses the banner.\n"
                  << " -u, --url          Specify the URL to find IP addresses.\n"
                  << "\nURL format:\n"
                  << " subdomain.domain-name.top-level-domain\n"
                  << "\nExample:\n"
                  << " " << argv[0] << " -u www.wiki.com\n"
                  << " " << argv[0] << " -u github.com\n";
        return 0;
    }

    if (cmdArgs.find("-n") == cmdArgs.end() && cmdArgs.find("--nobanner") == cmdArgs.end())
    {
        std::cout << "\nBackToIP 1.0 (x64) : (c) TechWhizKid - All rights reserved.\n";
        std::cout << "Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n";
    }

    auto urlIt = cmdArgs.find("-u");
    if (urlIt == cmdArgs.end())
    {
        urlIt = cmdArgs.find("--url");
    }

    if (urlIt == cmdArgs.end())
    {
        std::cerr << "\nURL not specified. Use --url/-u to specify the URL.\n";
        return 1;
    }

    std::string url = urlIt->second;

    NetworkUtility netUtil;
    if (!netUtil.Initialize())
        return 1;

    auto ipAddresses = netUtil.GetIPAddresses(url);
    if (ipAddresses.empty())
        return 1;

    std::cout << "IP addresses for " << url << ":\n";
    for (const auto &ip : ipAddresses)
    {
        std::cout << "  " << ip << '\n';
    }

    return 0;
}

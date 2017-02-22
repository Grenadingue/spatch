#include "SshProxy.hpp"
#include <unistd.h>
#include <iostream>
#include <string>

const std::vector<std::string> SshProxy::_shellCommands =
{
    "list",
    "endpoint",
    "alias",
    "exit",
};

SshProxy::SshProxy(const AccessListController &acl)
    : _acl(acl)
{
}

SshProxy::~SshProxy()
{
}

std::vector<std::string> SshProxy::tokenizeLine(const std::string &line)
{
    const std::string delimiter(" ");
    std::vector<std::string> tokens;
    std::string buffer = line;
    std::string token;
    size_t pos = 0;

    while ((pos = buffer.find(delimiter)) != std::string::npos)
    {
        token = buffer.substr(0, pos);
        if (token != "")
            tokens.push_back(token);
        buffer.erase(0, pos + delimiter.length());
    }
    if (buffer != "")
        tokens.push_back(buffer);
    return tokens;
}

void SshProxy::interactiveShell(const User &user, const char *command)
{
    std::string buffer;
    bool end = false;

    while (!end)
    {
        std::getline(std::cin, buffer);

        for (std::string token : tokenizeLine(buffer))
        {
            std::cout << "token = '" << token << "'" << std::endl;
        }

        if (buffer == "pass")
            end = true;

        buffer = "";
    }


    while (buffer != "hello")
    {
        std::cout << "say hello" << std::endl;
        std::getline(std::cin, buffer);
    }

    if (command)
        execl("/bin/sh", "sh", "-c", command, NULL);
    else
        execl("/bin/sh", "sh", "-l", NULL, NULL);
}

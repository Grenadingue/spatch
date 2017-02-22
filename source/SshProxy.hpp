#ifndef SSH_PROXY_HPP
# define SSH_PROXY_HPP

# include <vector>
# include <string>
# include "AccessListController.hpp"
// # include "User.hpp"

class SshProxy
{
private:
    static const std::vector<std::string> _shellCommands;

    const AccessListController &_acl;

public:
    SshProxy(const AccessListController &acl);
    ~SshProxy();

    std::vector<std::string> tokenizeLine(const std::string &line);
    void interactiveShell(const User &user, const char *command);
};

#endif // !SSH_PROXY_HPP

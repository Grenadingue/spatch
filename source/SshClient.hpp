#ifndef SSH_CLIENT_HPP
# define SSH_CLIENT_HPP

# include <string>
# include "Endpoint.hpp"

class SshClient
{
private:
    const Endpoint &_endpoint;
    const std::string &_username;
    const char *_command;

public:
    SshClient(const Endpoint &endpoint, const std::string &username, const char *command);
    ~SshClient();

    void connect();
};

#endif // !SSH_CLIENT_HPP

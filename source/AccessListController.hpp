#ifndef		ACCESSLISTCONTROLLER_HPP_
# define	ACCESSLISTCONTROLLER_HPP_

#include <string>
#include <vector>
#include <map>

class User;
class Endpoint;

class User
{
public:
    User(std::string _name, std::string _password, std::vector<Endpoint *> _availableEndpoints)
      : name(_name), password(_password), availableEndpoints(_availableEndpoints)
    {
    }

    ~User() {}

    const std::string name;
    const std::string password;
    const std::vector<Endpoint *> availableEndpoints;
};

class Endpoint
{
public:
    Endpoint(std::string _name, std::string _ipAdress, unsigned short _port, std::map<User *, std::vector<std::string>> _usersAccessControl)
        : name(_name), ipAdress(_ipAdress), port(_port), usersAccessControl(_usersAccessControl)
    {
    }

    ~Endpoint() {}

    const std::string name;
    const std::string ipAdress;
    const unsigned short port;
    const std::map<User *, std::vector<std::string>> usersAccessControl;
};

class AccessListController
{
private:
  User *_user = new User("foo", "bar", std::vector<Endpoint *>({ NULL, NULL }));

public:
  AccessListController(void *config)
  {
  }

  ~AccessListController()
  {
  }

  const User *authenticateLocalUser(const std::string &user, const std::string &password) const
  {
    if (user == "foo" && password == "bar")
      return _user;
    return NULL;
  }

  const std::vector<Endpoint *> getAvailableEndpointsForUser(const User &user) const
  {
    return std::vector<Endpoint *>();
  }

  const std::vector<std::string> getAvailableRemoteUsernamesForUserAtEndpoint(const User &user, const Endpoint &endpoint) const
  {
    return std::vector<std::string>();
  }
};

#endif		/* !ACCESSLISTCONTROLLER_HPP_ */

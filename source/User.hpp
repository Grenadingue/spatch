#ifndef		USER_HPP_
# define	USER_HPP_

#include <string>
#include <vector>

class Endpoint;

class User
{

public:
  User(std::string _name, std::string _password, std::vector<Endpoint *> _availableEndpoints);
  ~User();
  const std::string name;
  const std::string password;
  const std::vector<Endpoint *> availableEndpoints;
};

#endif		/* !USER_HPP_ */

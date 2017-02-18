#ifndef		USER_HPP_
# define	USER_HPP_

#include <string>
#include <vector>

class Endpoint;

class User
{

public:
  User();
  User(const User&);
  ~User();
  User &operator=(const User&);
  const std::string name;
  const std::string password;
  const std::vector<Endpoint *> availableEndpoints;
};

#endif		/* !USER_HPP_ */

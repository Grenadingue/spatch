#ifndef		ENDPOINT_HPP_
# define	ENDPOINT_HPP_

#include <string>
#include <map>
#include <vector>

class User;

class Endpoint
{

public:
  Endpoint(std::string _name, std::string _ipAdress, unsigned short _port, std::map<User *, std::vector<std::string>> _usersAccessControl);
  ~Endpoint();
  const std::string name;
  const std::string ipAdress;
  const unsigned short port;
  const std::map<User *, std::vector<std::string>> usersAccessControl;
};

#endif		/* !ENDPOINT_HPP_ */

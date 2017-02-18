#ifndef		ENDPOINT_HPP_
# define	ENDPOINT_HPP_

#include <string>
#include <map>
#include <vector>

class User;

class Endpoint
{

public:
  Endpoint();
  Endpoint(const Endpoint&);
  ~Endpoint();
  Endpoint &operator=(const Endpoint&);
  const std::string ipAdress;
  const unsigned short port;
  const std::map<User *, std::vector<std::string>> usersAccessControl;
};

#endif		/* !ENDPOINT_HPP_ */

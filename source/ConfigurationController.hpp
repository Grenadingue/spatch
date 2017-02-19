#ifndef		CONFIGURATIONCONTROLLER_HPP_
# define	CONFIGURATIONCONTROLLER_HPP_

#include <vector>

class User;
class Endpoint;

class ConfigurationController
{

private:
  std::vector<User *> _users;
  std::vector<Endpoint *> _endpoints;
public:
  ConfigurationController(std::vector<User *> _users, std::vector<Endpoint *> _endpoints);
  ~ConfigurationController();
  const std::vector<User *> &users();
  const std::vector<Endpoint *> &endpoints();
};

#endif		/* !CONFIGURATIONCONTROLLER_HPP_ */

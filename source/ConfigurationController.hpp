#ifndef		CONFIGURATIONCONTROLLER_HPP_
# define	CONFIGURATIONCONTROLLER_HPP_

#include <vector>
#include <iostream>
#include <map>

class User;
class Endpoint;
class ServerConfiguration;

class ConfigurationController
{

private:
  std::vector<User *> _users;
  std::vector<Endpoint *> _endpoints;
  ServerConfiguration * _server;
  std::vector <std::pair <std::string, std::string>> getKeysAndValues(std::vector <std::pair <std::string, std::string>> pairs);
  unsigned short getServerPort(std::vector <std::pair <std::string, std::string>> pairs);
  std::vector<User *> getLocalUsers(std::vector <std::pair <std::string, std::string>> pairs);
  std::vector<Endpoint *> getEndpoints(std::vector <std::pair <std::string, std::string>> pairs);
  void exitError(std::string section);
public:
  ConfigurationController(std::vector<User *> _users, std::vector<Endpoint *> _endpoints);
  ~ConfigurationController();
  const std::vector<User *> &users();
  const std::vector<Endpoint *> &endpoints();
  const ServerConfiguration &server();
};

#endif		/* !CONFIGURATIONCONTROLLER_HPP_ */

#include "ConfigurationController.hpp"
#include "ConfigurationReader.hpp"
#include "Endpoint.hpp"
#include "ServerConfiguration.hpp"
#include "User.hpp"

ConfigurationController::ConfigurationController(std::vector<User *> users, std::vector<Endpoint *> endpoints)
  : _users(users), _endpoints(endpoints)
{
  	std::map <std::string, std::vector <std::pair <std::string, std::string>>> datas;
    std::vector <std::pair <std::string, std::string>> keysAndValues;

  	ConfigurationReader* foo2 = new ConfigurationReader("./config/spatch/config.ini");
  	datas = foo2->getDatas();

  	for (auto const &it : datas) {
      std::cout << '[' << it.first << ']' << std::endl;
      if (it.first == "server") {
        if (it.second.size() > 1) {
          this->exitError(it.first);
        }
        // Check is not long number
        this->_server = new ServerConfiguration(this->getServerPort(it.second));
        std::cout << "server port : " << this->_server->port << std::endl;
      } else if (it.first == "local_users") {
        this->_users = this->getLocalUsers(it.second);
        for (auto const &it2 : this->_users) {
          std::cout << "name : " << it2->name << " password : " << it2->password << std::endl;
        }
      } else if (it.first == "endpoints") {
        this->_endpoints = this->getEndpoints(it.second);
        for (auto const &it2 : this->_endpoints) {
          std::cout << "name : " << it2->name << " ipAdress : " << it2->ipAdress << " port : " << it2->port << std::endl;
        }
      } else if (it.first == "users_control") {
        this->getKeysAndValues(it.second);
      } else if (it.first == "remote_users") {
        this->getKeysAndValues(it.second);
      }
  	}
}

ConfigurationController::~ConfigurationController()
{
}

const std::vector<User *> &users()
{
  // return (this->_users);
}

const std::vector<Endpoint *> &endpoints()
{
  // return (this->_endpoints);
}

const ServerConfiguration * &server()
{
  // return (this->_server);
}

std::vector <std::pair <std::string, std::string>> ConfigurationController::getKeysAndValues(std::vector <std::pair <std::string, std::string>> pairs)
{
  std::vector <std::pair <std::string, std::string>> keysAndValues;

  keysAndValues = {};
  for (auto const &it : pairs) {
    keysAndValues.push_back(std::pair <std::string, std::string>(it.first, it.second));
  }
  return (keysAndValues);
}

unsigned short ConfigurationController::getServerPort(std::vector <std::pair <std::string, std::string>> pairs)
{
  return (strtoul((this->getKeysAndValues(pairs).front().second).c_str(), NULL, 0));
}

std::vector<User *> ConfigurationController::getLocalUsers(std::vector <std::pair <std::string, std::string>> pairs)
{
  std::vector<User *> users;

  for (auto const &it : this->getKeysAndValues(pairs)) {
    users.push_back(new User(it.first, it.second, {}));
  }
  return (users);
}

std::vector<Endpoint *> ConfigurationController::getEndpoints(std::vector <std::pair <std::string, std::string>> pairs)
{
  std::vector<Endpoint *> endpoints;
  std::string ipAdress;
  unsigned short port;

  for (auto const &it : this->getKeysAndValues(pairs)) {
    ipAdress = it.second.substr(0, (it.second.find('|')));
    port = (unsigned short)strtoul(it.second.substr((it.second.find('|') + 1), (it.second.length() - 1)).c_str(), NULL, 0);
    endpoints.push_back(new Endpoint(it.first, ipAdress, port, {}));
  }
  return (endpoints);
}

void ConfigurationController::exitError(std::string section)
{
  std::cerr << "Can't read spatch config file correctly, please check [" << section << "] configuration." << std::endl;
  std::exit(-1);
}

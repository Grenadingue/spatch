#include "ConfigurationController.hpp"

ConfigurationController::ConfigurationController(std::vector<User *> users, std::vector<Endpoint *> endpoints)
  : _users(users), _endpoints(endpoints)
{
}

ConfigurationController::~ConfigurationController()
{
}

const std::vector<User *> &users()
{
}

const std::vector<Endpoint *> &endpoints()
{
}

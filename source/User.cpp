#include "User.hpp"

User::User(std::string _name, std::string _password, std::vector<Endpoint *> _availableEndpoints)
  : name(_name), password(_password), availableEndpoints(_availableEndpoints)
{
}

User::~User()
{
}

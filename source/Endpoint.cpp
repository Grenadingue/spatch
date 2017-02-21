#include "Endpoint.hpp"

Endpoint::Endpoint(std::string _name, std::string _ipAdress, unsigned short _port, std::map<User *, std::vector<std::string>> _usersAccessControl)
  : name(_name), ipAdress(_ipAdress), port(_port), usersAccessControl(_usersAccessControl)
{
}

Endpoint::~Endpoint()
{
}

#include "Endpoint.hpp"

Endpoint::Endpoint(std::string _ipAdress, unsigned short _port, std::map<User *, std::vector<std::string>> _usersAccessControl)
  : ipAdress(_ipAdress), port(_port), usersAccessControl(_usersAccessControl)
{
}

Endpoint::~Endpoint()
{
}

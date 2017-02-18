#include "ServerConfiguration.hpp"

ServerConfiguration::ServerConfiguration():port(0)
{
}

ServerConfiguration::ServerConfiguration(const ServerConfiguration &other):port(0)
{
  (void)other;
}

ServerConfiguration::~ServerConfiguration()
{
}

ServerConfiguration &ServerConfiguration::operator=(const ServerConfiguration &other)
{
  (void)other;
  return (*this);
}

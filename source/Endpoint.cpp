#include "Endpoint.hpp"

Endpoint::Endpoint():port(0)
{
}

Endpoint::Endpoint(const Endpoint &other):port(0)
{
  (void)other;
}

Endpoint::~Endpoint()
{
}

Endpoint &Endpoint::operator=(const Endpoint &other)
{
  (void)other;
  return (*this);
}

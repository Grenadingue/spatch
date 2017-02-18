#include "User.hpp"

User::User()
{
}

User::User(const User &other)
{
  (void)other;
}

User::~User()
{
}

User &User::operator=(const User &other)
{
  (void)other;
  return (*this);
}

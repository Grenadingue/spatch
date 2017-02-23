#include "ConfigurationController.hpp"
#include "ServerController.hpp"

int main(int ac, char **av)
{
  ConfigurationController config;
  ServerController server(&config);

  return server.run();
}

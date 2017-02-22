#include "ServerController.hpp"

int main(int ac, char **av)
{
  void *config = NULL;
  ServerController server(config);

  return server.run();
}

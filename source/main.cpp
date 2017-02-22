#include "SshSession.hpp"

int main(int ac, char **av)
{
  SshSession session("localhost", "42010");

  session.init();
  session.handleNewSessions();
  return 0;
}

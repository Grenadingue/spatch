#include "ServerController.hpp"

ServerController::ServerController(void *config)
    : _config(config), _acl(_config), _proxy(_acl), _session(_acl, _proxy, "localhost", "42010")
{
}

ServerController::~ServerController()
{
}

int ServerController::run()
{
    return _session.init() != 0 || _session.handleNewSessions() != 0;
}

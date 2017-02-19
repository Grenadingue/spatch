#include "AccessListController.hpp"

AccessListController::AccessListController()
{
}

AccessListController::~AccessListController()
{
}

const User *authenticateLocalUser(const std::string &user, const std::string &password)
{
}

const std::vector<Endpoint *> &getAvailableEndpointsForUser(const User &)
{
}

const std::vector<std::string> &getAvailableRemoteUsernamesForUserAtEndpoint(const User &, const Endpoint &)
{
}

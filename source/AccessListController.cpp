#include "AccessListController.hpp"
#include "ConfigurationController.hpp"
#include "User.hpp"
#include "Endpoint.hpp"

AccessListController::AccessListController(ConfigurationController *configurationController)
    :_configurationController(configurationController)
{
}

AccessListController::~AccessListController()
{
}

const User *AccessListController::authenticateLocalUser(const std::string &user, const std::string &password)
{
    for (auto const &it : this->_configurationController->users()) {
        if (it->name == user && it->password == password) {
            return (it);
        }
    }
    return (NULL);
}

const std::vector<Endpoint *> AccessListController::getAvailableEndpointsForUser(const User &user)
{
    for (auto const &it : this->_configurationController->users()) {
        if (it->name == user.name && it->password == user.password) {
            return (it->availableEndpoints);
        }
    }
    return (std::vector<Endpoint *>());
}

const std::vector<std::string> AccessListController::getAvailableRemoteUsernamesForUserAtEndpoint(User &user, const Endpoint &endpoint)
{
    for (auto const &it : this->_configurationController->endpoints()) {
        if (it->name == endpoint.name) {
            if (it->usersAccessControl.find(&user) != it->usersAccessControl.end()) {
               return (it->usersAccessControl[&user]);
           }
        }
    }
    return (std::vector<std::string>());
}

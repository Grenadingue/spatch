#ifndef		ACCESSLISTCONTROLLER_HPP_
# define	ACCESSLISTCONTROLLER_HPP_

#include <string>
#include <vector>

class User;
class Endpoint;

class AccessListController
{

public:
  AccessListController();
  AccessListController(const AccessListController&);
  ~AccessListController();
  AccessListController &operator=(const AccessListController&);
  const User *authenticateLocalUser(const std::string &user, const std::string &password);
  const std::vector<Endpoint *> &getAvailableEndpointsForUser(const User &);
  const std::vector<std::string> &getAvailableRemoteUsernamesForUserAtEndpoint(const User &, const Endpoint &);
};

#endif		/* !ACCESSLISTCONTROLLER_HPP_ */

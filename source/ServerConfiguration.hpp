#ifndef		SERVERCONFIGURATION_HPP_
# define	SERVERCONFIGURATION_HPP_

class ServerConfiguration
{

public:
  ServerConfiguration();
  ServerConfiguration(const ServerConfiguration&);
  ~ServerConfiguration();
  ServerConfiguration &operator=(const ServerConfiguration&);
  const unsigned short port;
};

#endif		/* !SERVERCONFIGURATION_HPP_ */

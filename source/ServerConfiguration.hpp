#ifndef		SERVERCONFIGURATION_HPP_
# define	SERVERCONFIGURATION_HPP_

class ServerConfiguration
{

public:
  ServerConfiguration(unsigned short _port);
  ~ServerConfiguration();
  const unsigned short port;
};

#endif		/* !SERVERCONFIGURATION_HPP_ */

#ifndef		CONFIGURATIONREADER_HPP_
# define	CONFIGURATIONREADER_HPP_

#include <string>

class ConfigurationReader
{

public:
  ConfigurationReader(std::string _configurationFilePath);
  ~ConfigurationReader();
  const std::string configurationFilePath;
};

#endif		/* !CONFIGURATIONREADER_HPP_ */

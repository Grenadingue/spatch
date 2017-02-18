#ifndef		CONFIGURATIONREADER_HPP_
# define	CONFIGURATIONREADER_HPP_

#include <string>

class ConfigurationReader
{

public:
  ConfigurationReader();
  ConfigurationReader(const ConfigurationReader&);
  ~ConfigurationReader();
  ConfigurationReader &operator=(const ConfigurationReader&);
  const std::string configurationFilePath;
};

#endif		/* !CONFIGURATIONREADER_HPP_ */

#ifndef		CONFIGURATIONREADER_HPP_
# define	CONFIGURATIONREADER_HPP_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ConfigurationReader
{

private:
  std::ifstream _infile;

public:
  ConfigurationReader(std::string _configurationFilePath);
  ~ConfigurationReader();
  getDatas();
};

#endif		/* !CONFIGURATIONREADER_HPP_ */

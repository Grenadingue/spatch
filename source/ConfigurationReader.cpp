#include "ConfigurationReader.hpp"

ConfigurationReader::ConfigurationReader(std::string configurationFilePath)
  : _infile(configurationFilePath.c_str())
{
  if (!this->_infile) {
    std::cerr << "Can't open file " << configurationFilePath << std::endl;
    std::exit(-1);
  }
}

ConfigurationReader::~ConfigurationReader()
{
  this->_infile.close();
}

ConfigurationReader::getDatas()
{
  std::string line;

  while (std::getline(this->_infile, line))
  {
    std::cout << line << std::endl;
    // std::istringstream iss(line);
    // std::cerr << "Couldn't open " << filename << " for reading\n";
    // int a, b;
    // if (!(iss >> a >> b)) { break; } // error
  }
}

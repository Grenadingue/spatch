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
  std::map <std::string, std::vector <std::pair <std::string, std::string>>> datas;
  std::vector <std::pair <std::string, std::string>> line_datas;
  std::vector <std::pair <std::string, std::string>>::iterator line_datas_it;
  std::string line;
  std::string section;

  while (std::getline(this->_infile, line)) {
    line = this->epurLine(line);
    if (line[0] == '[') {
      section = line.substr(1, (line.find(']') - 1));
      datas[section] = {
        { "", "" }
      };
      std::cout << '[' << section << ']' << std::endl;
    } else if (line.length() > 1 && line != "" && line.find('=') != -1) {
      if (line.find('=') > 1 && (line.length() - 1) - (line.find('=') + 1) > 1) {
        line_datas = getLineDatas(line);
      } else {
        std::cerr << "Can't read spatch config file correctly, please check [" << section << "] configuration." << std::endl;
        std::exit(-1);
      }
    }
  }
  std::map <std::string, std::vector <std::pair <std::string, std::string>>>::iterator it;
  for (it=datas.begin(); it!=datas.end(); ++it) {
    std::cout << it->first << '\n';
  }
}

std::string ConfigurationReader::epurLine(std::string line) {
  std::string epuredLine;

  for (int i = 0; line[i] && line[i] != '#' && line[i] != '\n' && line[i] != '\r'; i++) {
    if (line[i] != ' ') {
      epuredLine.push_back(line[i]);
    }
  }
  epuredLine.push_back('\0');
  return (epuredLine);
}

std::vector <std::pair <std::string, std::string>> ConfigurationReader::getLineDatas(std::string line) {
  std::string keys;
  std::string values;

  keys = line.substr(0, (line.find('=')));
  values = line.substr((line.find('=') + 1), (line.length() - 1));
  std::cout << "keys : " << keys << " && values : " << values << std::endl;

  return {{keys, values}};
}

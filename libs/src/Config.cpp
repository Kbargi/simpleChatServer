#include "Config.h"

void Config::parse() throw() {
  std::ifstream file;
  std::string section = "[DEFAULT]";
  std::string line;
  size_t found = std::string::npos;
  unsigned int lineNr = 1;

  file.open(m_mainConf.c_str(), std::ios::in);
  try {
    if (!file.is_open())
      throw std::runtime_error(
          std::string("Cannot open config file: " + m_mainConf));

    while (getline(file, line)) {
      line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
      if (line.empty() || line[0] == '#') {
        ++lineNr;
        continue;
      }
      if (line[0] == '[') {
        found = line.find(']');
        if (found == std::string::npos || found + 1 != line.size() ||
            line.size() < 3) {
          throw std::runtime_error(
              std::string("Syntax error in configuration "
                          "\nline: ") +
              line + std::string("\nfile: ") + m_mainConf +
              std::string("\nline number: ") +
              boost::lexical_cast<std::string>(lineNr) +
              std::string(
                  "\n error description - section not properly closed\n"));
        } else {
          line.erase(line.begin());
          line.pop_back();
          section = line;
        }
        ++lineNr;
        continue;
      } else {
        found = line.find('=');
        if (found == std::string::npos) {
          throw std::runtime_error(
              std::string("Syntax error in configuration"
                          "\nline: ") +
              line + std::string("\nfile: ") + m_mainConf +
              std::string("\nline number: ") +
              boost::lexical_cast<std::string>(lineNr) +
              std::string("\nerror description - cannot find \'=\' sign\n"));
        }
        m_sections[section][line.substr(0, found)] = line.substr(found + 1);
        ++lineNr;
        continue;
      }
    }
    if (m_sections.empty()) {
      throw std::runtime_error(
          std::string("Configuration file is probably empty"));
    }
  } catch (std::runtime_error& e) {
    file.close();
    throw e;
  } catch (...) { /*catch to be sure that file was properly closed*/
    file.close();
    throw std::runtime_error(
        std::string("Unknown exception while parsing configuration file"));
  }
  file.close();
}
void Config::dump() {
  for (auto const& it : m_sections) {
    std::cout << "-----------------\n*NAGLOWEK: " << it.first << "\n";
    for (auto const& itp : it.second) {
      std::cout << "\t" << itp.first << " -> " << itp.second << "\n";
    }
  }
}

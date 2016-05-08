#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include <boost/lexical_cast.hpp>

class Config {
   typedef std::map<std::string, std::string> commandsMap;
   typedef std::map<std::string, commandsMap> sectionsMap;

   public:
      Config() = delete;
      Config(std::string&& mainConf) : m_mainConf(std::move(mainConf)) {}
      Config(std::string& mainConf) : m_mainConf(mainConf) {}
      ~Config(){}

      void parse() throw();

      void dump();

      template<class T>
      T get(const std::string& section, const std::string& command) throw() {
         return boost::lexical_cast<T>(m_sections.at(section).at(command));
      }
      template<class T>
      T get(const std::string& section, const std::string& command, const T& defValue) {
         try {
            std::string& t = m_sections.at(section).at(command);
            return boost::lexical_cast<T>(t);
         } catch(std::out_of_range& e) {
            return defValue;
         }
         catch(boost::bad_lexical_cast& e) {
      	   std::cout << e.what();
      	   return defValue;
         }
      }

   private:
      std::string m_mainConf;
      sectionsMap m_sections;
};

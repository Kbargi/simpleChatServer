#include <iostream>
#include "Listener.h"
#include "Config.h"

int main(int argc, char** argv) {
   if(argc != 2) {
      std::cout << "server [configName]\n";
      return 1;
   }
   try {
      Config conf(argv[1]);
      conf.parse();
      //conf.dump();

      Listener l(conf.get<std::string>("NETWORK", "PORT"), conf.get("PERFORMANCE", "THREADS", 4));
      l.init();
      l.run();

   } catch(std::out_of_range& e) {
      std::cout<< e.what();
   } catch(std::runtime_error& e) {
      std::cout<< e.what();
   } catch(std::logic_error& e) {
      std::cout<< e.what();
   } catch(...) {
      std::cout << "NIEZNANY WYJATEK\n";
   }
   return 0;
}

#include <iostream>
#include <unistd.h>
#include <boost/program_options.hpp>
#include "Listener.hpp"
#include "Singleton.hpp"
#include "Config.hpp"
#include <signal.h>

void signal_callback_handler(int signum) {
	std::cout << "Received signal: " << signum << "\n";
    Singleton<Listener>::getInstance().stop();
}

int main(int argc, char** argv) {
  try {
    boost::program_options::options_description description("Allowed options");
    description.add_options()("help", "produce help message")(
        "config", boost::program_options::value<std::string>(),
        "configuration file path")("demonize", "run server as a daemon");
    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, description),
        vm);
    boost::program_options::notify(vm);
    if (vm.count("config")) {
      // std::cout << "Configuration: " << vm["config"].as<std::string>() <<
      // "\n";
    } else {
      std::cout << description << "\n";
      return 1;
    }
    if (vm.count("demonize")) {
      if (daemon(1, 1)) {
        std::cout << "daemon failed\n";
        return 1;
      } else {
        std::cout << "demonized\n";
      }
    }
    struct sigaction sa;
        // Setup the sighub handler
        sa.sa_handler = signal_callback_handler;

        // Block every signal during the handler
        sigfillset(&sa.sa_mask);

        // Intercept SIGHUP and SIGINT
        if (sigaction(SIGHUP, &sa, NULL) == -1) {
            std::cout <<"sigaction error\n";
            exit(1);
        }
        if (sigaction(SIGINT, &sa, NULL) == -1) {
                    std::cout <<"sigaction error\n";
                    exit(1);
                }

    Config conf(vm["config"].as<std::string>());
    conf.parse();
    // conf.dump();

    Singleton<Listener>::getInstance().init(conf.get<std::string>("NETWORK", "PORT"),
            conf.get("PERFORMANCE", "THREADS", 4));

    Singleton<Listener>::getInstance().run();

  } catch (std::out_of_range& e) {
    std::cout << e.what();
  } catch (std::runtime_error& e) {
    std::cout << e.what();
  } catch (boost::program_options::error& e) {
    std::cout << e.what();
  } catch (std::logic_error& e) {
    std::cout << e.what();
  } catch (...) {
    std::cout << "Unknown exception\n";
  }
  return 0;
}

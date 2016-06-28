#include "App.hpp"

void App::init(int argc, char** argv) {
  boost::program_options::options_description optionsDescription(
      "Allowed options");
  optionsDescription.add_options()("help", "produce help message [optional]\n")(
      "config", boost::program_options::value<std::string>(),
      "configuration file path [required]")("demonize", "run server as a daemon [optional]\n")
	  ("version", "prints version [optional]\n");
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(
                                    argc, argv, optionsDescription),
                                vm);
  boost::program_options::notify(vm);
  if (vm.count("config")) {
    // std::cout << "Configuration: " << vm["config"].as<std::string>() <<
    // "\n";
  } else if (vm.count("version")){
    version();
    exit(0);
  } else {
    std::cout << optionsDescription;
    exit(1);
  }
  if (vm.count("demonize")) {
    demonize();
  }

  m_config = std::make_shared<Config>(vm["config"].as<std::string>());
  m_config->parse();

  m_listener = std::make_shared<Listener>();
  m_listener->init(m_config->get<std::string>("NETWORK", "PORT"),
                   m_config->get("PERFORMANCE", "THREADS", 4));
}

void App::run() { m_listener->run(); }

void App::demonize() {
  if (daemon(1, 1)) {
    std::cout << "daemon failed\n";
    exit(1);
  } else {
    std::cout << "demonized\n";
  }
}
void App::version() {
  std::cout << "simpleChatServer version: " << VERSION << "\n";
}
void App::handleSignal(int num) { m_listener->stop(); }

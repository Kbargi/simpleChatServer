#pragma once

#define VERSION "1.0.0"

#include <boost/program_options.hpp>
#include <signal.h>
#include "Listener.hpp"
#include "Singleton.hpp"
#include "Config.hpp"

class App : public Singleton<App> {
  friend class Singleton<App>;

 public:

  void init(int argc, char** argv);
  void run();

  void handleSignal(int);

 private:
  App() {}

  void demonize();
  void version();

  std::shared_ptr<Config> m_config;
  std::shared_ptr<Listener> m_listener;
};

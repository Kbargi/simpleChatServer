#include <App.hpp>

void signal_callback_handler(int signum) {
  std::cout << "Received signal: " << signum << "\n";
  App::getInstance().handleSignal(signum);
}

int main(int argc, char** argv) {
  struct sigaction sa;
  // Setup the sighub handler
  sa.sa_handler = signal_callback_handler;

  // Block every signal during the handler
  sigfillset(&sa.sa_mask);

  // Intercept SIGHUP and SIGINT
  if (sigaction(SIGHUP, &sa, NULL) == -1) {
    std::cout << "sigaction error\n";
    exit(1);
  }
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    std::cout << "sigaction error\n";
    exit(1);
  }
  try {
    App::getInstance().init(argc, argv);
    App::getInstance().run();

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

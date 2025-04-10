#ifndef SHARE_RUNTIME_DVFSTIMERTHREAD_HPP
#define SHARE_RUNTIME_DVFSTIMERTHREAD_HPP

#include "runtime/nonJavaThread.hpp"
#include "runtime/threadSMR.hpp"

class DVFSThread : public NonJavaThread {
private:
  static DVFSThread* _instance;
  static bool _should_terminate;
  static int _interval_ms;

public:
  static DVFSThread* instance() { return _instance; }
  static void start();
  static void stop();
  static bool is_running() { return _instance != nullptr; }
  static void set_build_phase(bool in_build);

  void run() override;
  void sleep() const;
  void execute_tasks();
};

#endif // SHARE_RUNTIME_DVFSTIMERTHREAD_HPP 
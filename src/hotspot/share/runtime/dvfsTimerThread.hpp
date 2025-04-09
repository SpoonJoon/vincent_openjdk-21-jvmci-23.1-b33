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
  static void start(int interval_ms = 1);
  static void stop();
  static bool is_running() { return _instance != nullptr; }

  void run() override;
  void sleep() const;
  void execute_tasks();
  void print() const override;
};

#endif // SHARE_RUNTIME_DVFSTIMERTHREAD_HPP 
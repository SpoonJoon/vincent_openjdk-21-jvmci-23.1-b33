#include "precompiled.hpp"
#include "runtime/dvfsTimerThread.hpp"
#include "runtime/javaThread.hpp"
#include "runtime/os.hpp"
#include "runtime/threadSMR.hpp"

DVFSThread* DVFSThread::_instance = nullptr;
bool DVFSThread::_should_terminate = false;
int DVFSThread::_interval_ms = 1;

void DVFSThread::start(int interval_ms) {
  if (_instance == nullptr) {
    _interval_ms = interval_ms;
    _instance = new DVFSThread();
    _instance->create_and_start();
  }
}

void DVFSThread::stop() {
  if (_instance != nullptr) {
    _should_terminate = true;
    _instance->join();
    delete _instance;
    _instance = nullptr;
  }
}

void DVFSThread::run() {
  while (!_should_terminate) {
    sleep();
    execute_tasks();
  }
}

void DVFSThread::sleep() const {
  os::naked_short_sleep(_interval_ms);
}

void DVFSThread::execute_tasks() {
  ThreadsListHandle tlh;
  for (JavaThreadIteratorWithHandle jtiwh; JavaThread* thread = jtiwh.next(); ) {
    if (thread->should_sample_dvfs()) {
      thread->increment_dvfs_timer();
    }
  }
} 
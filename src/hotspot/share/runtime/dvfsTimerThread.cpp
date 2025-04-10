#include "precompiled.hpp"
#include "runtime/dvfsTimerThread.hpp"
#include "runtime/javaThread.hpp"
#include "runtime/os.hpp"
#include "runtime/threadSMR.hpp"
#include "logging/log.hpp"

DVFSThread* DVFSThread::_instance = nullptr;
bool DVFSThread::_should_terminate = false;
int DVFSThread::_interval_ms = 8;

void DVFSThread::start() {
  if (_instance == nullptr) {
    _instance = new DVFSThread();
    if (os::create_thread(_instance, os::dvfs_thread)) {
      os::start_thread(_instance);
      log_info(dvfs)("DVFS thread started with interval %d ms", _interval_ms);
    }
  }
}

void DVFSThread::stop() {
  if (_instance != nullptr) {
    _should_terminate = true;
    delete _instance;
    _instance = nullptr;
    log_info(dvfs)("DVFS thread stopped");
  }
}

void DVFSThread::run() {
  log_info(dvfs)("DVFS thread running");
  while (!_should_terminate) {
    os::naked_short_sleep(_interval_ms);
    execute_tasks();
  }
}

void DVFSThread::sleep() const {
  os::naked_short_sleep(_interval_ms);
}

void DVFSThread::execute_tasks() {
  ThreadsListHandle tlh;
  int sampled_count = 0;
  for (JavaThreadIteratorWithHandle jtiwh; JavaThread* thread = jtiwh.next(); ) {
    if (thread->should_sample_dvfs()) {
      thread->increment_dvfs_timer();
      sampled_count++;
      log_info(dvfs)("Thread %p current timer value: %d", thread, thread->get_dvfs_timer());
    }
  }
  if (sampled_count > 0) {
    log_info(dvfs)("Sampled %d threads", sampled_count);
  }
} 
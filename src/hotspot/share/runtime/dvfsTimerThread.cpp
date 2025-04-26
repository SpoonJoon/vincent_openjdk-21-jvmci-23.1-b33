#include "precompiled.hpp"
#include "runtime/dvfsTimerThread.hpp"
#include "runtime/javaThread.hpp"
#include "runtime/os.hpp"
#include "runtime/threadSMR.hpp"
#include "utilities/ostream.hpp"

DVFSThread* DVFSThread::_instance = nullptr;
bool DVFSThread::_should_terminate = false;
int DVFSThread::_interval_ms = 8;
static bool _in_build_phase = false;

void DVFSThread::start() {
  if (_instance == nullptr) {
    _instance = new DVFSThread();
    if (os::create_thread(_instance, os::dvfs_thread)) {
      os::start_thread(_instance);
    }
  }
}

void DVFSThread::stop() {
  if (_instance != nullptr) {
    _should_terminate = true;
    delete _instance;
    _instance = nullptr;
  }
}

void DVFSThread::run() {
  while (!_should_terminate) {
    os::naked_short_sleep(_interval_ms);
    start_dvfs_timeslice();
  }
}

void DVFSThread::sleep() const {
  os::naked_short_sleep(_interval_ms);
}

void DVFSThread::start_dvfs_timeslice() {
  ThreadsListHandle tlh;
  for (JavaThreadIteratorWithHandle jtiwh; JavaThread* thread = jtiwh.next(); ) {
      thread->enable_dvfs();
    }
}


void DVFSThread::set_build_phase(bool in_build) {
  _in_build_phase = in_build;
} 
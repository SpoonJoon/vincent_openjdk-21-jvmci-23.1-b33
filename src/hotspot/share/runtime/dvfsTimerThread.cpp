#include "precompiled.hpp"
#include "runtime/dvfsTimerThread.hpp"
#include "runtime/javaThread.hpp"
#include "runtime/os.hpp"
#include "runtime/threadSMR.hpp"
#include "utilities/ostream.hpp"
#include "cds/archiveBuilder.hpp"

DVFSThread* DVFSThread::_instance = nullptr;
bool DVFSThread::_should_terminate = false;
int DVFSThread::_interval_ms = 8;
static bool _in_build_phase = false;

void DVFSThread::start() {
  // Don't start the DVFS thread if we're creating a CDS archive
  if (ArchiveBuilder::is_active()) {
    return;
  }

  if (_instance == nullptr) {
    _instance = new DVFSThread();
    if (os::create_thread(_instance, os::dvfs_thread)) {
      os::start_thread(_instance);
      tty->print_cr("DVFS thread started with interval %d ms", _interval_ms);
    }
  }
}

void DVFSThread::stop() {
  if (_instance != nullptr) {
    _should_terminate = true;
    delete _instance;
    _instance = nullptr;
    tty->print_cr("DVFS thread stopped");
  }
}

void DVFSThread::run() {
  tty->print_cr("DVFS thread running");
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
      tty->print_cr("Thread %p timer: %d", thread, thread->get_dvfs_timer());
    }
  }
  if (sampled_count > 0) {
    tty->print_cr("Sampled %d threads", sampled_count);
  }
}

void DVFSThread::set_build_phase(bool in_build) {
  _in_build_phase = in_build;
} 
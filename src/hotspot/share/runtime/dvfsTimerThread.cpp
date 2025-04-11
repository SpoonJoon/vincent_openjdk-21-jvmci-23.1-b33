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
    FILE* log_file = fopen("/tmp/dvfs_debug.log", "a");
    if (log_file != nullptr) {
      fprintf(log_file, "Creating new DVFS thread instance\n");
      fclose(log_file);
    }

    _instance = new DVFSThread();
    if (os::create_thread(_instance, os::dvfs_thread)) {
      os::start_thread(_instance);
      log_file = fopen("/tmp/dvfs_debug.log", "a");
      if (log_file != nullptr) {
        fprintf(log_file, "DVFS thread started successfully with interval %d ms\n", _interval_ms);
        fclose(log_file);
      }
    } else {
      log_file = fopen("/tmp/dvfs_debug.log", "a");
      if (log_file != nullptr) {
        fprintf(log_file, "Failed to create DVFS thread\n");
        fclose(log_file);
      }
    }
  } else {
    FILE* log_file = fopen("/tmp/dvfs_debug.log", "a");
    if (log_file != nullptr) {
      fprintf(log_file, "DVFS thread already exists, skipping start\n");
      fclose(log_file);
    }
  }
}

void DVFSThread::stop() {
  if (_instance != nullptr) {
    _should_terminate = true;
    delete _instance;
    _instance = nullptr;
    FILE* log_file = fopen("/tmp/dvfs_debug.log", "a");
    if (log_file != nullptr) {
      fprintf(log_file, "DVFS thread stopped\n");
      fclose(log_file);
    }
  }
}

void DVFSThread::run() {
  FILE* log_file = fopen("/tmp/dvfs_debug.log", "a");
  if (log_file != nullptr) {
    fprintf(log_file, "DVFS thread running\n");
    fclose(log_file);
  }

  while (!_should_terminate) {
    os::naked_short_sleep(_interval_ms);
    execute_tasks();
  }
}

void DVFSThread::sleep() const {
  os::naked_short_sleep(_interval_ms);
}

void DVFSThread::execute_tasks() {
  FILE* log_file = fopen("/tmp/dvfs_debug.log", "a");
  if (log_file != nullptr) {
    fprintf(log_file, "execute_tasks() called\n");
    fclose(log_file);
  }

  ThreadsListHandle tlh;
  int sampled_count = 0;
  int total_threads = 0;
  
  for (JavaThreadIteratorWithHandle jtiwh; JavaThread* thread = jtiwh.next(); ) {
    total_threads++;
    log_file = fopen("/tmp/dvfs_debug.log", "a");
    if (log_file != nullptr) {
      fprintf(log_file, "Thread %p state - timer: %d, skipCount: %d, sampleCount: %d\n", 
              thread,
              thread->get_dvfs_timer(),
              thread->get_dvfs_skip_count(),
              thread->get_dvfs_sample_count());
      fclose(log_file);
    }

    if (thread->should_sample_dvfs()) {
      thread->increment_dvfs_timer();
      sampled_count++;
      log_file = fopen("/tmp/dvfs_debug.log", "a");
      if (log_file != nullptr) {
        fprintf(log_file, "Thread %p sampled - timer: %d\n", 
                thread, 
                thread->get_dvfs_timer());
        fclose(log_file);
      }
    }
  }

  log_file = fopen("/tmp/dvfs_debug.log", "a");
  if (log_file != nullptr) {
    fprintf(log_file, "Total threads: %d, Sampled threads: %d\n", total_threads, sampled_count);
    fclose(log_file);
  }
}

void DVFSThread::set_build_phase(bool in_build) {
  _in_build_phase = in_build;
} 
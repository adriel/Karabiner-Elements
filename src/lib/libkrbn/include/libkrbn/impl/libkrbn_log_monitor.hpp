#pragma once

#include <pqrs/spdlog.hpp>

class libkrbn_log_lines_class final {
public:
  libkrbn_log_lines_class(std::shared_ptr<std::deque<std::string>> lines) : lines_(lines) {
  }

  std::shared_ptr<std::deque<std::string>> get_lines(void) const {
    return lines_;
  }

private:
  std::shared_ptr<std::deque<std::string>> lines_;
};

class libkrbn_log_monitor final {
public:
  libkrbn_log_monitor(const libkrbn_log_monitor&) = delete;

  libkrbn_log_monitor(libkrbn_log_monitor_callback callback,
                      void* refcon) {
    std::vector<std::string> targets = {
        "/var/log/karabiner/observer.log",
        "/var/log/karabiner/grabber.log",
    };
    auto log_directory = krbn::constants::get_user_log_directory();
    if (!log_directory.empty()) {
      targets.push_back(log_directory + "/console_user_server.log");
    }

    monitor_ = std::make_unique<pqrs::spdlog::monitor>(pqrs::dispatcher::extra::get_shared_dispatcher(),
                                                       targets,
                                                       250);

    monitor_->log_file_updated.connect([callback, refcon](auto&& lines) {
      if (callback) {
        auto log_lines = new libkrbn_log_lines_class(lines);
        callback(reinterpret_cast<libkrbn_log_lines*>(log_lines), refcon);
        delete log_lines;
      }
    });

    monitor_->async_start(std::chrono::milliseconds(1000));
  }

private:
  std::unique_ptr<pqrs::spdlog::monitor> monitor_;
};
#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

// modeled after Python's threading.Event class, this class is simply a
// convenient way to share a flag across threads.
class Event {
public:
  Event() : _flag{false} {}
  ~Event() = default;
  // we don't want copies
  Event(const Event &) = delete;
  Event &operator=(const Event &) = delete;
  // std::mutex can't be moved anyway
  Event(Event &&) = delete;
  Event &operator=(Event &&) = delete;

  bool isSet() const { return _flag; }
  void set() {
    std::lock_guard lock(_mtx);
    _flag = true;
    _cv.notify_all();
  }

  void clear() {
    std::lock_guard lock(_mtx);
    _flag = false;
  }

  bool wait() {
    std::unique_lock lock(_mtx);

    if (!_flag)
      _cv.wait(lock, [&]() { return _flag; });

    return _flag;
  }

private:
  bool _flag = false;
  std::mutex _mtx;
  std::condition_variable _cv;
};

// Go-style channel for inter thread communication
template <typename T> class Channel {
public:
  Channel(int size) : _size(size) {}

  // we don't want copies
  Channel(const Channel &) = delete;
  Channel &operator=(const Channel &) = delete;
  // std::mutex can't be moved anyway
  Channel(Channel &&) = delete;
  Channel &operator=(Channel &&) = delete;

  bool send(T &&data, std::chrono::milliseconds timeout =
                          std::chrono::milliseconds(60 * 30 * 1000)) {
    std::unique_lock lock(_mtx);

    if (_buf.size() == (unsigned)_size) {
      auto ret = _full_cond.wait_for(
          lock, timeout, [&]() { return _buf.size() < (unsigned)_size; });

      if (!ret)
        return false;
    }

    _buf.emplace_back(std::move(data));
    _empty_cond.notify_one();
    return true;
  }

  T receive(std::chrono::milliseconds timeout =
                std::chrono::milliseconds(60 * 30 * 1000)) {
    std::unique_lock lock(_mtx);
    if (_buf.empty()) {
      auto ret =
          _empty_cond.wait_for(lock, timeout, [&]() { return _buf.empty(); });
      if (!ret)
        throw std::runtime_error("receive timed out!");
    }

    auto it = _buf.begin();
    auto data = std::move(*it);
    _buf.erase(it);
    _full_cond.notify_one();

    return data;
  }

private:
  int _size;
  std::vector<T> _buf;
  std::mutex _mtx;
  std::condition_variable _empty_cond; // used to wait when the buffer is empty
  std::condition_variable _full_cond;  // used to wait when the buffer is full
};

std::string generateName(std::mt19937 &rng);

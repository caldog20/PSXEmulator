//
// Created by Caleb Yates on 12/16/22.
//

#pragma once

#include <queue>
#include <cstddef>
#include <mutex>
#include <atomic>
#include <optional>
#include <condition_variable>

namespace calib {

template <typename T>
class Fifo {
  public:
    Fifo() = default;

    // Disable copies
    Fifo(const Fifo<T>&) = delete;
    Fifo& operator=(const Fifo<T>&) = delete;

    // Disable moves
    Fifo(Fifo<T>&& other) = delete;
    Fifo&& operator=(Fifo<T>&& other) = delete;

    virtual ~Fifo() {}

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [&] { return !m_queue.empty(); });

        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }

    void push(const T& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_cv.notify_one();
    }

    void push(const T&& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(item));
        m_cv.notify_one();
    }



  private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
};

} // namespace calib
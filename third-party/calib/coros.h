//
// Created by Caleb Yates on 12/26/22.
//

#pragma once

#include <concepts>
#include <exception>
#include <experimental/coroutine>
#include <iostream>

namespace calib {

using namespace std::experimental;


template <typename T = void>
struct Coro {
    struct Empty{};
    typedef typename std::conditional<std::is_void<T>::value, Empty, T>::type CoroType;

    Coro() = default;
    Coro(Coro const &) = delete;
    Coro(Coro &&other) = default;
    Coro &operator=(Coro const &) = delete;
    Coro &operator=(Coro  &&other) = default;

    ~Coro() {
        // Destroy Coro handle as long as it's not nullptr
        if (m_handle) destroy();
    }

    void resume() {
        if (m_handle) m_handle.resume();
    }

    void destroy() {
        if (m_handle) m_handle.destroy();
    }

    bool done() {
        if (m_handle) return m_handle.done();
    }

    const CoroType& value() { return m_handle.promise().p_value; }

  private:
    struct PromiseVoid {
        Coro get_return_object() { return coroutine_handle<Promise>::from_promise(*this); }
        auto initial_suspend() {return suspend_always();}
        auto final_suspend() noexcept {return suspend_always();}
        void unhandled_exception() {}
        void return_void() {}
    };

    struct PromiseValue {
        Coro get_return_object() { return coroutine_handle<Promise>::from_promise(*this); }
        auto initial_suspend() {return suspend_always();}
        auto final_suspend() noexcept {return suspend_always();}
        void return_value(CoroType value) { p_value = value; }
        void unhandled_exception() {}
        suspend_always yield_value(CoroType value) {
            p_value = value;
            return {};
        }
        CoroType p_value;
    };




    typedef typename std::conditional<std::is_void<T>::value, PromiseVoid, PromiseValue>::type Promise;
    Coro(coroutine_handle<Promise> h) : m_handle(h) {}
    coroutine_handle<Promise> m_handle;

  public:
    using promise_type = Promise;

};

template <typename T>
class Generator {
  public:
    struct promise_type;
    using handle = coroutine_handle<promise_type>;

    struct promise_type {
        std::exception_ptr p_exception;

        Generator get_return_object() {
            return handle::from_promise(*this);
        }
        suspend_always initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }

        void return_void() { }
        void unhandled_exception() {p_exception = std::current_exception(); }

        suspend_always yield_value(const T value) {
            current_value = value;
            return {};
        }

        T current_value;
    };

    explicit Generator(handle h) : m_handle(h) {}
    ~Generator() {
        if (m_handle) m_handle.destroy();
    }

    Generator(Generator&) = delete;
    Generator(Generator&&) = delete;

    T value() {
        return m_handle.promise().current_value;
    }

    bool next() {
        m_handle.resume();
        return !m_handle.done();
    }

  private:
    handle m_handle;
};

}  // namespace calib
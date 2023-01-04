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

struct VoidTask {
    struct promise_type {
        using Handle = coroutine_handle<promise_type>;
        std::exception_ptr p_exception;

        auto get_return_object() { return VoidTask{Handle::from_promise(*this)}; }
        suspend_always initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }

        void return_void() {}
        void unhandled_exception() { p_exception = std::current_exception(); }
    };

    explicit VoidTask(promise_type::Handle coro) : coro_(coro) {}

    ~VoidTask() {
        if (coro_) destroy();
    }

    void destroy() { coro_.destroy(); }
    void resume() { coro_.resume(); }

  private:
    promise_type::Handle coro_;
};

}  // namespace calib
/*
 * MIT License
 *
 * Copyright(c) 2020 DaiMingze
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include<condition_variable>
#include<thread>
#include<queue>
#include<future>
#include<iostream>
#include<atomic>

namespace GreyDawn
{
    class ThreadPool
    {
    public:
        ThreadPool()
        {
            start();
        }

        explicit ThreadPool(uint32_t num_thread)
        {
            start(num_thread);
        }

        ~ThreadPool()
        {
            stop();
        }

        template<typename Task, typename ...Args>
        void executeTask(Task&& task, Args&& ...args)
        {
            std::shared_ptr<std::function<void()>> executeTask =
                std::make_shared<std::function<void()>>(
                    [task = std::move(task), args = std::make_tuple(std::forward<Args>(args)...)]() mutable
                    {
                        return std::apply(std::move(task), std::move(args));
                    }
            );
            {
                std::unique_lock<std::mutex> UniqueLock(mutex_);
                tasks_.emplace([=] {
                    (*executeTask)();
                });
            }
            condition_variable_.notify_one();
        }

        template<typename Task>
        void executeTask(Task&& task)
        {
            std::shared_ptr<std::function<void()>> executeTask = std::make_shared<std::function<void()>>(std::forward<Task>(task));
            {
                std::unique_lock<std::mutex> UniqueLock(mutex_);
                tasks_.emplace([=] {
                    (*executeTask)();
                });
            }
            condition_variable_.notify_one();
        }

        template<typename Task, typename ...Args>
        auto packagedTask(Task&& task, Args&& ...args)
        {

            std::shared_ptr< std::packaged_task<std::invoke_result_t<Task, Args...>()> > packaged_task =
                std::make_shared< std::packaged_task<std::invoke_result_t<Task, Args...>()> >(
                    [task = std::move(task), args = std::make_tuple(std::forward<Args>(args)...)]() mutable
                    {
                        return std::apply(std::move(task), std::move(args));
                    }
            );

            {
                std::unique_lock<std::mutex> UniqueLock(mutex_);
                tasks_.emplace([=] {
                    (*packaged_task)();
                });
            }
            condition_variable_.notify_one();
            return packaged_task->get_future();
        }

        template<typename Task>
        auto packagedTask(Task&& task)->std::future<decltype(std::forward<Task>(task)())>
        {
            std::shared_ptr< std::packaged_task<decltype(std::forward<Task>(task)())()> > packaged_task =
                std::make_shared< std::packaged_task<decltype(std::forward<Task>(task)())()> >(task);
            {
                std::unique_lock<std::mutex> UniqueLock(mutex_);
                tasks_.emplace([=] {
                    (*packaged_task)();
                });
            }
            condition_variable_.notify_one();
            return packaged_task->get_future();
        }

        void waitTaskEmpty()
        {
            while (!taskEmpty())
                std::this_thread::yield();
        }

        bool taskEmpty()
        {
            return tasks_.size() == 0;
        }

        int idleNumber()
        {
            return thread_count_ - current_working_;
        }

    private:
        int thread_count_ = 0;
        std::atomic_int32_t current_working_;
        bool stop_ = false;
        std::mutex mutex_;
        std::vector<std::thread> threads_;
        std::queue<std::function<void()>> tasks_;
        std::condition_variable condition_variable_;

        void start(uint32_t NumThread = 0)
        {
            uint32_t thread_count_ = NumThread ? NumThread : (std::thread::hardware_concurrency() * 2);
            current_working_ = 0;
            for (size_t i = 0; i < thread_count_; i++)
            {
                threads_.emplace_back([=,id = i] {
                    std::function<void()> task;
                    while (true)
                    {
                        {
                            std::unique_lock<std::mutex> lock(this->mutex_);
                            current_working_++;
                            condition_variable_.wait(lock, [=] {return this->stop_ || !tasks_.empty(); });
                            current_working_--;
                            if (this->stop_ && tasks_.empty())
                                break;
                            task = std::move(this->tasks_.front());
                            this->tasks_.pop();
                        }
                        task();
                    }
                });
            }
        }

        void stop()noexcept
        {
            {
                std::unique_lock<std::mutex> lock{ mutex_ };
                stop_ = true;
            }
            condition_variable_.notify_all();
            for (auto& thread : threads_)
            {
                thread.join();
            }
        }
    };
}
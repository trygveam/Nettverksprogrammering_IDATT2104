#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>

using namespace std;

struct Workers
{
    vector<thread> threads;
    list<function<void()>> tasks;
    mutex tasks_mutex;
    condition_variable cv;
    int thread_count;
    bool join_called;

    explicit Workers(int threads)
    {
        thread_count = threads;
        join_called = true;
    }

    void start()
    {
        if (!join_called)
        {
            return;
        }

        join_called = false;

        for (int i = 0; i < thread_count; i++)
        {
            threads.emplace_back([i, this]
                                 {
                                     while (true)
                                     {
                                         function<void()> task;
                                         {
                                             unique_lock<mutex> lock(tasks_mutex);
                                             while (tasks.empty())
                                             {
                                                 cout << to_string(i) + " no tasks to work on \n";
                                                 if (join_called)
                                                     return;
                                                 cout << to_string(i) + " sleeping \n";
                                                 cv.wait(lock);
                                                 cout << to_string(i) + " got woke up \n";
                                             }

                                             task = *tasks.begin();
                                             tasks.pop_front();
                                         }
                                         if (task)
                                         {
                                             cout << to_string(i) + ": Running task.\n";
                                             task();
                                         }
                                     } });
        }
    }

    void post(const function<void()> &f)
    {
        unique_lock<mutex> lock(tasks_mutex);
        tasks.emplace_back(f);
        cv.notify_one();
    }

    void post_timeout(const function<void()> &f, long timeout_ms)
    {
        post([&f, timeout_ms]
             {
                 this_thread::sleep_for(chrono::milliseconds(timeout_ms));
                 f(); });
    }

    void join()
    {
        join_called = true;
        cv.notify_all();
        for (auto &thread : threads)
        {
            thread.join();
            cout << " thread returned:\n";
        }
    }
};

int main()
{
    Workers worker_threads(3);

    Workers event_loop(1);

    worker_threads.start();
    event_loop.start();

    worker_threads.post([]
                        { cout << "Function 1 running in thread: "
                               << this_thread::get_id() << endl; });
    worker_threads.post([]
                        { cout << "Function 2 running in thread: "
                               << this_thread::get_id() << endl; });
    worker_threads.post([]
                        { cout << "Function 3 running in thread: "
                               << this_thread::get_id() << endl; });
    event_loop.post([]
                    { cout << "Event loop function 1 running in thread: "
                           << this_thread::get_id() << endl; });

    event_loop.post([]
                    { cout << "Event loop function 2 running in thread: "
                           << this_thread::get_id() << endl; });

    worker_threads.post_timeout([]
                                { cout << "Function 4 running on thread: " << this_thread::get_id() << endl; },
                                4000);
    this_thread::sleep_for(chrono::milliseconds(2000));
    worker_threads.join();
    event_loop.join();

    return 0;
}
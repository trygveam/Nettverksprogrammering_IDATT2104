#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>

using namespace std;

struct WorkerPool
{
    vector<thread> threads;
    list<function<void()>> tasks;
    mutex tasks_mutex;
    condition_variable tasks_cv;

    int thread_count;
    bool join_called;

    explicit WorkerPool(int threads)
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
                while(true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(tasks_mutex);
                        while(tasks.empty()) {
                            cout << to_string(i) + " no tasks \n";
                            if(join_called) return;
                            cout << to_string(i) + " sleep \n";
                            tasks_cv.wait(lock);
                            cout << to_string(i) + " woke up \n";
                        }

                        task = *tasks.begin();
                        tasks.pop_front();

                    }
                    if(task) {
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
        tasks_cv.notify_one();
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
        tasks_cv.notify_all();
        for (auto &thread : threads)
        {
            thread.join();
            cout << "Thread returned.\n";
        }
    }
};

int main()
{
    WorkerPool worker_threads(3);

    worker_threads.start(); // Create internal threads

    worker_threads.post([]
                        { cout << "Function 1 running.\n"; });
    worker_threads.post([]
                        { cout << "Function 2 running.\n"; });
    worker_threads.post([]
                        { cout << "Function 3 running.\n"; });
    worker_threads.post_timeout([]
                                { cout << "Function 4 running.\n"; },
                                4000);

    cout << "Calling join.\n";
    worker_threads.join(); // Calls join() on the worker threads

    return 0;
}
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <bits/stdc++.h>

using namespace std;

// Function for finding prime numbers
void primeInRange(int L, int R, vector<int> *primes)
{
    int flag;

    for (int i = L; i <= R; i++)
    {
        if (i == 1 || i == 0)
            continue;
        flag = 1;
        for (int j = 2; j <= i / 2; ++j)
        {
            if (i % j == 0)
            {
                flag = 0;
                break;
            }
        }
        if (flag == 1)
            primes->emplace_back(i);
    }
    // Function call to get thread id
    //cout << std::this_thread::get_id() << endl;
}

int main()
{
    int L = 1;
    int R = 100;
    int numberOfThreads = 5;
    int intervall = 20;

    vector<int> primes;
    vector<int> arr[numberOfThreads];
    vector<thread> threads;

    // Creating threads
    for (int i = 0; i < numberOfThreads; i++)
    {
        threads.emplace_back(primeInRange, (intervall * i) + 1, intervall * (i + 1), &arr[i]);
    }

    // Waiting for all threads to finish
    for (auto &thread : threads)
        thread.join();

    // Combining all vectors
    for (int i = 0; i < numberOfThreads; i++)
    {
        primes.insert(primes.end(), arr[i].begin(), arr[i].end());
    }

    // Printint to console
    for (int i = 0; i < primes.size(); i++)
    {
        cout << primes.at(i) << " ";
    }
    return 0;
}
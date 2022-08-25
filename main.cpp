#include "ThreadPool.h"
#include <ratio>
#include <iostream>
#include <random>

bool make_thread = true;
ThreadPool pool;

void quickSort(int*& arr, int low, int high)
{
    int mid, count;
    int l = low, h = high;
    mid = arr[(l + h) / 2]; //âû÷èñëåíèå îïîðíîãî ýëåìåíòà
    do
    {
        while (arr[l] < mid)
            l++;
        while (arr[h] > mid)
            h--;
       
        if (l <= h) //ïåðåñòàíîâêà ýëåìåíòîâ
        {
            count = arr[l];
            arr[l] = arr[h];
            arr[h] = count;
            l++;
            h--;
        }
    } while (l < h);
    /*if (low < h)
        quickSort(arr, low, h);
    if (l < high)
        quickSort(arr, l, high);*/

    if (make_thread && (high - low > 10000))
    {
        if (low < h)
        {
            auto f = pool.push_task(quickSort, arr, low, h); 
            f.wait_for(std::chrono::seconds(0)) == std::future_status::timeout;
        }
        if (l < high)
            quickSort(arr, l, high);
    }
    else {
        if (low < h)
            quickSort(arr, low, h);
        if (l < high)
            quickSort(arr, l, high);
    }
}

int main()
{
    srand(time(nullptr));
    long arr_size = 100000000;
    int* array = new int[arr_size];
    for (long i = 0; i < arr_size; i++) {
        array[i] = rand() % 10000;
    }
    
    // ìíîãîïîòî÷íûé çàïóñê
    std::cout << "Multi thread" << std::endl;
    auto begin = std::chrono::system_clock::now();
    quickSort(array, 0, arr_size - 1);
    auto end = std::chrono::system_clock::now();
    std::cout << "The time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " milliseconds" << std::endl << std::endl;

    for (long i = 0; i < arr_size - 1; i++) {
        if (array[i] > array[i + 1]) {
            cout << "Unsorted" << endl;
            break;
        }
    }
    
    for (long i = 0; i < arr_size; i++) {
        array[i] = rand() % 10000;
    }
    // îäíîïîòî÷íûé çàïóñê
    make_thread = false;
    std::cout << "Single thread" << std::endl;
    begin = std::chrono::system_clock::now();
    quickSort(array, 0, arr_size - 1);
    end = std::chrono::system_clock::now();
    std::cout << "The time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " milliseconds" << std::endl << std::endl;
        
    delete[] array;
    return 0;
}

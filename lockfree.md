# Questions network latency

1. Delete oldest log file in a directory, while maintaining a maximum of 3 log files.  (see `log.c`)

    First, one need a function to list all log files in a directory.Use the `opendir` and `readdir` functions in the `dirent.h` library to list all files in the directory. One can use the stat function in sys/stat.h to check if each file is a regular file (not a directory or special file).

    Next, I need to identify the log files. I would start by checking the filename of each file. For example, we might check if the filename ends in ".log".

    One need to sort the log files by modification time. One can use the stat function again to get the modification time of each file. We can store the filenames and modification times in an array of structs, then sort the array.

    Finally, I need to delete the oldest log files until only three remain. I do this by iterating over the sorted array from oldest to newest, deleting each file with the remove function until only three files remain.


2. Multiple threads in a program needs access to a particular configuration/data. The threads are on a program critical path and in a low latency network environment. How will you update the configuration/data and make it available to the threads.


In a multi-threaded environment, ensuring safe access to shared data can be a challenging task. Traditionally, this is managed by using synchronization primitives like mutexes or locks to protect the shared data. But in a low latency or real-time environment, mutexes might not be the best choice due to their blocking nature, which might cause threads to be suspended, leading to context switching and potential priority inversion issues. (see https://www.youtube.com/watch?v=ndeN983j_GQ)

One approach to handle such a scenario is to use lock-free programming techniques, like atomic operations and lock-free data structures. The main idea behind lock-free programming is to allow multiple threads to operate on the data concurrently, but ensure that their operations are atomic - that is, they either happen completely or don't happen at all, and they don't interfere with each other.

In this specific case, an effective method would be to use a combination of atomic operations and versioned data structures. The shared configuration data could be stored in a structure along with a version number. Each time the data is updated, a new copy of the data is made, the changes are applied to the copy, and the version number is incremented. Then, an atomic "compare and swap" operation is used to replace the old version of the data with the new one, if and only if the version number hasn't changed since we made the copy.

Meanwhile, threads that need to access the data would use an atomic operation to obtain a pointer or reference to the current version of the data. As the updates are made to a copy of the data, the threads can continue to use the old version of the data until they're ready to move on to the new version, at which point they can fetch the new version using another atomic operation.

This approach ensures that updates to the data don't block threads that are accessing the data, and also that threads always have a consistent view of the data. It's also suitable for a real-time or low latency environment, as it avoids the use of locks and the potential for blocking. However, it requires careful programming to ensure correct operation, and it might not be suitable for all types of data or update operations.

For instance, if multiple updates need to be applied to the data in a specific order, or if the updates are complex and can't easily be made to a copy of the data, then a different approach might be needed. In such a case, one might need to consider other techniques, like using a real-time or lock-free queue to serialize the updates, or using a real-time locking protocol that avoids priority inversion, such as the priority inheritance or priority ceiling protocols.


In C, use the POSIX threads (pthreads) library for threading, and C11 atomic types or GCC built-in atomic operations for atomic operations. Here's we go

define the Config struct. This will be similar to the C++ version, but with C-style syntax

```cpp
struct Config {
    // Earlier configuration data here.
    double some_parameter;
};

```

2. Define a global variable to hold the shared config, along with a mutex to protect it

```cpp
struct Config* config;
pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;
```

3. define the updateConfig and useConfig functions. These will use the pthreads mutex functions to ensure safe access to the shared config

```cpp
void updateConfig() {
    pthread_mutex_lock(&config_mutex);
    // Update the config here...
    pthread_mutex_unlock(&config_mutex);
}

void useConfig() {
    pthread_mutex_lock(&config_mutex);
    // Use the config here...
    pthread_mutex_unlock(&config_mutex);
}
```

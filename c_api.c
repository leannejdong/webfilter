#include <stddef.h>
#include <pthread.h>

struct Config {
    // My hardware room reverb

    double dryWetMix; // Between 0.0 (all dry) and 1.0 (all wet)
    double roomSize;  // Between 0.0 (smallest room) and 1.0 (largest room)
    double damping;   // Between 0.0 (no damping) and 1.0 (maximum damping)
    double width;     // Between 0.0 (mono) and 1.0 (wide stereo)

};

struct Config* config;
pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;


#define NUM_THREADS 10

void* updateThread(void* arg) {
    for (int i = 0; i < 10; ++i) {
        updateConfig();
        sleep(1);
    }
    return NULL;
}

void* useThread(void* arg) {
    for (int i = 0; i < 100; ++i) {
        useConfig();
        usleep(100000); // 100 milliseconds
    }
    return NULL;
}

int main() {
    // Initialize the config.
    config = malloc(sizeof(struct Config));
    // Initialize the config...

    // Create the threads.
    pthread_t threads[NUM_THREADS];
    pthread_create(&threads[0], NULL, updateThread, NULL);
    for (int i = 1; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, useThread, NULL);
    }

    // Wait for the threads to finish.
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Clean up.
    free(config);

    return 0;
}

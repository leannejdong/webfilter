
//https://leanne.godbolt.org/z/Tzhc4rds7
#include <atomic>
#include <memory>
#include <thread>

struct Config {
    // My hardware room reverb

    double dryWetMix; // Between 0.0 (all dry) and 1.0 (all wet)
    double roomSize;  // Between 0.0 (smallest room) and 1.0 (largest room)
    double damping;   // Between 0.0 (no damping) and 1.0 (maximum damping)
    double width;     // Between 0.0 (mono) and 1.0 (wide stereo)

    Config()
        : dryWetMix(0.5)
        , roomSize(0.5)
        , damping(0.5)
        , width(1.0)
    {}

    // Optionally, add a method to validate the configuration parameters.
    void validate() {
        dryWetMix = clamp(dryWetMix, 0.0, 1.0);
        roomSize = clamp(roomSize, 0.0, 1.0);
        damping = clamp(damping, 0.0, 1.0);
        width = clamp(width, 0.0, 1.0);
    }

    // Helper to clamp a value between a minimum and maximum value.
    double clamp(double value, double min, double max) {
        return std::max(min, std::min(max, value));
    }
};

std::shared_ptr<Config> config;

void updateConfig() {
    // Create a new Config object and modify it as necessary.
    std::shared_ptr<Config> newConfig = std::make_shared<Config>(*config);
    // Modify newConfig as needed

    // Atomically replace the old Config with the new one.
    std::atomic_store(&config, std::move(newConfig));
}

void useConfig() {
    // Atomically load the current Config.
    std::shared_ptr<Config> currentConfig = std::atomic_load(&config);

}

int main() {
    // Initialize the shared Config.
    config = std::make_shared<Config>();

    // Launch a thread to update the Config.
    std::thread updater([] {
        for (int i = 0; i < 10; ++i) {
            updateConfig();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    // Launch threads to use the Config.
    std::thread users[10];
    for (auto& user : users) {
        user = std::thread([] {
            for (int i = 0; i < 100; ++i) {
                useConfig();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    // Join the updater thread.
    updater.join();

    // Join the user threads.
    for (auto& user : users) {
        user.join();
    }
}

/*
Multiple threads in a program needs access to a particular configuration/data. 
The threads are on a program critical path and in a low latency network environment. 
How will you update the configuration/data and make it available to the threads.
*/

/*
raditionally, this is managed by using synchronization primitives like mutexes or locks 
to protect the shared data. But in a low latency or real-time environment, 
mutexes might not be the best choice due to their blocking nature, which might cause threads to be suspended, 
leading to context switching and potential priority inversion issues. I will go with some lockfree data structures.
This is quite advanced :)

*/



//Ref: https://timur.audio/using-locks-in-real-time-audio-processing-safely
// Many cppcon talks on lockfree programming, and audio dev conf.


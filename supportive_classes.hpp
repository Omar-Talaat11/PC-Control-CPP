#include <thread>



class ShutdownException : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Shutdown requested";
    }
};


class startThread
{
public:
    template <typename Callable,typename argument>
    startThread(Callable&& func,argument ar=""): t1(std::thread(std::forward<Callable>(func),ar))
    {
        // Detach the thread to allow it to run independently
        t1.detach();
    }

    // Delete copy constructor and assignment operator to prevent copying
    startThread(const startThread&) = delete;
    startThread& operator=(const startThread&) = delete;

    // Destructor is not needed to join the thread because it's detached
    ~startThread() = default;

private:
    std::thread t1;
};
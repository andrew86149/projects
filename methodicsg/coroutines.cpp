#include <iostream>
#include <vector>
#include <coroutine>
#include <memory>
#include <chrono>
#include <functional>
#include <string>
#include <stdexcept>
#include <atomic>
#include <thread>

// client <-> coroutine-handle <=> promise-object <=> coroutine-frame
class Event{
public:
    Event() = default;

    Event(const Event&) = delete;
    Event(Event&&) = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&) = delete;

    class Awaiter;
    Awaiter operator co_await() const noexcept;

    void notify() noexcept;
    private:
    friend class Awaiter;
    mutable std::atomic<void*> suspendedWaiter{nullptr};
    mutable std::atomic<bool> notified{false};
};
class Event::Awaiter{
    public:
    Awaiter(const Event& eve): event(eve){}
    bool await_ready() const;
    bool await_suspend(std::coroutine_handle<> corHandle) noexcept;
    void await_resume() noexcept{}
    private:
    friend class Event;
    const Event& event;
    std::coroutine_handle<> coroutineHandle;
};
bool Event::Awaiter::await_ready() const {
    if(event.suspendedWaiter.load() != nullptr){
        throw std::runtime_error("More than one waiter is not valid");
    }
    return event.notified;
}
bool Event::Awaiter::await_suspend(std::coroutine_handle<> corHandle) noexcept{
    coroutineHandle = corHandle;
    if(event.notified) return false;
    event.suspendedWaiter.store(this);
    return true;
}
void Event::notify() noexcept{
    notified = true;
    auto* waiter = static_cast<Awaiter*>(suspendedWaiter.load());
    if(waiter != nullptr){
        waiter->coroutineHandle.resume();
    }
}
Event::Awaiter Event::operator co_await() const noexcept{
    return Awaiter{ *this };
}
struct Task{
    struct promise_type{
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};
Task receiver(Event& event){
    auto start = std::chrono::high_resolution_clock::now();
    co_await event;
    std::cout <<"Got the notification! " << '\n';
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Waited " << elapsed.count() << " seconds." << '\n';
}
using namespace std::chrono_literals;

struct Job{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coro;
    
    Job(handle_type h): coro(h){}
    ~Job(){ if(coro) coro.destroy(); }

    void start(){ coro.resume(); }

    struct promise_type{
        auto get_return_object(){ return Job{handle_type::from_promise(*this)}; }
        std::suspend_always initial_suspend(){
            std::cout << "   Prepare jop" << '\n';
            return {};
        }
        std::suspend_always final_suspend() noexcept{
            std::cout << "   Performing jop" << '\n';
            return {};
        }
        void return_void(){}
        void unhandled_exception(){}
    };
};

Job prepareJob(){
    co_await std::suspend_never();
}

template<typename T>
struct Generator{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coro; // coroutine handle

    Generator(handle_type h): coro(h){} //конструктор получает дескриптор сопрограммы
    ~Generator(){ if(coro) coro.destroy(); }

    Generator(const Generator&) = delete;
    Generator& operator = (const Generator&) = delete;

    Generator(Generator&& oth) noexcept : coro(oth.coro) { 
        oth.coro = nullptr; 
    }
    Generator& operator = (Generator&& oth) noexcept{
        coro = oth.coro;
        oth.coro = nullptr;
        return *this;
    }
    // запросить у promise-object значение при помощи дескриптора сопрограммы
    T getValue(){ return coro.promise().current_value; }
    // позволяют клиенту возобновить выполнение promise-object
    bool next(){
        coro.resume();
        return not coro.done();
    }
    
    struct promise_type{
        promise_type() = default;// default ctor [1]
        ~promise_type() = default;
        // является ли сопрограмма приостановленной(suspend) до своего выполнения [2], возвращает ожидаемый объект(awaitable)
        auto initial_suspend(){ 
            return std::suspend_always{}; 
        }
        // Concept Awaitable  требует реализации трёх методов: await_ready, await_suspend, arait_resume
        //С++20 уже определены два базовых ожидаемых объектв: 
        //suspend_always всегда приостанавливает выполнение,    await_ready=false 
        //suspend_never никогда не приостанавливает выполнение, await_ready=true
        // приостанавливается ли сопрограмма перед завершением [3], возвращает ожидаемый объект(awaitable)
        auto final_suspend() noexcept { 
            return std::suspend_always{}; 
        }
        // возвращает объект-сопрограмму [5],т.е.возобновляемый(resumable) объект,
        //  который клиент использует для взаимодействия с сопрограммой
        // coroutine handle - часть возобнавляемой функции
        auto get_return_object(){ 
            return Generator{handle_type::from_promise(*this)}; 
        }
        // вызывается co_return [7], по крайней мере один из трёх
        // [6]return_value, [7]return_void, [8]yield_value
        auto return_void(){ 
            return std::suspend_never{}; 
        }
        // вызывается co_yeild val [8], возвращает ожидаемый объект(awaitable)
        auto yield_value(const T value){
            current_value = value;
            return std::suspend_always{};
        }
        // вызывается, когда происходит исключение [4]
        void unhandled_exception(){ 
            std::exit(1); 
        }

        T current_value;
    };
    
};
//фабричная функция сопрограммы
Generator<int> getNext(int start=0, int step=1){ 
    auto value = start;
    while (true){
        co_yield value;// promise-object
        value += step;
    }
}

// простейшая сопрограмма
template<typename T>
struct MyFuture{
    std::shared_ptr<T> value;
    MyFuture(std::shared_ptr<T> p): value(p){}
    ~MyFuture(){}
    T get(){ return *value; }

    struct promise_type{
        std::shared_ptr<T> ptr = std::make_shared<T>();
        ~promise_type(){}
        MyFuture<T> get_return_object(){ return ptr; }
        void return_value(T v){ *ptr = v; }
        std::suspend_never initial_suspend(){ return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception(){ std::exit(1); }
    };
    
};

MyFuture<int> createFuture(){ co_return 2026; } // фабрика-coroutine возвращает объект-coroutine

std::vector<int> greedyGenerator(int begin, int end, int inc=1){
    std::vector<int> numbers;
    for(int i=begin; i < end; i += inc){
        numbers.push_back(i);
    }
    return numbers;
}

int main(){
    std::cout << "Examples ..." << std::endl;
    std::cout << "greedyGenerator" << std::endl;
    const auto numbers = greedyGenerator(-10, 11);
    for(auto n : numbers) std::cout << n << " ";
    std::cout << "\n\n";
    for(auto n : greedyGenerator(0, 101, 5)) std::cout << n << " ";
    std::cout << "\n\n";
    std::cout << "simple coroutine" << std::endl;
    auto fut = createFuture();
    std::cout << "fut.get(): " << fut.get() << '\n';
    std::cout << "Generator" << std::endl;
    std::cout << "getNext():";
    auto gen = getNext();
    for(int i=0;i<=10;++i){
        gen.next();// возобновить выполнение promise-object
        std::cout << " " << gen.getValue();// запросить у него значение при помощи дескриптора сопрограммы
    }
    std::cout << "\n\n";
    std::cout << "getNext():";
    auto gen2 = getNext(100, -10);
    for(int i=0;i<=20;++i){
        gen2.next();// возобновить выполнение promise-object
        std::cout << " " << gen2.getValue();// запросить у него значение при помощи дескриптора сопрограммы
    }
    std::cout << "\n\n";
    std::cout << "Before job" << '\n';
    auto job = prepareJob();
    job.start();
    std::cout << "After job" << '\n';

    std::cout << "Notification before waiting" << '\n';
    Event event1{};
    auto senderThread1 = std::thread([&event1]{ event1.notify(); });
    auto receiverThread1 = std::thread(receiver, std::ref(event1));
    receiverThread1.join();
    senderThread1.join();
    std::cout << '\n';

    std::cout << "Notification after 2 seconds waiting" << '\n';
    Event event2{};
    auto receiverThread2 = std::thread(receiver, std::ref(event2));
    auto senderThread2 = std::thread([&event2]{
        std::this_thread::sleep_for(2s);
        event2.notify();
    });
    receiverThread2.join();
    senderThread2.join();
    std::cout << '\n';
    return 0;
}
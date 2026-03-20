#include <cstdio>
#include <utility>

template <typename T>
struct SimpleUniquePointer{
    private:
    T* m_pointer;
    public:
    SimpleUniquePointer() = default;
    SimpleUniquePointer(T* pointer): m_pointer{ pointer } {}
    ~SimpleUniquePointer() { if(m_pointer) delete m_pointer; }
    SimpleUniquePointer(const SimpleUniquePointer&) = delete;
    SimpleUniquePointer& operator=(const SimpleUniquePointer&) = delete;
    SimpleUniquePointer(SimpleUniquePointer&& other) noexcept : m_pointer{other.m_pointer} {
        other.m_pointer = nullptr;
    }
    SimpleUniquePointer& operator=(SimpleUniquePointer&& other) noexcept {
        if(m_pointer) delete m_pointer;
        m_pointer = other.m_pointer;
        other.m_pointer = nullptr;
        return *this;
    }
    T* get() { return m_pointer; }
};

struct Tracer{
    Tracer(const char* name): name{name}{
        printf("%s constructed.\n", name);
    }
    ~Tracer(){
        printf("%s destructed.\n", name);
    }
    private:
    const char* const name;
};

void consumer(SimpleUniquePointer<Tracer> consumer_ptr){
    printf("(cons) consumer_ptr: 0x%p\n", consumer_ptr.get());
}

int main(){
    auto ptr_a = SimpleUniquePointer(new Tracer{"ptr_a"});
    printf("(main) ptr_a: 0x%p\n", ptr_a.get());
    consumer(std::move(ptr_a));
    printf("(main) ptr_a: 0x%p\n", ptr_a.get());
}
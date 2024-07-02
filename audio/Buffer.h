#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
#include <mutex>
template<typename T>
class Buffer
{
private:
    std::vector<T> m_buffer;
    unsigned int m_head;
    unsigned int m_tail;
    unsigned int m_capacity;
    std::mutex m_mutex;
public:
    Buffer(unsigned int n)
    {
        resize(n);
        m_head = 0;
        m_tail = 0;
    }
    
    ~Buffer(){}
    
    void reset() {
        m_head = 0;
        m_tail = 0;
    }
    
    void resize(unsigned int capacity)
    {
        m_capacity = capacity;
        m_buffer.resize(m_capacity);
    }
    
    void put(T val)
    {
        m_buffer[m_head] = val;
        m_mutex.lock();
        m_head = (m_head + 1) % m_capacity;
        m_mutex.unlock();
        if (m_head == m_tail) {
            m_mutex.lock();
            m_tail = (m_tail + 1) % m_capacity;
            m_mutex.unlock();
        }
    }
    
    void pop_front()
    {
        if (empty()) return;
        m_mutex.lock();
        m_tail = (m_tail + 1) % m_capacity;
        m_mutex.unlock();
    }

    unsigned int read(T* vals, unsigned int n)
    {
        for (unsigned int i = 0; i<n; i++) {
            vals[i] = get();
            if (empty()) return i;
        }
        return n;
    }
    
    unsigned int write(T* vals, unsigned int n) {
        for (unsigned int i = 0; i<n; i++) {
            put(vals[i]);
            if (full()) return i;
        }
        return n;
    }
    
    std::vector<T> readAll()
    {
        T values[m_capacity];
        unsigned int n = read(values, m_capacity);
        return std::vector<T>(values, values+n);
    }
    
    T get()
    {
        T val = m_buffer[m_tail];
        pop_front();
        return val;
    }
    
    unsigned int capacity() const
    {
        return m_capacity;
    }
    
    bool empty() const
    {
        return m_head == m_tail;
    }
    
    bool full() const
    {
        return (m_head + 1) % m_capacity == m_tail;
    }
    
    unsigned int size() const
    {
        if (m_head >= m_tail) return m_head - m_tail;
        return m_capacity - (m_tail - m_head);
    }
    
};
#endif

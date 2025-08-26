#pragma once

#include <mutex>

template <typename T>
class Queue
{
    size_t capacity;
    T *data;
    size_t head;
    size_t tail;

    template <typename Val_T>
    void __Enqueue(Val_T &&val);

public:
    Queue() : data(new T[capacity]), head(0), tail(0), capacity(16) {}
    ~Queue() { delete[] data; }

    inline bool Empty() { return head == tail; }

    void Enqueue(T const &elem)
    {
        __Enqueue(elem);
    }

    void Enqueue(T &&elem) noexcept
    {
        __Enqueue(elem);
    }

    T Dequeue()
    {
        auto const &res = data[head++];
        if (head >= capacity) // Wrap to front
            head = 0;
        // TODO: Possibly shrink if too little space is used
        return std::move(res);
    }
};

template <typename T>
class InterThreadQueue
{
private:
    bool multiConsumer, multiProducer;
    std::mutex enqueueMtx;
    std::mutex dequeueMtx;
    Queue<T> queue;

    template <typename Val_T>
    inline void __Enqueue(Val_T &&elem);

public:
    InterThreadQueue(bool multiConsumer = true, bool multiProducer = true) : enqueueMtx(), dequeueMtx(), queue(), multiConsumer(multiConsumer), multiProducer(multiProducer) {}
    ~InterThreadQueue() {}

    inline bool Empty() { return queue.Empty(); }

    void Enqueue(T const &elem) noexcept
    {
        __Enqueue(elem);
    }
    void Enqueue(T &&elem) noexcept
    {
        __Enqueue(elem);
    }

    T Dequeue()
    {
        if (multiConsumer)
        {
            dequeueMtx.lock();
            auto const res = queue.Dequeue();
            dequeueMtx.unlock();
            return std::move(res);
        }
        else
        {
            return queue.Dequeue();
        }
    }
};

template <typename T>
template <typename Val_T>
inline void Queue<T>::__Enqueue(Val_T &&elem)
{
    data[tail++] = std::move(elem);
    if (tail >= capacity) // Wrap to front
        tail = 0;
    if (tail == head) // Grow queue
    {
        auto newData = new T[capacity * 2];
        for (size_t cursor = 0; cursor < capacity; cursor++) // Copy data
        {
            newData[cursor] = data[(head + cursor) % capacity];
        }
        delete[] data;
        data = newData;
        head = 0;
        tail = capacity;
        capacity *= 2;
    }
}

template <typename T>
template <typename Val_T>
inline void InterThreadQueue<T>::__Enqueue(Val_T &&elem)
{
    if (multiProducer)
    {
        enqueueMtx.lock();
        queue.Enqueue(elem);
        enqueueMtx.unlock();
    }
    else
    {
        queue.Enqueue(elem);
    }
}

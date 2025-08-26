#pragma once

#include "Image.h"
#include "Scene.h"
#include "InterThreadQueue.h"
#include <thread>
#include <mutex>
#include <vector>
#include "IntegerPacking.h"

struct TraceTask
{
    uint16_t x, y, w, h;
};

template <typename Result_T>
struct TraceResult
{
    ImageT<Result_T> pixelValues;
    uint16_t x, y;
};

template <typename Tracer_T>
concept RayTracer = (Converter<decltype(Tracer_T::ConvertToPackedInt), typename Tracer_T::ResultType, uint32_t>) &&
                    requires(Tracer_T &tracer, TraceTask const &task, InterThreadQueue<TraceResult<typename Tracer_T::ResultType>> &resultQueue, bool &runningFlag) {
                        { tracer.Work(task) };
                        { Tracer_T(resultQueue, runningFlag) };
                    };

template <RayTracer Tracer_T>
class RayTracingManager
{
private:
    ImageT<uint32_t> &canvas;
    std::mutex &canvasMutex;
    bool &runningFlag;

    InterThreadQueue<TraceTask> taskQueue;
    InterThreadQueue<TraceResult<typename Tracer_T::ResultType>> resultQueue;

    static const uint16_t PATCH_SIZE = 64;
    static const uint16_t THREAD_COUNT = 8;

    std::vector<std::thread> threads;

    inline void DisplayCurrentResults()
    {
        while (!resultQueue.Empty())
        {
            auto const &res = resultQueue.Dequeue();
            canvasMutex.lock();
            canvas.Set(res.pixelValues, res.x, res.y, Tracer_T::ConvertToPackedInt);
            canvasMutex.unlock();
        }
    }

    inline void Trace(Scene const &scene)
    {
        // Create render patches
        for (uint16_t x = 0; x < canvas.W; x += PATCH_SIZE)
        {
            for (uint16_t y = 0; y < canvas.H; y += PATCH_SIZE)
            {
                taskQueue.Enqueue({.x = x, .y = y, .w = std::min<uint16_t>(PATCH_SIZE, canvas.W - x), .h = std::min<uint16_t>(PATCH_SIZE, canvas.H - y)});
            }
        }

        for (uint16_t i = 0; i < THREAD_COUNT; i++)
        {
            threads.push_back(std::thread([this]()
                                          {
                // Work patches
                Tracer_T worker{resultQueue, runningFlag};
                while (runningFlag && !taskQueue.Empty())
                {
                    worker.Work(taskQueue.Dequeue());
                } }));
        }

        // Draw results to canvas
        do
        {
            if (!runningFlag)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            DisplayCurrentResults();
        } while (!taskQueue.Empty());

        for (auto &t : threads)
        {
            t.join();
        }
        DisplayCurrentResults();
    }

public:
    RayTracingManager(ImageT<uint32_t> &canvas, std::mutex &canvasMutex, bool &runningFlag) : canvas(canvas), canvasMutex(canvasMutex), taskQueue(true, false), resultQueue(false), runningFlag(runningFlag) {}
    ~RayTracingManager() {}

    inline void GenerateContent(Scene const &scene)
    {
        Trace(scene);
    }
};

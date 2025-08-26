#include "RGBTracer.h"
#include "RayTracingManager.h"

void RGBTracer::Work(TraceTask const &task)
{
    ImageT<RGB> pixelBuffer{task.w, task.h};

    int rayCount = 0;

    std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 1000));

    for (int update = 0; runningFlag && update < 10; update++)
    {
        for (int ray = 0; runningFlag && ray < 2 + update * update * update; ray++)
        {
            for (int x = 0; x < task.w; x++)
            {
                for (int y = 0; y < task.h; y++)
                {
                    pixelBuffer.at(x, y) += {
                        .R = double(rng()) / rng.max(),
                        .G = double(rng()) / rng.max(),
                        .B = double(rng()) / rng.max()};
                }
            }
            rayCount++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(rng() % 200));
        TraceResult<RGB> res = {
            .pixelValues = {task.w, task.h},
            .x = task.x,
            .y = task.y};

        res.pixelValues.Set(pixelBuffer, 0, 0, [rayCount](RGB const &rgb)
                            { return RGB{rgb.R / rayCount, rgb.G / rayCount, rgb.B / rayCount}; });

        resultEndpoint.Enqueue(std::move(res));
    }
}
# Game of life

- So usage of a nested for loop
- Make use of a collection class, so nom simple vector

600 by 600 grid, FPS is around 5.

## Multi threading

I want to multithread each row of the grid.
For this I will be using the thread pool of the class (MultithreadingDemo).

First fixed a small bug with the threadpool itself. I changed line 31.
```c++
// was this
workers.emplace_back(threadFunc);
// is now this
workers.emplace_back(&WorkerThreadPool::threadFunc, this);
```
After that I applied it to each row of my game of life.
I did change the grid back to a vector, because that was a different assigment.
After changing it to multithreading I only gained 1 FPS. Something is not right.
---
I found 2 possible bottlenecks, the way I draw the grid is wrong of the multithreading taks is too short.
So I fixed the drawing because 600*600 squares each frame does not sound fast.

I put the whole grid into a `sf::VertexArray` that is stored with 2 triangles to make each cell of the grid.
This 10x the FPS from 6 to 60, sometimes it can even spike up to 180.
---
Now that the rendering is correct. Let's fix the thread task that is to small.

I changed the threading to have multipole rows instead of 1. This was better for each thread, now they don't wait as long.

I also increased the size to 1200 by 1800 to have it around 50FPS. But I should test it to the limits.
What about 5000 by 10.000 cells? I can reach 1.6FPS, so that is a good limit. 100 Million triangles that 1 draw each frame.
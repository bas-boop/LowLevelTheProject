# Particle system

At first, I clean up the code given. Moving stuff to a cpp file instead having everything in a hpp file.

Sometimes the window will run and close shortly after OR will it will show a white screen and close shortly after.
This was the first thing I fixed.

Then I tried out structure of arrays (SoA) instead of array of structures (AoS).
The gravity changed a bit, so the task to not change behaviour so not correct at the moment.

I added back the old version, so I can test it with my new version. But it will crash shortly after opening.
The gravity may be wrong, but it does work consistently now.

# The metrics
To check if my new approach is better I run some metrics for both. Where I check the updateMs, renderMS, frameMs, and FPS. The expected number are that the old system is bad around 30-60 FPS and that the new system is much higher than that, at least double than that 60-120 FPS.

## Old system
| Start | End |
|:-----:|:---:|
| ![Old system start](https://github.com/user-attachments/assets/05328219-f82a-4f83-8a27-b06e4be91fd4) | ![Old system end](https://github.com/user-attachments/assets/c77d5f33-76f3-4587-af03-5b64d33d1e8e) |

## New system
| Start | End |
|:-----:|:---:|
| ![New system start](https://github.com/user-attachments/assets/507988ab-ed33-4f2f-b1c5-3af27a86ad6f) | ![New system end](https://github.com/user-attachments/assets/c3b79c59-0889-48af-a8f0-8eeb73282b27) |

# Conclusion
The new system is MUCH better, it taught it would double performance. Is's average around 3000 FPS, that is 50 times more FPS then the old system.

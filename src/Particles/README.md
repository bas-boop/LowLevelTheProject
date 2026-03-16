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
images here, at the start and end

## New system
images here, at the start and end

# Conclusion
The new system is MUCH better, it taught it would double performance. Is's average around 3000 FPS, that is 50 times more FPS then the old system.
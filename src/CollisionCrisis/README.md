# balls

first time running (no changes made):
* 10 - 12 FPS
* 2500 balls

seconde time running (changed vector into set):
* 5 FPS
* 2500 balls

third time running (changed set back to vector):
* 6 FPS
* 2500 balls

fourth time running (with spacial hashing, but broke collision)
* 82 FPS
* 2500 balls

fith time running (with spacial hashing, forgot the call `buildSpatialGrid()`)
* 71 FPS
* 2500 balls

sixth time running (release mode instead of debug mode + code clean-up)
* 180 FPS (spikes to 300)
* 5000 balls
* or
* 1.1
* 250.000 balls
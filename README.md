### TODO list

[ ] Go through libraries and integrate them better into CMake
[ ] Fix all of the warnings VSCode is giving me
[ ] Solve asset including issue

[ ] Reimplement the physics
    [ ] Fix collisions
    [ ] Fix Raycasting
    [ ] Get frustum culling working

[ ] General rewrite
    [ ] Reorganise files
    [ ] Redo rendering API
    [ ] Remove optional

[ ] Debug features
    [ ] Add hierarchy to profiler
    [ ] Add debug rendering to BatchRenderer (lines in 3D and stuff)

[ ] Improve world generation
    [ ] Biomes

[ ] Sound

[ ] Experiment with threading

[ ] Creative inventory

### Known bugs

[ ] Chunk meshing code
[ ] Origin chunk disappearing when moving chunks
[ ] Textures look strange on hotbar
[ ] VoxelRayCast: all normals are 0.0f

### Future issues

[ ] Chunks are never removed from chunk vector - Could cause memory issues in long play sessions
[ ] Do proper error checking
# opengl-minecraft
Basic minecraft implementation in C++ and OpenGL

I compared different rendering approaches to test their performance:
1. 1 VAO per chunk results in cca 250 fps
2. Dynamic VBO:
    1. for all geometry together (face-based-rendering-world-storage, old)
    2. for each chunk separately (chunks store which faces need to be rendered, and the renderer generates vertex data each frame) (face-based-batching)
    3. depends on the size of the buffer
        - If the buffer is small the performance drops ~50fps
        - If the buffer can hold all the vertices, performance is back to ~250fps
3. Static VBO for all geometry together (face-based-rendering-world-storage):
    - The buffer gets set once at the beggining or when the geometry changes
    - Massive performance uplift > 10000fps

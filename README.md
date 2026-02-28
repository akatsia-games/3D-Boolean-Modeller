# 3D-Boolean-Modeller

Constructive solid geometry library based on my personal rewrite of the CSG part of the Java tool/library "UnBBoolean"

## Optimizations

This version will be using a **Bounding-Box B-tree** to hold the elements of the objects(points and triangles) during calculations

This should get the number of calculations down from somewhere around **O(N^2\*M + M^2\*N)**

to **O(N\*log(M) + M\*log(N) + n\*log^2(M) + m\*log^2(N))**,

where "**n**" and "**m**" are the number of boundary triangles between the objects after they have been split,
and "**N**" and "**M**" are the total number of faces at the end

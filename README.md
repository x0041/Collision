# Demo

**[Video Demo](https://www.youtube.com/watch?v=a7SwpA9wUoM)**

# Features

##Octrees

Partition spaces when provided instances of objects based on octant population goals and max depth settings.

##Raycasts

Given an origin and direction, Raycasts can be used on Octrees to perform intersection checks over many different instances of objects while also avoiding some unnecessary intersection checks. Returns hit information:
- Hit Result
- Hit Location
- Hit Distance

## Collision Shapes

Any predefined collision shape can be added to Octrees for raycast checks. Existing shapes include:
- BSpheres
- AABBs
- OBBs
- Planes
- Rays
- Line Segments
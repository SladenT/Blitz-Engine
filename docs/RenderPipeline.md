## Overview

The basic render pipeline consists of a Model, which contains a Mesh and an entity ID.  This entity ID informs us of several things which can be pushed to the shader for rendering, notably, a Material (which generally consists of multiple material attributes, which are simply collections of data to push to the shader), and the Entity transform.

Each of these models is linked to a specific shader, of which each runs in turn, processing each of their models within.  This begins by first uploading a texture array to the GPU, of which we use an offset to grab our texture from.  We then loop through every Model that is within that shader group, and then move onto the next shader.
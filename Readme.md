# CBSP
#### A basic Quake 3 BSP loader, written in C

##### See demos/ for a basic BSP

##### See include/ for the header

It doesn't support all BSP properties because I don't need them

I'll add them later (or you could do it yourself)

Look at the demo, it's a very basic API. See the CBSP struct below
```
typedef struct{
    // Main BSP header
    CBSPheader *mainHeader;

    // The number of each element
    cbsp_int nEnt;
    cbsp_int nTex;
    cbsp_int nPlane;
    cbsp_int nNodes;
    cbsp_int nLeafs;
    cbsp_int nLeaffaces;
    cbsp_int nLeafBrushes;
    cbsp_int nModels;
    cbsp_int nBrushes;
    cbsp_int nBrushSides;
    cbsp_int nVertexs;
    cbsp_int nMeshes;

    // The actual arrays of these elements
    // DO NOT ACCESS THIS FIELD UNLESS YOU WANT A LONG STRING OF THE RAW ENTITIES
    cbsp_string     *mINTERNALEntities;
    CBSPTexture     *mTextures;
    CBSPPlane       *mPlanes;
    CBSPNode        *mNodes;
    CBSPLeaf        *mLeafs;
    CBSPLeafFaces   *mLeafFaces;
    CBSPLeafBrushes *mLeafBrushes;
    CBSPModel       *mModels;
    CBSPBrush       *mBrushes;
    CBSPBrushsides  *mBrushSides;
    CBSPVertex      *mVertices;
    CBSPMesh        *mMeshes;

    // An array of each entity & the entity count
    cbsp_int nEntityCount;
    CBSP_Entity *mEntity;

} CBSP;
```

* The main header is just the file data, casted to a struct. 
* n* is the number of an element
* m* is the pointer to the array of elements
* Do not use anything with internal in it's name unless you've reviewed the code & understand
* The CBSP_Entity is a parsed entity type, it contains the offset to the entity in the raw entity string & the classname. Use CBSP_getKeyFromEntity to get a key value from an entity. It is the programmer's responsibility to cache outputs and such so the slow reading process doesn't need to re-run.

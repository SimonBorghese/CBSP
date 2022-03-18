#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "CBSP.h"

#define SCALE (1/4)

char filename[50];
// Mental Health stigma
char *data;

CBSP *fileowo;
int main()
{
    fileowo = CBSP_loadBSP("test.bsp");
    printf("BSP Signature: %s\n", fileowo->mainHeader->sig);
    printf("BSP Version: %x\n", fileowo->mainHeader->version);
    printf("Reading 11 available properties\n");
    for (int x = 0; x < fileowo->nEntityCount; x++){
        printf("Found entity, name: %s offset: %d\n", fileowo->mEntity[x].classname, fileowo->mEntity[x].offset);
    }

    for (int x = 0; x < fileowo->nTex; x++){
        printf("Found Texture: Name: %s  Content Flags: %d Surface Flags: %d\n", fileowo->mTextures[x].name, fileowo->mTextures[x].content_flag, fileowo->mTextures[x].surface_flag);
    }
    for (int x = 0; x < fileowo->nPlane; x++){
        printf("Found Plane: Normals: %f %f %f Distance: %f\n", fileowo->mPlanes[x].normals[0], fileowo->mPlanes[x].normals[1], fileowo->mPlanes[x].normals[2], fileowo->mPlanes[x].distance);
    }
    for (int x = 0; x < fileowo->nNodes; x++){
        printf("Found Node: Plane: %d Children: %d %d Mins: %d %d %d Maxs: %d %d %d\n", fileowo->mNodes[x].planeindex, fileowo->mNodes[x].childrenindice[0], fileowo->mNodes[x].childrenindice[1],
               fileowo->mNodes[x].mins[0], fileowo->mNodes[x].mins[1], fileowo->mNodes[x].mins[2],
               fileowo->mNodes[x].maxs[0], fileowo->mNodes[x].maxs[1], fileowo->mNodes[x].maxs[2]);
    }
    for (int x = 0; x < fileowo->nLeafs; x++){
        printf("Found Leaf: Cluster: %d Area: %d Mins: %d %d %d Maxs: %d %d %d Leaf Face: %d Number Leaf Faces: %d Leaf Brush: %d Number Leaf Brushes: %d\n",
               fileowo->mLeafs[x].cluster, fileowo->mLeafs[x].area,
               fileowo->mLeafs[x].mins[0], fileowo->mLeafs[x].mins[1], fileowo->mLeafs[x].mins[2],
               fileowo->mLeafs[x].maxs[0], fileowo->mLeafs[x].maxs[1], fileowo->mLeafs[x].maxs[2],
               fileowo->mLeafs[x].leafface, fileowo->mLeafs[x].nLeafface,
               fileowo->mLeafs[x].leafbrush, fileowo->mLeafs[x].nleafbrush);
    }
    for (int x = 0; x < fileowo->nLeaffaces; x++){
        printf("Found Leaf Faces: %d\n", fileowo->mLeafFaces[x]);
    }
    for (int x = 0; x < fileowo->nLeafBrushes; x++){
        printf("Found Leaf Brushes: %d\n", fileowo->mLeafBrushes[x]);
    }
    for (int x = 0; x < fileowo->nModels; x++){
        printf("Found Model: Mins: %f %f %f Maxs: %f %f %f Face: %d Number of Faces: %d Brush: %d Number of Brushes: %d\n",
               fileowo->mModels[x].mins[0], fileowo->mModels[x].mins[1], fileowo->mModels[x].mins[2],
               fileowo->mModels[x].maxs[0], fileowo->mModels[x].maxs[1], fileowo->mModels[x].maxs[2],
               fileowo->mModels[x].face, fileowo->mModels[x].nfaces,
               fileowo->mModels[x].brush, fileowo->mModels[x].nbrushes);
    }
    for (int x = 0; x < fileowo->nBrushes; x++){
        printf("Found Brush: Brush Side: %d Number of Brush Sides: %d Texture: %d\n",
               fileowo->mBrushes[x].brushside, fileowo->mBrushes[x].nbrushside, fileowo->mBrushes[x].texture);
    }
    for (int x = 0; x < fileowo->nBrushSides; x++){
        printf("Found Brush Sides: Plane: %d Texture: %d\n", fileowo->mBrushSides[x].plane, fileowo->mBrushSides[x].texture);
    }
    for (int x = 0; x < fileowo->nVertexs; x++){
        printf("Found Vertices: Pos: %f %f %f Texture Surface: %f %f Texture Light Map; %f %f Normals: %f %f %f Colors: %x %x %x %x\n",
               fileowo->mVertices[x].pos[0], fileowo->mVertices[x].pos[1], fileowo->mVertices[x].pos[2],
               fileowo->mVertices[x].texcord[0][0], fileowo->mVertices[x].texcord[0][1],
               fileowo->mVertices[x].texcord[1][0], fileowo->mVertices[x].texcord[1][1],
               fileowo->mVertices[x].normal[0], fileowo->mVertices[x].normal[1], fileowo->mVertices[x].normal[2],
               fileowo->mVertices[x].color[0],fileowo->mVertices[x].color[1], fileowo->mVertices[x].color[2], fileowo->mVertices[x].color[3]);
    }
    for (int x = 0; x < fileowo->nMeshes; x++){
        printf("Found Meshes: Offset: %d\n", fileowo->mMeshes[x].offset);
    }

    return 0;
}

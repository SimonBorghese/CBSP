#ifndef CBSP_H_INCLUDED
#define CBSP_H_INCLUDED
/*
*
* CBSP Loader Version 1.0
* Please see the provided license file for license information
*
* Author: Simon
*/

#include <stdint.h>
#include <string.h>

// Basic BSP Type Def
typedef uint8_t cbsp_ubyte ;
typedef int cbsp_int ;
typedef float cbsp_float ;
typedef char cbsp_string ;

/*
*
* Below are the raw structures for each element of the BSP format
* They should only be modified if a new BSP format is being implemented
* This is written against the Quake 3 Arena map spec
* https://www.mralligator.com/q3/
*
*/

// Entry struct
typedef struct{
    cbsp_int offset; // Entry Offset
    cbsp_int len;   // Entry Length
} CBSPentry;

// BSP header
typedef struct{
    cbsp_string sig[4];     // Signature
    cbsp_int    version;    // Version
    CBSPentry   entry[17];  // 17 Entries (Quake 3)

} CBSPheader;


// All of the location of each lump
enum cBSPLumps{
    cBSP_Entity      = 0,
    cBSP_Text        = 1,
    cBSP_Plane       = 2,
    cBSP_Node        = 3,
    cBSP_Leaf        = 4,
    cBSP_LeafFace    = 5,
    cBSP_LeafBrush   = 6,
    cBSP_Model       = 7,
    cBSP_Brush       = 8,
    cBSP_BrushSide   = 9,
    cBSP_Vertex      = 10,
    cBSP_Meshes      = 11,
    cBSP_Effect       = 12,
    cBSP_Faces       = 13,
    cBSP_LightMap    = 14,
    cBSP_LightIllum  = 15,
    cBSP_Vis         = 16
};

// Entity Type
typedef struct{
    cbsp_string *str;
} CBSPEntity;

// Texture struct
typedef struct{
    cbsp_string name[64];
    cbsp_int    surface_flag;
    cbsp_int    content_flag;
} CBSPTexture;

// Plane struct
typedef struct{
    cbsp_float normals[3];
    cbsp_float distance;
} CBSPPlane;

// Node Struct
typedef struct{
    cbsp_int planeindex;
    cbsp_int childrenindice[2];
    cbsp_int mins[3];
    cbsp_int maxs[3];
} CBSPNode;

// Leaf Struct
typedef struct{
    cbsp_int cluster;
    cbsp_int area;
    cbsp_int mins[3];
    cbsp_int maxs[3];
    cbsp_int leafface;
    cbsp_int nLeafface;
    cbsp_int leafbrush;
    cbsp_int nleafbrush;
} CBSPLeaf;


// Leaf faces type
typedef cbsp_int CBSPLeafFaces;

// Leaf Brushes type
typedef cbsp_int CBSPLeafBrushes;

// Model struct
typedef struct{
    cbsp_float mins[3];
    cbsp_float maxs[3];
    cbsp_int  face;
    cbsp_int  nfaces;
    cbsp_int  brush;
    cbsp_int  nbrushes;
} CBSPModel;

// Brush Struct
typedef struct{
    cbsp_int brushside;
    cbsp_int nbrushside;
    cbsp_int texture;
} CBSPBrush;

typedef struct{
    cbsp_int plane;
    cbsp_int texture;
} CBSPBrushsides;

typedef struct{
    cbsp_float pos[3];
    cbsp_float texcord[2][2];
    cbsp_float normal[3];
    cbsp_ubyte color[4];
} CBSPVertex;

typedef struct{
    cbsp_int offset;
} CBSPMesh;


/*
*
* Below is the struct for the CBSP Entity type, unlike the mEntities field in CBSP, this struct is intended to be used by the program that uses the library
*
*/
typedef struct{
    cbsp_int offset;
    void *CBSP;
    const cbsp_string* classname;
} CBSP_Entity;

/*
*
* Below is the struct for the CBSP type, a struct containing all of the BSP data
*
*/
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

// copyLump:
// target:      the CBSP object in use
// data:        a uint8 type casted pointer to the raw BSP header data
// lump:        the offset of the lump, from cBSPLumps enum
// struct size: size of the target struct
// targetLump:  a pointer to the target lump pointer, can point to null, but not safe to pass in null itself
cbsp_int CBSP_INTERNAL_copyLump(CBSP *target, uint8_t *data, int lump, size_t structSize, uint8_t **targetLump){
    cbsp_int len = target->mainHeader->entry[lump].len / structSize;
    *targetLump = (uint8_t*) malloc(structSize * len);

    memcpy((*targetLump), &data[target->mainHeader->entry[lump].offset], target->mainHeader->entry[lump].len-2);
    return len;
}

#define CBSP_getKeyFromEntity_FAILURE "Failed to find key in provided entity"
#define CBSP_EOF_FAILURE "Got end of class before key"

// This function is kinda a dumpster fire, designed to iterate through entity, should only be used internally
const char * CBSP_INTERNAL_getKeyFromEntity(CBSP *target, char *targetstr, cbsp_int currentOffset, const char *entity){
    // String containing the target string
    char *str = targetstr;
    // The String to contain the value, although the memory allocation is potentionally large, it removes risk of overflow
    char *foundstr = (char*) malloc(sizeof(char) * target->nEnt);

    // Contains the current position in the string
    int stringlen = 0;
    // This hold the value to determine if the quotes the program is reading are a key (I.E. if firstquotes = 1 then you're reading a key)
    int firstquotes = 1;

    // Used to hold the value to determine if you're in quotes
    int inquotes = 0;
    // Dark name; Used to set the function to return if the key was found
    int markedForDeath = 0;

    // Iterate through the provided entity string, to prevent segfault, the current offset is provided
    for (int i = 0; i < target->nEnt - currentOffset; i++){
        // If the character is equal to a quote
        if (str[i] == '"'){ // "
            // If we're not in quotes, mark ourselves as in quotes
            if (!inquotes){
                inquotes = 1;
            }
            // If we were in quotes, end the string then compare the found keys/values
            else{
                inquotes = 0;
                // If we're in a key, check if the key matches our provided one
                if (firstquotes){
                    if (!strncmp(foundstr, entity, strlen(entity))){
                        // It matches; return on the next string which will be the value
                        markedForDeath = 1;
                    }
                }
                else{
                    // We're not in a key
                    // If we found a value & we're marked for death; return
                    if (markedForDeath){
                        return (const char *)foundstr;
                    }
                }
                // If we didn't find our needed key; free the string to clear it & try again
                free(foundstr);
                foundstr = (char*) malloc(sizeof(char) * target->nEnt);
                stringlen = 0;

                // If we weren't in a key before, we're going to be & if we are in a key, then mark ourselves as not
                firstquotes = !firstquotes;
            }
        }
        else if (str[i] == '}'){
            // If the end of a entity is found without the key we're looking for, exit with failure
            return (const char *) CBSP_getKeyFromEntity_FAILURE;
        }
        else{
            // If we didn't find quotes, a bracket, or an endline, append the current string to our string
            if (inquotes){
                if (str[i] != '\n'){
                    foundstr[stringlen] = str[i];
                    stringlen++;
                    // Add NULL so length could be easily found
                    foundstr[stringlen] = '\0';
                }
            }
        }
    }

    // We shouldn't be able to reach here, but if we do, free the current string and return failure
    if (foundstr){
        free(foundstr);
    }
    return (const char *) CBSP_EOF_FAILURE;
}

// Oh look, another dumpster fire function, let me explain lol
float* CBSP_INTERNAL_convertStringToOrigin(const char *value){
    // Allocate enough space for our target origin float array
    float *position = (float*) malloc(sizeof(float) * 3);
    // Allocate enough space for our string that will contain our string (before it's converted to float)
    char *currentstr = (char*) malloc(sizeof(char) * strlen(value));
    // The current position in the string above
    int strpos = 0;
    // The current float we're on
    int currentNum = 0;
    // Iterate through the string
    for (int p = 0; p < (int) strlen(value); p++){
        switch (value[p]){
        case ' ':
            // If it's a space, end the current number and convert it to float
            position[currentNum] = (float) atof(currentstr);
            // Iterate to the next number
            currentNum++;
            // Reset the string
            strpos = 0;
            // If we're beyond the third number, return, something's wrong
            if (currentNum > 2){
                free(currentstr);
                return position;
            }
            break;
        default:
            // Add the current character to the end
            currentstr[strpos++] = value[p];
            // Add NULL to the end so length can be properly detected
            currentstr[strpos] = '\0';
            break;
        }
    }
    // If we made it to the end, convert the current string to float & return
    position[currentNum] = (float) atof(currentstr);
    free(currentstr);
    return position;
}

const char * CBSP_getKeyFromEntity(CBSP_Entity *ent, const char *key){
    return CBSP_INTERNAL_getKeyFromEntity((CBSP*) ent->CBSP, &((CBSP*)ent->CBSP)->mINTERNALEntities[ent->offset], ent->offset, key);
}

void CBSP_getOriginFromEntity(CBSP_Entity *ent, float **target){
    if (target == NULL){
        return;
    }
    const char *raworigin = CBSP_getKeyFromEntity(ent, "origin");
    if (strcmp(raworigin, CBSP_getKeyFromEntity_FAILURE) != 0 && strcmp(raworigin, CBSP_EOF_FAILURE) != 0){
        *target = CBSP_INTERNAL_convertStringToOrigin(raworigin);
    }
    return;
}

CBSP* CBSP_loadBSP(const char *filename){
    FILE    *file;
    void    *data;
    uint32_t filesize;

    if ((file = fopen(filename, "r")) == NULL){
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if ((data = malloc(sizeof(char) * filesize)) == NULL){
        return NULL;
    }

    fread(data, 1, filesize, file);
    fclose(file);

    CBSP *target = (CBSP*) malloc(sizeof(CBSP));
    target->mainHeader = (CBSPheader*) malloc(sizeof(char) * filesize);
    memmove(target->mainHeader, data, filesize);
    free(data);
    if (target->mainHeader != NULL){
        // F o r m a t t i n g
        target->nEnt =          CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Entity,     sizeof(cbsp_string),     (uint8_t**)&target->mINTERNALEntities);
        target->nTex =          CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Text,       sizeof(CBSPTexture),     (uint8_t**)&target->mTextures);
        target->nPlane =        CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Plane,      sizeof(CBSPPlane),       (uint8_t**)&target->mPlanes);
        target->nNodes =        CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Node,       sizeof(CBSPNode),        (uint8_t**)&target->mNodes);
        target->nLeafs =        CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Leaf,       sizeof(CBSPLeaf),        (uint8_t**)&target->mLeafs);
        target->nLeaffaces =     CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_LeafFace,   sizeof(CBSPLeafFaces),   (uint8_t**)&target->mLeafFaces);
        target->nLeafBrushes =  CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_LeafBrush,  sizeof(CBSPLeafBrushes), (uint8_t**)&target->mLeafBrushes);
        target->nModels =       CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Model,      sizeof(CBSPModel),       (uint8_t**)&target->mModels);
        target->nBrushes =      CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Brush,      sizeof(CBSPBrush),       (uint8_t**)&target->mBrushes);
        target->nBrushSides =   CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_BrushSide,  sizeof(CBSPBrushsides),  (uint8_t**)&target->mBrushSides);
        target->nVertexs =      CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Vertex,     sizeof(CBSPVertex),      (uint8_t**)&target->mVertices);
        target->nMeshes =       CBSP_INTERNAL_copyLump(target, (uint8_t*)target->mainHeader, cBSP_Meshes,     sizeof(CBSPMesh),        (uint8_t**)&target->mMeshes);


        const char *classname;
        cbsp_int entCount = 0;
        target->mEntity = (CBSP_Entity*) malloc(sizeof(CBSPEntity));
        for (int e = 0; e < target->nEnt; e++){
                if (target->mINTERNALEntities[e] == '{'){
                        classname = CBSP_INTERNAL_getKeyFromEntity(target, &target->mINTERNALEntities[e], e, "classname");
                        target->mEntity = (CBSP_Entity*) realloc(target->mEntity, sizeof(CBSP_Entity) * ++entCount);
                        if (target->mEntity == NULL){
                            printf("FAILED TO ALLOCATE MEMORY FOR BSP ENTITY LOADER\n");
                            break;
                        }
                        target->mEntity[entCount-1].offset = e;
                        target->mEntity[entCount-1].CBSP = target;
                        target->mEntity[entCount-1].classname = classname;
                }
        }
        target->nEntityCount = entCount;
    }
    return target;
}
#endif // CBSP_H_INCLUDED

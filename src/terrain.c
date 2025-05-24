#include <stdlib.h>

#include "open-simplex-noise.h"

#include "g.h"
#include "screen.h"
#include "terrain.h"

#define GEN_NOISE_WIDTH_0 (297)
#define GEN_NOISE_WIDTH_1 (217)
#define GEN_FLATBED (128)
#define GEN_SLOPE (369)

static struct osn_context* noiseCtx = NULL;

static struct chunk* rootChunk = NULL;
struct chunk {
    struct tile tiles[WORLD_HEIGHT * CHUNK_WIDTH];
    struct chunk* right;
    int idx;
};

void tInit() {
    open_simplex_noise(rand(), &noiseCtx);
    gDummyX = WORLD_WIDTH / 2;
    gDummyY = WORLD_HEIGHT / 2;
}

#define NOISE(localX, w)                                                                                               \
    (0.5 * (open_simplex_noise2(noiseCtx, (double)(idx * CHUNK_WIDTH + (localX)) / GEN_NOISE_WIDTH_##w, 0.0) + 1.0))
#define IDX (localY * CHUNK_WIDTH + localX)
static struct chunk* genChunk(int idx) {
    struct chunk* chunk = malloc(sizeof(*chunk));
    chunk->idx = idx;
    chunk->right = NULL;

    for (int localX = 0; localX < CHUNK_WIDTH; localX++) {
        int slope = GEN_SLOPE * NOISE(localX, 0) * NOISE(localX, 1);

        int localY = 0;
        for (; localY < GEN_FLATBED + slope; localY++)
            chunk->tiles[IDX].fSolid = 1;
        for (; localY < WORLD_HEIGHT; localY++)
            chunk->tiles[IDX].fSolid = 0;
    }

    return chunk;
}
#undef IDX
#undef NOISE

static void freeChunk(struct chunk* chunk) {
    free(chunk);
}

void tUpdate() {
    int leftIdx = (gDummyX - VIEW_RADIUS + 1) / CHUNK_WIDTH;
    int rightIdx = (gDummyX + VIEW_RADIUS - 1) / CHUNK_WIDTH;

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
    leftIdx = CLAMP(leftIdx, 0, WORLD_WIDTH / CHUNK_WIDTH - 1);
    rightIdx = CLAMP(rightIdx, 0, WORLD_WIDTH / CHUNK_WIDTH - 1);
#undef CLAMP

    if (rootChunk == NULL)
        rootChunk = genChunk(leftIdx);

    while (rootChunk->idx < leftIdx) {
        struct chunk* tmp = rootChunk;
        rootChunk = rootChunk->right;
        freeChunk(tmp);
    }

    while (rootChunk->idx > leftIdx) {
        struct chunk* newChunk = genChunk(rootChunk->idx - 1);
        newChunk->right = rootChunk;
        rootChunk = newChunk;
    }

    struct chunk* rightmost = rootChunk;
    while (rightmost->right != NULL)
        rightmost = rightmost->right;

    while (rightmost->idx < rightIdx) {
        struct chunk* righter = genChunk(rightmost->idx + 1);
        rightmost->right = righter;
        rightmost = righter;
    }

    rightmost = rootChunk;
    while (rightmost != NULL) {
        if (rightmost->idx == rightIdx)
            break;
        rightmost = rightmost->right;
    }

    while (rightmost != NULL && rightmost->right != NULL) {
        struct chunk* tmp = rightmost->right;
        rightmost->right = NULL;
        if (tmp != NULL)
            rightmost->right = tmp->right;
        freeChunk(tmp);
    }
}

void tDraw() {
    for (int screenY = 0; screenY < gRows; screenY++) {
        int worldY = gDummyY - gRows / 2 + screenY;
        for (int screenX = 0; screenX + 1 < gCols; screenX += 2) {
            int worldX = gDummyX - gCols / 4 + screenX / 2;

            if (tAt(worldX, worldY) == NULL || !tAt(worldX, worldY)->fSolid) {
                pxAt(screenX, screenY)->txt = ' ';
                pxAt(screenX, screenY)->bg = C_AQUA;
                pxAt(screenX + 1, screenY)->txt = ' ';
                pxAt(screenX + 1, screenY)->bg = C_AQUA;
            } else {
                pxAt(screenX, screenY)->txt = ' ';
                pxAt(screenX, screenY)->bg = C_WHITE;
                pxAt(screenX + 1, screenY)->txt = ' ';
                pxAt(screenX + 1, screenY)->bg = C_WHITE;
            }
        }
    }
}

void tCleanup() {
    if (noiseCtx != NULL)
        open_simplex_noise_free(noiseCtx);

    while (rootChunk != NULL) {
        struct chunk* tmp = rootChunk;
        rootChunk = rootChunk->right;
        freeChunk(tmp);
    }
}

struct tile* tAt(int x, int y) {
    if (y < 0 || y >= WORLD_HEIGHT)
        return NULL;
    for (struct chunk* chunk = rootChunk; chunk != NULL; chunk = chunk->right)
        if (chunk->idx == x / CHUNK_WIDTH)
            return &chunk->tiles[y * CHUNK_WIDTH + (x % CHUNK_WIDTH)];
    return NULL;
}

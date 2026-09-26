//! PSYQ=3.3 FORCE_MEM=true

#include "jet_private.h"

extern JetNode g_JetNodeListHeads[10];
extern JetNode g_JetNodePool[0xC8];
extern s16 g_JetNodeFreeList[0xC8];
extern s16 g_JetNextFreeNode;
extern JetNode g_JetNodeListTails[10];

static s16 JetNodeIndexAlloc(void);
static void JetNodeIndexFree(s16 index);
static void JetNodeInit(JetNode* node, s16 index);
static void JetNodeLink(JetNode* node, JetNode* parent);
static void JetNodeUnlink(JetNode* node);

void JetNodesInit(void) {
    JetNode* head;
    JetNode* tail;
    s32 i;

    JetNodeInit(&g_JetRootNode, 0);
    g_JetRootNode.depth = 0;
    g_JetNextFreeNode = 0;
    for (i = 0; i < LEN(g_JetNodeFreeList); i++) {
        g_JetNodeFreeList[i] = i + 1;
    }
    for (i = 0; i < LEN(g_JetNodeListHeads); i++) {
        head = &g_JetNodeListHeads[i];
        tail = &g_JetNodeListTails[i];
        head->prev = NULL;
        head->next = tail;
        tail->prev = head;
        tail->next = NULL;
    }
}

static void JetNodeInit(JetNode* node, s16 index) {
    node->m.m[0][0] = 0x1000;
    node->m.m[1][1] = 0x1000;
    node->m.m[2][2] = 0x1000;
    node->m.t[0] = 0;
    node->m.t[1] = 0;
    node->m.t[2] = 0;
    node->m.m[0][1] = 0;
    node->m.m[0][2] = 0;
    node->m.m[1][0] = 0;
    node->m.m[1][2] = 0;
    node->m.m[2][0] = 0;
    node->m.m[2][1] = 0;
    node->parent = &g_JetRootNode;
    node->index = index;
    node->prev = 0;
    node->next = 0;
}

JetNode* JetNodeAlloc(
    s16 modelId, s32 arg1, s32 arg2, s32 arg3, JetNode* parent, s32 x, s32 y, s32 z, s16 rotX, s16 rotY, s16 rotZ) {
    SVECTOR rot;
    JetNode* node;
    JetNode* pool;
    s16 index;

    index = JetNodeIndexAlloc();
    pool = g_JetNodePool;
    node = &pool[index];
    JetNodeLink(node, parent);
    node->model = g_JetModelTable[modelId];
    node->modelId = modelId;
    node->index = index;
    setVector(&rot, rotX, rotY, rotZ);
    RotMatrix(&rot, &node->m);
    node->m.t[0] = x;
    node->m.t[1] = y;
    node->m.t[2] = z;
    return node;
}

void JetNodeFree(JetNode* node) {
    JetNodeUnlink(node);
    JetNodeIndexFree(node->index);
}

static s16 JetNodeIndexAlloc(void) {
    s16* head;
    s16 index;

    head = &g_JetNextFreeNode;
    index = *head;
    *head = g_JetNodeFreeList[index];

    return index;
}

static void JetNodeIndexFree(s16 index) {
    s16* head;
    s16* slot;

    slot = &g_JetNodeFreeList[index];
    head = &g_JetNextFreeNode;
    *slot = *head;
    *head = index;
}

static void JetNodeLink(JetNode* node, JetNode* parent) {
    JetNode* last;
    JetNode* tail;
    s16 depth;

    node->parent = parent;
    depth = parent->depth + 1;
    node->depth = depth;
    tail = &g_JetNodeListTails[depth];
    last = tail->prev;
    node->prev = last;
    node->next = last->next;
    tail->prev->next = node;
    tail->prev = node;
}

static void JetNodeUnlink(JetNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

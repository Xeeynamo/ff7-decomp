//! PSYQ=3.3 FORCE_MEM=true

#include "jet_private.h"

extern VECTOR g_JetLeftPlaneNormal;
extern VECTOR g_JetRightPlaneNormal;
extern s32 g_JetLeftPlaneInsideRef;
extern s32 g_JetRightPlaneInsideRef;
extern s32 g_JetLeftPlaneDistance;
extern s32 g_JetRightPlaneDistance;
extern s32 g_JetLeftNormalLength;
extern s32 g_JetRightNormalLength;

// The four view frustum corner rays at the projection distance, screen order.
const VECTOR g_JetFrustumBottomLeft = {-160, 120, 256, 0};
const VECTOR g_JetFrustumBottomRight = {160, 120, 256, 0};
const VECTOR g_JetFrustumTopLeft = {-160, -120, 256, 0};
const VECTOR g_JetFrustumTopRight = {160, -120, 256, 0};

// Build the left and right frustum planes from the four corner rays.
void JetFrustumInit(void) {
    VECTOR tl;
    VECTOR bl;
    VECTOR tr;
    VECTOR br;
    VECTOR blCorner;
    VECTOR brCorner;
    VECTOR tlCorner;
    VECTOR trCorner;
    VECTOR* ln;
    VECTOR* rn;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;

    ln = &g_JetLeftPlaneNormal;
    rn = &g_JetRightPlaneNormal;
    blCorner = g_JetFrustumBottomLeft;
    brCorner = g_JetFrustumBottomRight;
    tlCorner = g_JetFrustumTopLeft;
    trCorner = g_JetFrustumTopRight;

    tl.vx = tlCorner.vx >> 2;
    tl.vy = tlCorner.vy >> 2;
    tl.vz = tlCorner.vz >> 2;
    bl.vx = blCorner.vx >> 2;
    bl.vy = blCorner.vy >> 2;
    bl.vz = blCorner.vz >> 2;
    tr.vx = trCorner.vx >> 2;
    tr.vy = trCorner.vy >> 2;
    tr.vz = trCorner.vz >> 2;
    br.vx = brCorner.vx >> 2;
    br.vy = brCorner.vy >> 2;
    br.vz = brCorner.vz >> 2;
    OuterProduct0(&tl, &bl, ln);
    OuterProduct0(&tr, &br, rn);

    lx = ln->vx;
    ly = ln->vy;
    lz = ln->vz;
    rx = rn->vx;
    ry = rn->vy;
    rz = rn->vz;
    g_JetLeftPlaneDistance = -(lx * (tlCorner.vx >> 2)) - (ly * (tlCorner.vy >> 2)) - (lz * (tlCorner.vz >> 2));
    g_JetRightPlaneDistance = -(rx * (trCorner.vx >> 2)) - (ry * (trCorner.vy >> 2)) - (rz * (trCorner.vz >> 2));
    g_JetLeftPlaneInsideRef =
        (lx * (trCorner.vx >> 2)) + (ly * (trCorner.vy >> 2)) + (lz * (trCorner.vz >> 2)) + g_JetLeftPlaneDistance;
    g_JetRightPlaneInsideRef =
        (rx * (tlCorner.vx >> 2)) + (ry * (tlCorner.vy >> 2)) + (rz * (tlCorner.vz >> 2)) + g_JetRightPlaneDistance;
    g_JetLeftNormalLength = SquareRoot0((lx * lx) + (ly * ly) + (lz * lz));
    g_JetRightNormalLength = SquareRoot0((rn->vx * rn->vx) + (rn->vy * rn->vy) + (rn->vz * rn->vz));
}

s32 JetVectorInsidePlanes(VECTOR* point) {
    s32 hsLeft;
    s32 rightOk;
    s32 leftOk;
    s32 hsRight;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;

    leftOk = 0;
    rightOk = 0;
    lx = g_JetLeftPlaneNormal.vx;
    ly = g_JetLeftPlaneNormal.vy;
    lz = g_JetLeftPlaneNormal.vz;
    hsLeft = (lx * (point->vx >> 2)) + (ly * (point->vy >> 2)) + (lz * (point->vz >> 2)) + g_JetLeftPlaneDistance;
    rx = g_JetRightPlaneNormal.vx;
    ry = g_JetRightPlaneNormal.vy;
    rz = g_JetRightPlaneNormal.vz;
    hsRight = (rx * (point->vx >> 2)) + (ry * (point->vy >> 2)) + (rz * (point->vz >> 2)) + g_JetRightPlaneDistance;
    if (hsLeft > 0 && g_JetLeftPlaneInsideRef > 0) {
        leftOk = 1;
    }
    if (hsLeft < 0 && g_JetLeftPlaneInsideRef < 0) {
        leftOk = 1;
    }
    if (hsRight > 0 && g_JetRightPlaneInsideRef > 0) {
        rightOk = 1;
    }
    if (hsRight < 0 && g_JetRightPlaneInsideRef < 0) {
        rightOk = 1;
    }
    return leftOk & rightOk;
}

static s32 JetSVectorInsidePlanes(SVECTOR* point) {
    s32 hsLeft;
    s32 rightOk;
    s32 leftOk;
    s32 hsRight;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;

    leftOk = 0;
    rightOk = 0;
    lx = g_JetLeftPlaneNormal.vx;
    ly = g_JetLeftPlaneNormal.vy;
    lz = g_JetLeftPlaneNormal.vz;
    hsLeft = (lx * (point->vx >> 2)) + (ly * (point->vy >> 2)) + (lz * (point->vz >> 2)) + g_JetLeftPlaneDistance;
    rx = g_JetRightPlaneNormal.vx;
    ry = g_JetRightPlaneNormal.vy;
    rz = g_JetRightPlaneNormal.vz;
    hsRight = (rx * (point->vx >> 2)) + (ry * (point->vy >> 2)) + (rz * (point->vz >> 2)) + g_JetRightPlaneDistance;
    if (hsLeft > 0 && g_JetLeftPlaneInsideRef > 0) {
        leftOk = 1;
    }
    if (hsLeft < 0 && g_JetLeftPlaneInsideRef < 0) {
        leftOk = 1;
    }
    if (hsRight > 0 && g_JetRightPlaneInsideRef > 0) {
        rightOk = 1;
    }
    if (hsRight < 0 && g_JetRightPlaneInsideRef < 0) {
        rightOk = 1;
    }
    return leftOk & rightOk;
}

static s32 JetLeftPlaneHalfSpace(s32 x, s32 y, s32 z) {
    s32 nx;
    s32 ny;
    s32 nz;

    nx = g_JetLeftPlaneNormal.vx;
    ny = g_JetLeftPlaneNormal.vy;
    nz = g_JetLeftPlaneNormal.vz;

    return (nx * (x >> 2)) + (ny * (y >> 2)) + (nz * (z >> 2)) + g_JetLeftPlaneDistance;
}

static s32 JetRightPlaneHalfSpace(s32 x, s32 y, s32 z) {
    s32 nx;
    s32 ny;
    s32 nz;

    nx = g_JetRightPlaneNormal.vx;
    ny = g_JetRightPlaneNormal.vy;
    nz = g_JetRightPlaneNormal.vz;

    return (nx * (x >> 2)) + (ny * (y >> 2)) + (nz * (z >> 2)) + g_JetRightPlaneDistance;
}

static s32 JetSphereInsidePlanes(VECTOR* center, s16 radius) {
    s32 leftOk;
    s32 hsLeft;
    s32 rightOk;
    s32 hsRight;
    s32 planeDistance;
    s32 lx;
    s32 ly;
    s32 lz;
    s32 rx;
    s32 ry;
    s32 rz;
    s32 len;

    leftOk = 0;
    rightOk = 0;
    lx = g_JetLeftPlaneNormal.vx;
    ly = g_JetLeftPlaneNormal.vy;
    lz = g_JetLeftPlaneNormal.vz;
    hsLeft = (lx * (center->vx >> 2)) + (ly * (center->vy >> 2)) + (lz * (center->vz >> 2)) + g_JetLeftPlaneDistance;
    if (g_JetLeftPlaneInsideRef > 0 && hsLeft >= 0) {
        leftOk = 1;
    }
    if (g_JetLeftPlaneInsideRef < 0 && hsLeft <= 0) {
        leftOk = 1;
    }
    if (leftOk == 0) {
        len = g_JetLeftNormalLength;
        planeDistance = ((hsLeft < 0) ? -hsLeft : hsLeft) / len;
        if (planeDistance < radius) {
            leftOk = 1;
        }
    }
    rx = g_JetRightPlaneNormal.vx;
    ry = g_JetRightPlaneNormal.vy;
    rz = g_JetRightPlaneNormal.vz;
    hsRight = (rx * (center->vx >> 2)) + (ry * (center->vy >> 2)) + (rz * (center->vz >> 2)) + g_JetRightPlaneDistance;
    if (g_JetRightPlaneInsideRef > 0 && hsRight >= 0) {
        rightOk = 1;
    }
    if (g_JetRightPlaneInsideRef < 0 && hsRight <= 0) {
        rightOk = 1;
    }
    if (rightOk == 0) {
        len = g_JetRightNormalLength;
        planeDistance = ((hsRight < 0) ? -hsRight : hsRight) / len;
        if (planeDistance < radius) {
            rightOk = 1;
        }
    }
    return leftOk & rightOk;
}

static s32 JetSphereInsideLeftPlane(s32 x, s32 y, s32 z, s16 radius) {
    s32 nx;
    s32 ny;
    s32 nz;
    s32 hs;
    s32 ok;
    s32 len;

    nx = g_JetLeftPlaneNormal.vx;
    ny = g_JetLeftPlaneNormal.vy;
    nz = g_JetLeftPlaneNormal.vz;
    ok = 0;
    hs = (nx * (x >> 2)) + (ny * (y >> 2)) + (nz * (z >> 2)) + g_JetLeftPlaneDistance;
    if (g_JetLeftPlaneInsideRef > 0 && hs >= 0) {
        ok = 1;
    }
    if (g_JetLeftPlaneInsideRef < 0 && hs <= 0) {
        ok = 1;
    }
    if (ok == 0) {
        len = g_JetLeftNormalLength;
        if (hs < 0) {
            hs = -hs;
        }
        if (hs / len < radius) {
            ok = 1;
        }
    }
    return ok;
}

static s32 JetSphereInsideRightPlane(s32 x, s32 y, s32 z, s16 radius) {
    s32 nx;
    s32 ny;
    s32 nz;
    s32 hs;
    s32 ok;
    s32 len;

    nx = g_JetRightPlaneNormal.vx;
    ny = g_JetRightPlaneNormal.vy;
    nz = g_JetRightPlaneNormal.vz;
    ok = 0;
    hs = (nx * (x >> 2)) + (ny * (y >> 2)) + (nz * (z >> 2)) + g_JetRightPlaneDistance;
    if (g_JetRightPlaneInsideRef > 0 && hs >= 0) {
        ok = 1;
    }
    if (g_JetRightPlaneInsideRef < 0 && hs <= 0) {
        ok = 1;
    }
    if (ok == 0) {
        len = g_JetRightNormalLength;
        if (hs < 0) {
            hs = -hs;
        }
        if (hs / len < radius) {
            ok = 1;
        }
    }
    return ok;
}

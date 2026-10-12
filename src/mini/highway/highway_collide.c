//! PSYQ=3.3

#include "highway_private.h"

const VECTOR g_HighwayFrustumBottomLeft = {-160, 120, 300, 0};
const VECTOR g_HighwayFrustumBottomRight = {160, 120, 300, 0};
const VECTOR g_HighwayFrustumTopLeft = {-160, -120, 300, 0};
const VECTOR g_HighwayFrustumTopRight = {160, -120, 300, 0};
extern VECTOR g_HighwayLeftPlaneNormal;
extern VECTOR g_HighwayRightPlaneNormal;
extern s32 g_HighwayLeftPlaneDistance;
extern s32 g_HighwayRightPlaneDistance;
extern s32 g_HighwayLeftPlaneInsideRef;
extern s32 g_HighwayRightPlaneInsideRef;
extern s32 g_HighwayLeftNormalLength;
extern s32 g_HighwayRightNormalLength;

// Same as JetFrustumInit.
// Build the left and right frustum planes from the four corner rays.
void HighwayFrustumInit(void) {
    VECTOR tl;
    VECTOR bl;
    VECTOR tr;
    VECTOR br;
    VECTOR blCorner;
    VECTOR brCorner;
    VECTOR tlCorner;
    VECTOR trCorner;

    blCorner = g_HighwayFrustumBottomLeft;
    brCorner = g_HighwayFrustumBottomRight;
    tlCorner = g_HighwayFrustumTopLeft;
    trCorner = g_HighwayFrustumTopRight;
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
    OuterProduct0(&tl, &bl, &g_HighwayLeftPlaneNormal);
    OuterProduct0(&tr, &br, &g_HighwayRightPlaneNormal);
    // Directly multiplies shifted corner coordinates with plane normal components without caching in locals.
    g_HighwayLeftPlaneDistance =
        -((tlCorner.vx >> 2) * g_HighwayLeftPlaneNormal.vx) - (tlCorner.vy >> 2) * g_HighwayLeftPlaneNormal.vy -
        (tlCorner.vz >> 2) * g_HighwayLeftPlaneNormal.vz;
    g_HighwayRightPlaneDistance =
        -((trCorner.vx >> 2) * g_HighwayRightPlaneNormal.vx) - (trCorner.vy >> 2) * g_HighwayRightPlaneNormal.vy -
        (trCorner.vz >> 2) * g_HighwayRightPlaneNormal.vz;
    g_HighwayLeftPlaneInsideRef =
        (trCorner.vx >> 2) * g_HighwayLeftPlaneNormal.vx + (trCorner.vy >> 2) * g_HighwayLeftPlaneNormal.vy +
        (trCorner.vz >> 2) * g_HighwayLeftPlaneNormal.vz + g_HighwayLeftPlaneDistance;
    g_HighwayRightPlaneInsideRef =
        (tlCorner.vx >> 2) * g_HighwayRightPlaneNormal.vx + (tlCorner.vy >> 2) * g_HighwayRightPlaneNormal.vy +
        (tlCorner.vz >> 2) * g_HighwayRightPlaneNormal.vz + g_HighwayRightPlaneDistance;
    g_HighwayLeftNormalLength = SquareRoot0(
        g_HighwayLeftPlaneNormal.vx * g_HighwayLeftPlaneNormal.vx +
        g_HighwayLeftPlaneNormal.vy * g_HighwayLeftPlaneNormal.vy +
        g_HighwayLeftPlaneNormal.vz * g_HighwayLeftPlaneNormal.vz);
    g_HighwayRightNormalLength = SquareRoot0(
        g_HighwayRightPlaneNormal.vx * g_HighwayRightPlaneNormal.vx +
        g_HighwayRightPlaneNormal.vy * g_HighwayRightPlaneNormal.vy +
        g_HighwayRightPlaneNormal.vz * g_HighwayRightPlaneNormal.vz);
}

// Same as JetVectorInsidePlanes.
s32 HighwayVectorInsidePlanes(VECTOR* point) {
    s32 x;
    s32 y;
    s32 z;
    s32 hsLeft;
    s32 hsRight;
    s32 rightOk;
    s32 leftOk;

    // Highway pre-shifts coordinates into x, y, z instead of shifting inline.
    x = point->vx >> 2;
    y = point->vy >> 2;
    z = point->vz >> 2;
    hsLeft = x * g_HighwayLeftPlaneNormal.vx + y * g_HighwayLeftPlaneNormal.vy + z * g_HighwayLeftPlaneNormal.vz +
             g_HighwayLeftPlaneDistance;
    hsRight = x * g_HighwayRightPlaneNormal.vx + y * g_HighwayRightPlaneNormal.vy + z * g_HighwayRightPlaneNormal.vz +
              g_HighwayRightPlaneDistance;
    leftOk = 0;
    rightOk = 0;
    if (hsLeft > 0 && g_HighwayLeftPlaneInsideRef > 0) {
        leftOk = 1;
    }
    if (hsLeft < 0 && g_HighwayLeftPlaneInsideRef < 0) {
        leftOk = 1;
    }
    if (hsRight > 0 && g_HighwayRightPlaneInsideRef > 0) {
        rightOk = 1;
    }
    if (hsRight < 0 && g_HighwayRightPlaneInsideRef < 0) {
        rightOk = 1;
    }
    return leftOk & rightOk;
}

// Same as JetSVectorInsidePlanes.
s32 HighwaySVectorInsidePlanes(SVECTOR* point) {
    s32 x;
    s32 y;
    s32 z;
    s32 hsLeft;
    s32 hsRight;
    s32 rightOk;
    s32 leftOk;

    // Highway pre-shifts coordinates into x, y, z instead of shifting inline.
    x = point->vx >> 2;
    y = point->vy >> 2;
    z = point->vz >> 2;
    hsLeft = x * g_HighwayLeftPlaneNormal.vx + y * g_HighwayLeftPlaneNormal.vy + z * g_HighwayLeftPlaneNormal.vz +
             g_HighwayLeftPlaneDistance;
    hsRight = x * g_HighwayRightPlaneNormal.vx + y * g_HighwayRightPlaneNormal.vy + z * g_HighwayRightPlaneNormal.vz +
              g_HighwayRightPlaneDistance;
    leftOk = 0;
    rightOk = 0;
    if (hsLeft > 0 && g_HighwayLeftPlaneInsideRef > 0) {
        leftOk = 1;
    }
    if (hsLeft < 0 && g_HighwayLeftPlaneInsideRef < 0) {
        leftOk = 1;
    }
    if (hsRight > 0 && g_HighwayRightPlaneInsideRef > 0) {
        rightOk = 1;
    }
    if (hsRight < 0 && g_HighwayRightPlaneInsideRef < 0) {
        rightOk = 1;
    }
    return leftOk & rightOk;
}

// Same as JetLeftPlaneHalfSpace.
s32 HighwayLeftPlaneHalfSpace(s32 x, s32 y, s32 z) {
    // Multiplies the shifted coordinate by the plane normal component directly without local caching.
    return ((x >> 2) * g_HighwayLeftPlaneNormal.vx) + ((y >> 2) * g_HighwayLeftPlaneNormal.vy) +
           ((z >> 2) * g_HighwayLeftPlaneNormal.vz) + g_HighwayLeftPlaneDistance;
}

// Same as JetRightPlaneHalfSpace.
s32 HighwayRightPlaneHalfSpace(s32 x, s32 y, s32 z) {
    // Multiplies the shifted coordinate by the plane normal component directly without local caching.
    return ((x >> 2) * g_HighwayRightPlaneNormal.vx) + ((y >> 2) * g_HighwayRightPlaneNormal.vy) +
           ((z >> 2) * g_HighwayRightPlaneNormal.vz) + g_HighwayRightPlaneDistance;
}

s32 HighwaySphereInsidePlanes(VECTOR* center, s16 radius) {
    s32 leftOk;
    s32 hsLeft;
    s32 rightOk;
    s32 hsRight;
    s32 planeDistance;
    s32 len;

    leftOk = 0;
    rightOk = 0;
    hsLeft = (center->vx >> 2) * g_HighwayLeftPlaneNormal.vx + (center->vy >> 2) * g_HighwayLeftPlaneNormal.vy +
             (center->vz >> 2) * g_HighwayLeftPlaneNormal.vz + g_HighwayLeftPlaneDistance;
    if (g_HighwayLeftPlaneInsideRef > 0 && hsLeft >= 0) {
        leftOk = 1;
    }
    if (g_HighwayLeftPlaneInsideRef < 0 && hsLeft <= 0) {
        leftOk = 1;
    }
    if (!leftOk) {
        len = g_HighwayLeftNormalLength;
        planeDistance = ((hsLeft < 0) ? -hsLeft : hsLeft) / len;
        if (planeDistance < radius) {
            leftOk = 1;
        }
    }
    hsRight = (center->vx >> 2) * g_HighwayRightPlaneNormal.vx + (center->vy >> 2) * g_HighwayRightPlaneNormal.vy +
              (center->vz >> 2) * g_HighwayRightPlaneNormal.vz + g_HighwayRightPlaneDistance;
    if (g_HighwayRightPlaneInsideRef > 0 && hsRight >= 0) {
        rightOk = 1;
    }
    if (g_HighwayRightPlaneInsideRef < 0 && hsRight <= 0) {
        rightOk = 1;
    }
    if (!rightOk) {
        len = g_HighwayRightNormalLength;
        planeDistance = ((hsRight < 0) ? -hsRight : hsRight) / len;
        if (planeDistance < radius) {
            rightOk = 1;
        }
    }
    return leftOk & rightOk;
}

// Same as JetSphereInsideLeftPlane.
s32 HighwaySphereInsideLeftPlane(s32 x, s32 y, s32 z, s16 radius) {
    s32 hs;
    s32 ok;
    s32 len;

    ok = 0;
    // Multiplies the shifted coordinate by the plane normal component directly without local caching.
    hs = ((x >> 2) * g_HighwayLeftPlaneNormal.vx) + ((y >> 2) * g_HighwayLeftPlaneNormal.vy) +
         ((z >> 2) * g_HighwayLeftPlaneNormal.vz) + g_HighwayLeftPlaneDistance;
    if (g_HighwayLeftPlaneInsideRef > 0 && hs >= 0) {
        ok = 1;
    }
    if (g_HighwayLeftPlaneInsideRef < 0 && hs <= 0) {
        ok = 1;
    }
    if (ok == 0) {
        len = g_HighwayLeftNormalLength;
        if (hs < 0) {
            hs = -hs;
        }
        if (hs / len < radius) {
            ok = 1;
        }
    }
    return ok;
}

// Same as JetSphereInsideRightPlane.
s32 HighwaySphereInsideRightPlane(s32 x, s32 y, s32 z, s16 radius) {
    s32 hs;
    s32 ok;
    s32 len;

    ok = 0;
    // Multiplies the shifted coordinate by the plane normal component directly without local caching.
    hs = ((x >> 2) * g_HighwayRightPlaneNormal.vx) + ((y >> 2) * g_HighwayRightPlaneNormal.vy) +
         ((z >> 2) * g_HighwayRightPlaneNormal.vz) + g_HighwayRightPlaneDistance;
    if (g_HighwayRightPlaneInsideRef > 0 && hs >= 0) {
        ok = 1;
    }
    if (g_HighwayRightPlaneInsideRef < 0 && hs <= 0) {
        ok = 1;
    }
    if (ok == 0) {
        len = g_HighwayRightNormalLength;
        if (hs < 0) {
            hs = -hs;
        }
        if (hs / len < radius) {
            ok = 1;
        }
    }
    return ok;
}

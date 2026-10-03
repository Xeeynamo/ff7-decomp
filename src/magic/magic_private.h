#ifndef MAGIC_PRIVATE_H
#define MAGIC_PRIVATE_H

// Shared by the magic overlays only; battle.h stays the public interface.

// Primitive buffer page. Each overlay holds two and alternates between them.
#define MAGIC_PAGE_SIZE 0x10000

// Summon camera script opcodes. Each script word addresses the eye or
// target path (CAM_EYE / CAM_TARGET) and is followed by its operands.
typedef enum {
    CAM_OP_HOLD,
    CAM_OP_SET_POS,
    CAM_OP_MOVE_TO,
    CAM_OP_DOLLY_TO, // keeps its distance to the other path while moving
    CAM_OP_ACCEL_TO, // constant acceleration from the current velocity
    CAM_OP_EASE_TO,
    CAM_OP_ATTACH, // actor 0 is the caster
    CAM_OP_DETACH,
} CamOpcode;

#define CAM_EYE 0x40
#define CAM_TARGET 0x80
#define CAM_VAR(n) (-(n) - 1) // a duration read from script variable n
#define CAM_HOLD(path, frames) (path) | CAM_OP_HOLD, frames
#define CAM_SET_POS(path, x, y, z) (path) | CAM_OP_SET_POS, x, y, z
#define CAM_MOVE_TO(path, frames, x, y, z) (path) | CAM_OP_MOVE_TO, frames, x, y, z
#define CAM_DOLLY_TO(path, frames, x, y, z) (path) | CAM_OP_DOLLY_TO, frames, x, y, z
#define CAM_ACCEL_TO(path, frames, x, y, z) (path) | CAM_OP_ACCEL_TO, frames, x, y, z
#define CAM_EASE_TO(path, frames, x, y, z) (path) | CAM_OP_EASE_TO, frames, x, y, z
#define CAM_ATTACH(path, actor, part) (path) | CAM_OP_ATTACH, actor, part
#define CAM_DETACH(path) (path) | CAM_OP_DETACH
#define CAM_END -1

#endif

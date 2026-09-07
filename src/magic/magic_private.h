#ifndef MAGIC_PRIVATE_H
#define MAGIC_PRIVATE_H

// Shared by the magic overlays only; battle.h stays the public interface.

// PSX fixed point: 1.0 == 1 << FIXED_SHIFT. Angles: FIXED_ONE == a full turn.
#define FIXED_SHIFT 12
#define FIXED_ONE (1 << FIXED_SHIFT)

// Primitive buffer page. Each overlay holds two and its buffer-flip slot
// alternates between them, so the GPU can read last frame's primitives while
// this frame builds.
#define MAGIC_PAGE_SIZE 0x10000

#endif

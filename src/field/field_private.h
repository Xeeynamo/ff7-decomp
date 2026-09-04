#ifndef FIELD_PRIVATE_H
#define FIELD_PRIVATE_H

#define GET_ENTITY_NAME(entityId) ((char*)g_FieldScripts + sizeof(FieldScriptHeader) + (entityId) * 8)

#define GET_FIELD_SCRIPT_OFFSET(dst, scriptOffset, entityDataOffset, extrasHeaderSize)                                 \
    (dst) = *((u8*)((scriptOffset) + (entityDataOffset) + (extrasHeaderSize) + (s32)g_FieldScripts) +                  \
              sizeof(FieldScriptHeader));                                                                              \
    (dst) |= *((u8*)((scriptOffset) + ((entityDataOffset) + (s32)g_FieldScripts) + (extrasHeaderSize)) +               \
               sizeof(FieldScriptHeader) + 1)                                                                          \
             << 8

#endif

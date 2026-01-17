#pragma once

uint32_t fnv1a_hash(const uint8_t* data, size_t len)
{
    const uint32_t FNV_OFFSET = 2166136261u;
    const uint32_t FNV_PRIME = 16777619u;

    uint32_t hash = FNV_OFFSET;
    for (size_t i = 0; i < len; ++i)
    {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }
    return hash;
}
/*
 * Copyright (C) 2014-2018 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "IsaGenerator_Gfx9.hpp"

#include <algorithm>
#include <string>

const std::string IsaGenerator_Gfx9::ASIC_NAME = "GFX9";

/* The binaries are generated from following ISA */
#if 0
/* flat_atomic_inc will not support by some PCIE, use flat_atomic_add instead */
shader atomic_add
asic(GFX9)
type(CS)
    v_mov_b32 v0, s0
    v_mov_b32 v1, s1
    v_mov_b32 v2, 1
    flat_atomic_add v3, v[0:1], v2 slc glc
    s_waitcnt 0
    s_endpgm
end

shader copy_dword
asic(GFX9)
type(CS)
/* copy the parameters from scalar registers to vector registers */
    v_mov_b32 v0, s0
    v_mov_b32 v1, s1
    v_mov_b32 v2, s2
    v_mov_b32 v3, s3
/* copy a dword between the passed addresses */
    flat_load_dword v4, v[0:1] slc glc
    s_waitcnt 0
    flat_store_dword v[2:3], v4 slc glc
    s_endpgm
end

shader main
asic(GFX9)
type(CS)
loop:
    s_branch loop
    s_endpgm
end


#endif

const uint32_t IsaGenerator_Gfx9::NOOP_ISA[] = {
    0xbf810000
};

const uint32_t IsaGenerator_Gfx9::COPY_DWORD_ISA[] = {
    0x7e000200, 0x7e020201,
    0x7e040202, 0x7e060203,
    0xdc530000, 0x047f0000,
    0xbf8c0000, 0xdc730000,
    0x007f0402, 0xbf810000
};

const uint32_t IsaGenerator_Gfx9::INFINITE_LOOP_ISA[] = {
    0xbf82ffff, 0xbf810000
};

const uint32_t IsaGenerator_Gfx9::ATOMIC_ADD_ISA[] = {
    0x7e000200, 0x7e020201,
    0x7e040281, 0xdd0b0000,
    0x037f0200, 0xbf8c0000,
    0xbf810000, 0x00000000
};

const uint32_t IsaGenerator_Gfx9::CUSTOM_SGPR_ISA[] = {
    0x7e040202,               // v_mov_b32 v2, s2
    0x7e060203,               // v_mov_b32 v3, s3
    0x7e08020f,               // v_mov_b32 v4, s15. s15 would be 0xCAFEBABE.
    0xdc730000, 0x007f0402,   // flat_store_dword v[2:3], v4 slc glc
    0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Gfx9::SCALAR_SET_ISA[] = {
    0x7e000200,               // v_mov_b32 v0, s0
    0x7e020201,               // v_mov_b32 v1, s1
    0x7e040202,               // v_mov_b32 v2, s2
    0x7e060203,               // v_mov_b32 v3, s3
    0xdc530000, 0x067f0002,   // flat_load_dword v6, v[2:3]
    0xbf8c0000,               // s_waitcnt 0
    0x680c0d06,               // v_add_u32 v6, v6, v6
    0xdc730000, 0x007f0600,   // flat_store_dword v[0:1], v6 slc glc
    0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Gfx9::SCALAR_ADD_ISA[] = {
    0x7e000200,               // v_mov_b32 v0, s0
    0x7e020201,               // v_mov_b32 v1, s1
    0x7e040202,               // v_mov_b32 v2, s2
    0x7e060203,               // v_mov_b32 v3, s3
    0x7e080204,               // v_mov_b32 v4, s4
    0x7e0A0205,               // v_mov_b32 v5, s5
    0xdc530000, 0x067f0002,   // flat_load_dword v6, v[2:3]
    0xdc530000, 0x077f0004,   // flat_load_dword v7, v[4:5]
    0xbf8c0000,               // s_waitcnt 0
    0x680c0d07,               // v_add_u32 v6, v6, v7
    0xdc730000, 0x007f0600,   // flat_store_dword v[0:1], v6 slc glc
    0xbf810000                // s_endpgm
};

void IsaGenerator_Gfx9::GetNoopIsa(HsaMemoryBuffer& rBuf) {
    std::copy(NOOP_ISA, NOOP_ISA+ARRAY_SIZE(NOOP_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetCopyDwordIsa(HsaMemoryBuffer& rBuf) {
    std::copy(COPY_DWORD_ISA, COPY_DWORD_ISA+ARRAY_SIZE(COPY_DWORD_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetInfiniteLoopIsa(HsaMemoryBuffer& rBuf) {
    std::copy(INFINITE_LOOP_ISA, INFINITE_LOOP_ISA+ARRAY_SIZE(INFINITE_LOOP_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetAtomicIncIsa(HsaMemoryBuffer& rBuf) {
    std::copy(ATOMIC_ADD_ISA, ATOMIC_ADD_ISA+ARRAY_SIZE(ATOMIC_ADD_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetCustomSGPRIsa(HsaMemoryBuffer& rBuf) {
    std::copy(CUSTOM_SGPR_ISA, CUSTOM_SGPR_ISA+ARRAY_SIZE(CUSTOM_SGPR_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetScalarSetIsa(HsaMemoryBuffer& rBuf) {
      std::copy(SCALAR_SET_ISA, SCALAR_SET_ISA+ARRAY_SIZE(SCALAR_SET_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetScalarAddIsa(HsaMemoryBuffer& rBuf) {
      std::copy(SCALAR_ADD_ISA, SCALAR_ADD_ISA+ARRAY_SIZE(SCALAR_ADD_ISA), rBuf.As<uint32_t*>());
}

const std::string& IsaGenerator_Gfx9::GetAsicName() {
    return ASIC_NAME;
}

const uint32_t IsaGenerator_Gfx9::GEMM_ISA_16_1152_5120[] = {
      0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Gfx9::GEMM_ISA_16_5120_384[] = {
      0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Gfx9::GEMM_ISA_16_1280_5120[] = {
      0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Gfx9::GEMM_ISA_16_5120_1280[] = {
      0xbf810000                // s_endpgm
};

void IsaGenerator_Gfx9::GetGEMMIsa_16_1152_5120(HsaMemoryBuffer& rBuf) {
      std::copy(GEMM_ISA_16_1152_5120, GEMM_ISA_16_1152_5120+ARRAY_SIZE(GEMM_ISA_16_1152_5120), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetGEMMIsa_16_5120_384(HsaMemoryBuffer& rBuf) {
      std::copy(GEMM_ISA_16_5120_384, GEMM_ISA_16_5120_384+ARRAY_SIZE(GEMM_ISA_16_5120_384), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetGEMMIsa_16_1280_5120(HsaMemoryBuffer& rBuf) {
      std::copy(GEMM_ISA_16_1280_5120, GEMM_ISA_16_1280_5120+ARRAY_SIZE(GEMM_ISA_16_1280_5120), rBuf.As<uint32_t*>());
}

void IsaGenerator_Gfx9::GetGEMMIsa_16_5120_1280(HsaMemoryBuffer& rBuf) {
      std::copy(GEMM_ISA_16_5120_1280, GEMM_ISA_16_5120_1280+ARRAY_SIZE(GEMM_ISA_16_5120_1280), rBuf.As<uint32_t*>());
}

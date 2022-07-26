/*
 * Copyright (C) 2020 Advanced Micro Devices, Inc. All Rights Reserved.
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

#include "IsaGenerator_Aldebaran.hpp"

#include <algorithm>
#include <string>

const std::string IsaGenerator_Aldbrn::ASIC_NAME = "ALDEBARAN";

/* The binaries are generated from following ISA */
#if 0
/* flat_atomic_inc will not support by some PCIE, use flat_atomic_add instead */
shader atomic_add
asic(ALDEBARAN)
type(CS)
    v_mov_b32 v0, s0
    v_mov_b32 v1, s1
    v_mov_b32 v2, 1
    flat_atomic_add v3, v[0:1], v2 slc glc scc
    s_waitcnt 0
    s_endpgm
end

shader copy_dword
asic(ALDEBARAN)
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
asic(ALDEBARAN)
type(CS)
loop:
    s_branch loop
    s_endpgm
end


#endif

const uint32_t IsaGenerator_Aldbrn::NOOP_ISA[] = {
    0xbf810000
};

const uint32_t IsaGenerator_Aldbrn::COPY_DWORD_ISA[] = {
    0x7e000200, 0x7e020201,
    0x7e040202, 0x7e060203,
    0xdc530000, 0x047f0000,
    0xbf8c0000, 0xdc730000,
    0x007f0402, 0xbf810000
};

const uint32_t IsaGenerator_Aldbrn::INFINITE_LOOP_ISA[] = {
    0xbf82ffff, 0xbf810000
};

const uint32_t IsaGenerator_Aldbrn::ATOMIC_ADD_ISA[] = {
    0x7e000200, 0x7e020201,
    0x7e040281, 0xdf0b0000,
    0x037f0200, 0xbf8c0000,
    0xbf810000, 0x00000000
};

const uint32_t IsaGenerator_Aldbrn::CUSTOM_SGPR_ISA[] = {
    0x7e040202,               // v_mov_b32 v2, s2
    0x7e060203,               // v_mov_b32 v3, s3
    0x7e08020f,               // v_mov_b32 v4, s15. s15 would be 0xCAFEBABE.
    0xdc730000, 0x007f0402,   // flat_store_dword v[2:3], v4 slc glc
    0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Aldbrn::SCALAR_SET_ISA[] = {
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

const uint32_t IsaGenerator_Aldbrn::SCALAR_ADD_ISA[] = {
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

const uint32_t IsaGenerator_Aldbrn::VECTOR_SET_ISA[] = {
    0x7e080200,               // v_mov_b32 v4, s0
    0x7e0a0201,               // v_mov_b32 v5, s1

    0x68020100,               // v_add_u32 v1, v0, v0
    0x68020301,               // v_add_u32 v1, v1, v1

    0x68080901,               // v_add_u32 v4, v4, v1

    0xdc730000, 0x007f0004,   // flat_store_dword v[4:5], v0 slc glc

    0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Aldbrn::VECTOR_ADD_ISA[] = {
    0x7e040202,               // v_mov_b32 v2, s2
    0x7e060203,               // v_mov_b32 v3, s3
    0x7e080204,               // v_mov_b32 v4, s4
    0x7e0a0205,               // v_mov_b32 v5, s5
    0x7e0c0200,               // v_mov_b32 v6, s0
    0x7e0e0201,               // v_mov_b32 v7, s1

    0x68020100,               // v_add_u32 v1, v0, v0
    0x68020301,               // v_add_u32 v1, v1, v1

    0x68040501,               // v_add_u32 v2, v2, v1
    0x68080901,               // v_add_u32 v4, v4, v1
    0x680c0d01,               // v_add_u32 v6, v6, v1

    0xdc530000, 0x087f0002,   // flat_load_dword v8, v[2:3]
    0xdc530000, 0x097f0004,   // flat_load_dword v9, v[4:5]
    0xbf8c0000,               // s_waitcnt 0

    0x68101109,               // v_add_u32 v8, v8, v9

    0xdc730000, 0x007f0806,   // flat_store_dword v[6:7], v8 slc glc

    0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Aldbrn::VECTOR_GROUP_SET_ISA[] = {
    0x7e080200,               // v_mov_b32 v4, s0
    0x7e0a0201,               // v_mov_b32 v5, s1

                              // v1 = bid
    0x7e020206,               // v_mov_b32 v1, s6 TGID_X

                              // v1 <<= 2
    0x68020301,               // v_add_u32 v1, v1, v1
    0x68020301,               // v_add_u32 v1, v1, v1

                              // v1 <<= 3
    0x68020301,               // v_add_u32 v1, v1, v1
    0x68020301,               // v_add_u32 v1, v1, v1
    0x68020301,               // v_add_u32 v1, v1, v1

                              // v2 = tid << 2
    0x68040100,               // v_add_u32 v2, v0, v0
    0x68040502,               // v_add_u32 v2, v2, v2

                              // v1 = (tid << 2) + (bid << 5)
    0x68020302,               // v_add_u32 v1, v1, v2

                              // v4 = v4 + v1
    0x68080901,               // v_add_u32 v4, v4, v1

    0x7e040206,               // v_mov_b32 v2, s6 TGID_X

    0xdc730000, 0x007f0204,   // flat_store_dword v[4:5], v2 slc glc

    0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Aldbrn::VECTOR_GROUP_ADD_ISA[] = {
                              // v[4:5] = &C
    0x7e080200,               // v_mov_b32 v4, s0
    0x7e0a0201,               // v_mov_b32 v5, s1

                              // v[6:7] = &A
    0x7e0c0202,               // v_mov_b32 v6, s2
    0x7e0e0203,               // v_mov_b32 v7, s3

                              // v[8:9] = &B
    0x7e100204,               // v_mov_b32 v8, s4
    0x7e120205,               // v_mov_b32 v9, s5

                              // v1 = bid
    0x7e020206,               // v_mov_b32 v1, s6 TGID_X

                              // v1 <<= 2
    0x68020301,               // v_add_u32 v1, v1, v1
    0x68020301,               // v_add_u32 v1, v1, v1

                              // v1 <<= 3
    0x68020301,               // v_add_u32 v1, v1, v1
    0x68020301,               // v_add_u32 v1, v1, v1
    0x68020301,               // v_add_u32 v1, v1, v1

                              // v2 = tid << 2
    0x68040100,               // v_add_u32 v2, v0, v0
    0x68040502,               // v_add_u32 v2, v2, v2

                              // v1 = (tid << 2) + (bid << 5)
    0x68020302,               // v_add_u32 v1, v1, v2

                              // v4 = v4 + v1
    0x68080901,               // v_add_u32 v4, v4, v1

                              // v6 = v6 + v1
    0x680c0d01,               // v_add_u32 v6, v6, v1

                              // v8 = v8 + v1
    0x68101101,               // v_add_u32 v8, v8, v1

    0xdc530000, 0x0a7f0006,   // flat_load_dword v10, v[6:7]
    0xdc530000, 0x0b7f0008,   // flat_load_dword v11, v[8:9]
    0xbf8c0000,               // s_waitcnt 0

    0x6814150b,               // v_add_u32 v10, v10, v11

    0xdc730000, 0x007f0a04,   // flat_store_dword v[4:5], v10 slc glc

    0xbf810000                // s_endpgm
};

void IsaGenerator_Aldbrn::GetNoopIsa(HsaMemoryBuffer& rBuf) {
    std::copy(NOOP_ISA, NOOP_ISA+ARRAY_SIZE(NOOP_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetCopyDwordIsa(HsaMemoryBuffer& rBuf) {
    std::copy(COPY_DWORD_ISA, COPY_DWORD_ISA+ARRAY_SIZE(COPY_DWORD_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetInfiniteLoopIsa(HsaMemoryBuffer& rBuf) {
    std::copy(INFINITE_LOOP_ISA, INFINITE_LOOP_ISA+ARRAY_SIZE(INFINITE_LOOP_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetAtomicIncIsa(HsaMemoryBuffer& rBuf) {
    std::copy(ATOMIC_ADD_ISA, ATOMIC_ADD_ISA+ARRAY_SIZE(ATOMIC_ADD_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetCustomSGPRIsa(HsaMemoryBuffer& rBuf) {
    std::copy(CUSTOM_SGPR_ISA, CUSTOM_SGPR_ISA+ARRAY_SIZE(CUSTOM_SGPR_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetScalarSetIsa(HsaMemoryBuffer& rBuf) {
      std::copy(SCALAR_SET_ISA, SCALAR_SET_ISA+ARRAY_SIZE(SCALAR_SET_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetScalarAddIsa(HsaMemoryBuffer& rBuf) {
      std::copy(SCALAR_ADD_ISA, SCALAR_ADD_ISA+ARRAY_SIZE(SCALAR_ADD_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetVectorSetIsa(HsaMemoryBuffer& rBuf) {
      std::copy(VECTOR_SET_ISA, VECTOR_SET_ISA+ARRAY_SIZE(VECTOR_SET_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetVectorAddIsa(HsaMemoryBuffer& rBuf) {
      std::copy(VECTOR_ADD_ISA, VECTOR_ADD_ISA+ARRAY_SIZE(VECTOR_ADD_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetVectorGroupSetIsa(HsaMemoryBuffer& rBuf) {
      std::copy(VECTOR_GROUP_SET_ISA, VECTOR_GROUP_SET_ISA+ARRAY_SIZE(VECTOR_GROUP_SET_ISA), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetVectorGroupAddIsa(HsaMemoryBuffer& rBuf) {
      std::copy(VECTOR_GROUP_ADD_ISA, VECTOR_GROUP_ADD_ISA+ARRAY_SIZE(VECTOR_GROUP_ADD_ISA), rBuf.As<uint32_t*>());
}

const std::string& IsaGenerator_Aldbrn::GetAsicName() {
    return ASIC_NAME;
}

#define EMPTY_GEMM_KERNEL (0)

const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_1152_5120[] = {
#if EMPTY_GEMM_KERNEL
0xBF810000,              // s_endpgm                                                         // 000000000660:
#else
//0xC00A0302, 0x00000000, // s_load_dwordx4 s[12:15], s[4:5], 0x0                           // 000000000000:
//0xC00A0002, 0x00000010, // s_load_dwordx4 s[0:3], s[4:5], 0x10                            // 000000000008:
0x20060085,             // v_lshrrev_b32_e32 v3, 5, v0                                    // 000000000010:
0x24080082,             // v_lshlrev_b32_e32 v4, 2, v0                                    // 000000000014:
//0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000018:

0xBE8C0002,             //     s_mov_b32 s12, s2
0xBE8D0003,             //     s_mov_b32 s13, s3
0xBE8E0004,             //     s_mov_b32 s14, s4
0xBE8F0005,             //     s_mov_b32 s15, s5
0xBF800000,             // s_nop 0

0x8E038407,             // s_lshl_b32 s3, s7, 4                                           // 00000000001C:
0x20160084,             // v_lshrrev_b32_e32 v11, 4, v0                                   // 000000000020:
0x261808FF, 0x0000007C, // v_and_b32_e32 v12, 0x7c, v4                                    // 000000000024:
0x100A06FF, 0x00000900, // v_mul_u32_u24_e32 v5, 0x900, v3                                // 00000000002C:
0x8E028706,             // s_lshl_b32 s2, s6, 7                                           // 000000000034:
0x28261905,             // v_or_b32_e32 v19, v5, v12                                      // 000000000038:
0x2602008F,             // v_and_b32_e32 v1, 15, v0                                       // 00000000003C:
0x68141603,             // v_add_u32_e32 v10, s3, v11                                     // 000000000040:
0xB0031400,             // s_movk_i32 s3, 0x1400                                          // 000000000044:
0xD1FE0012, 0x02040513, // v_add_lshl_u32 v18, v19, s2, 1                                 // 000000000048:
0xD2850004, 0x0000070A, // v_mul_lo_u32 v4, v10, s3                                       // 000000000050:
0x24040281,             // v_lshlrev_b32_e32 v2, 1, v1                                    // 000000000058:
0xBE8700FF, 0x00020000, // s_mov_b32 s7, 0x20000                                          // 00000000005C:
0x28080504,             // v_or_b32_e32 v4, v4, v2                                        // 000000000064:
0xBE88000E,             // s_mov_b32 s8, s14                                              // 000000000068:
0xBE8A00FF, 0x01680000, // s_mov_b32 s10, 0x1680000                                       // 00000000006C:
0xBE89000F,             // s_mov_b32 s9, s15                                              // 000000000074:
0xBE8B0007,             // s_mov_b32 s11, s7                                              // 000000000078:
0x681A24FF, 0x00009000, // v_add_u32_e32 v13, 0x9000, v18                                 // 00000000007C:
0x24280881,             // v_lshlrev_b32_e32 v20, 1, v4                                   // 000000000084:
0xE0541000, 0x80020412, // buffer_load_dwordx2 v[4:5], v18, s[8:11], 0 offen              // 000000000088:
0xE0541900, 0x80020612, // buffer_load_dwordx2 v[6:7], v18, s[8:11], 0 offen offset:2304  // 000000000090:
0xE0541000, 0x8002080D, // buffer_load_dwordx2 v[8:9], v13, s[8:11], 0 offen              // 000000000098:
0xE0541900, 0x8002100D, // buffer_load_dwordx2 v[16:17], v13, s[8:11], 0 offen offset:2304// 0000000000A0:
0x241A0081,             // v_lshlrev_b32_e32 v13, 1, v0                                   // 0000000000A8:
0xD2010002, 0x0409410D, // v_and_or_b32 v2, v13, 32, v2                                   // 0000000000AC:
0xB00E0050,             // s_movk_i32 s14, 0x50                                           // 0000000000B4:
0x101A060E,             // v_mul_u32_u24_e32 v13, s14, v3                                 // 0000000000B8:
0x24040481,             // v_lshlrev_b32_e32 v2, 1, v2                                    // 0000000000BC:
0xB0030108,             // s_movk_i32 s3, 0x108                                           // 0000000000C0:
0xD1FD000D, 0x0409030D, // v_lshl_add_u32 v13, v13, 1, v2                                 // 0000000000C4:
0x10040603,             // v_mul_u32_u24_e32 v2, s3, v3                                   // 0000000000CC:
0x24181881,             // v_lshlrev_b32_e32 v12, 1, v12                                  // 0000000000D0:
0xD1FE0015, 0x0206050C, // v_add_lshl_u32 v21, v12, v2, 1                                 // 0000000000D4:
0x28040688,             // v_or_b32_e32 v2, 8, v3                                         // 0000000000DC:
0x10040403,             // v_mul_u32_u24_e32 v2, s3, v2                                   // 0000000000E0:
0xBE84000C,             // s_mov_b32 s4, s12                                              // 0000000000E4:
0xBE85000D,             // s_mov_b32 s5, s13                                              // 0000000000E8:
0xBE8300FF, 0x00012000, // s_mov_b32 s3, 0x12000                                          // 0000000000EC:
0xBE8600FF, 0x00050000, // s_mov_b32 s6, 0x50000                                          // 0000000000F4:
0xD1FE000C, 0x02061902, // v_add_lshl_u32 v12, v2, v12, 1                                 // 0000000000FC:
0x682C2403,             // v_add_u32_e32 v22, s3, v18                                     // 000000000104:
0x682E24FF, 0x0001B000, // v_add_u32_e32 v23, 0x1b000, v18                                // 000000000108:
0xE0501000, 0x80010214, // buffer_load_dword v2, v20, s[4:7], 0 offen                     // 000000000110:
0xE0501040, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen offset:64          // 000000000118:
0xB00D0500,             // s_movk_i32 s13, 0x500                                          // 000000000120:
0x681E180D,             // v_add_u32_e32 v15, s13, v12                                    // 000000000124:
0xD3D94004, 0x18000080, // v_accvgpr_write_b32 a4, 0                                      // 000000000128:
0x7E0AA504,             // v_accvgpr_mov_b32 a5, a4                                       // 000000000130:
0x7E0CA504,             // v_accvgpr_mov_b32 a6, a4                                       // 000000000134:
0x7E0EA504,             // v_accvgpr_mov_b32 a7, a4                                       // 000000000138:
0xD3D94000, 0x18000080, // v_accvgpr_write_b32 a0, 0                                      // 00000000013C:
0x24262681,             // v_lshlrev_b32_e32 v19, 1, v19                                  // 000000000144:
0xBE8C0080,             // s_mov_b32 s12, 0                                               // 000000000148:
0x681C2A0D,             // v_add_u32_e32 v14, s13, v21                                    // 00000000014C:
0xD1FD0013, 0x044D0202, // v_lshl_add_u32 v19, s2, 1, v19                                 // 000000000150:
0x282828FF, 0x00000080, // v_or_b32_e32 v20, 0x80, v20                                    // 000000000158:
0xBF8C0F71,             // s_waitcnt vmcnt(1)                                             // 000000000160:
0xD81A0000, 0x0000020D, // ds_write_b32 v13, v2                                           // 000000000164:
0xD2A00002, 0x00020D04, // v_pack_b32_f16 v2, v4, v6                                      // 00000000016C:
0xD2A01803, 0x00020D04, // v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                       // 000000000174:
0xD2A00004, 0x00020F05, // v_pack_b32_f16 v4, v5, v7                                      // 00000000017C:
0xD2A01805, 0x00020F05, // v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                       // 000000000184:
0xD2A00006, 0x00022108, // v_pack_b32_f16 v6, v8, v16                                     // 00000000018C:
0xD2A01807, 0x00022108, // v_pack_b32_f16 v7, v8, v16 op_sel:[1,1,0]                      // 000000000194:
0xD2A00008, 0x00022309, // v_pack_b32_f16 v8, v9, v17                                     // 00000000019C:
0xD2A01809, 0x00022309, // v_pack_b32_f16 v9, v9, v17 op_sel:[1,1,0]                      // 0000000001A4:
0xD9BE0500, 0x00000215, // ds_write_b128 v21, v[2:5] offset:1280                          // 0000000001AC:
0xD9BE0500, 0x0000060C, // ds_write_b128 v12, v[6:9] offset:1280                          // 0000000001B4:
0xE0541000, 0x80020616, // buffer_load_dwordx2 v[6:7], v22, s[8:11], 0 offen              // 0000000001BC:
0xE0541900, 0x80020816, // buffer_load_dwordx2 v[8:9], v22, s[8:11], 0 offen offset:2304  // 0000000001C4:
0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen              // 0000000001CC:
0xE0541900, 0x80020417, // buffer_load_dwordx2 v[4:5], v23, s[8:11], 0 offen offset:2304  // 0000000001D4:
0x24220085,             // v_lshlrev_b32_e32 v17, 5, v0                                   // 0000000001DC:
0xD1C8000C, 0x020D0300, // v_bfe_u32 v12, v0, 1, 3                                        // 0000000001E0:
0xD1C80010, 0x02090900, // v_bfe_u32 v16, v0, 4, 2                                        // 0000000001E8:
0x262222A0,             // v_and_b32_e32 v17, 32, v17                                     // 0000000001F0:
0xD2000011, 0x04450710, // v_lshl_or_b32 v17, v16, 3, v17                                 // 0000000001F4:
0x1018180E,             // v_mul_u32_u24_e32 v12, s14, v12                                // 0000000001FC:
0xD1FE0011, 0x02061911, // v_add_lshl_u32 v17, v17, v12, 1                                // 000000000200:
0x20180082,             // v_lshrrev_b32_e32 v12, 2, v0                                   // 000000000208:
0xD3D84017, 0x18000105, // v_accvgpr_read_b32 v23, a5                                     // 00000000020C:
0xD2010000, 0x0405610C, // v_and_or_b32 v0, v12, 48, v1                                   // 000000000214:
0x102020FF, 0x00000420, // v_mul_u32_u24_e32 v16, 0x420, v16                              // 00000000021C:
0xD1FD0010, 0x04410300, // v_lshl_add_u32 v16, v0, 1, v16                                 // 000000000224:
0xD3D94001, 0x18000117, // v_accvgpr_write_b32 a1, v23                                    // 00000000022C:
0xD3D84017, 0x18000106, // v_accvgpr_read_b32 v23, a6                                     // 000000000234:
0xD1FD0010, 0x00350310, // v_lshl_add_u32 v16, v16, 1, s13                                // 00000000023C:
0xD3D94002, 0x18000117, // v_accvgpr_write_b32 a2, v23                                    // 000000000244:
0xD3D84017, 0x18000107, // v_accvgpr_read_b32 v23, a7                                     // 00000000024C:
0x682A20A0,             // v_add_u32_e32 v21, 32, v16                                     // 000000000254:
0x682C20B0,             // v_add_u32_e32 v22, 48, v16                                     // 000000000258:
0xD3D94003, 0x18000117, // v_accvgpr_write_b32 a3, v23                                    // 00000000025C:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0) 264 <BB0_1>                               // 000000000264:
0xBF8A0000,             // s_barrier                                                      // 000000000268:
0xD9FE0000, 0x18000011, // ds_read_b128 v[24:27], v17                                     // 00000000026C:
0xD8700100, 0x24000010, // ds_read2st64_b32 v[36:37], v16 offset1:1                       // 000000000274:
0xD86EC484, 0x26000010, // ds_read2_b32 v[38:39], v16 offset0:132 offset1:196             // 00000000027C:
0xD8700504, 0x28000015, // ds_read2st64_b32 v[40:41], v21 offset0:4 offset1:5             // 000000000284:
0xD8700706, 0x2A000016, // ds_read2st64_b32 v[42:43], v22 offset0:6 offset1:7             // 00000000028C:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000294:
0xBF8A0000,             // s_barrier                                                      // 000000000298:
0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                           // 00000000029C:
0x7E400324,             // v_mov_b32_e32 v32, v36                                         // 0000000002A0:
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 0000000002A4:
0x7E420326,             // v_mov_b32_e32 v33, v38                                         // 0000000002A8:
0xBF8C0F74,             // s_waitcnt vmcnt(4)                                             // 0000000002AC:
0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                          // 0000000002B0:
0xE0501000, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen                    // 0000000002B8:
0xD3CD8004, 0x04124118, // v_mfma_f32_16x16x16f16 a[4:7], v[24:25], v[32:33], a[4:7]      // 0000000002C0:
0xBF8C0F73,             // s_waitcnt vmcnt(3)                                             // 0000000002C8:
0xD2A0001C, 0x00021106, // v_pack_b32_f16 v28, v6, v8                                     // 0000000002CC:
0xD2A0181D, 0x00021106, // v_pack_b32_f16 v29, v6, v8 op_sel:[1,1,0]                      // 0000000002D4:
0xD2A0001E, 0x00021307, // v_pack_b32_f16 v30, v7, v9                                     // 0000000002DC:
0xD2A0181F, 0x00021307, // v_pack_b32_f16 v31, v7, v9 op_sel:[1,1,0]                      // 0000000002E4:
0xBF8C0F71,             // s_waitcnt vmcnt(1)                                             // 0000000002EC:
0xD2A00020, 0x00020902, // v_pack_b32_f16 v32, v2, v4                                     // 0000000002F0:
0xD2A01821, 0x00020902, // v_pack_b32_f16 v33, v2, v4 op_sel:[1,1,0]                      // 0000000002F8:
0x6804260C,             // v_add_u32_e32 v2, s12, v19                                     // 000000000300:
0x680804FF, 0x00024000, // v_add_u32_e32 v4, 0x24000, v2                                  // 000000000304:
0x810C030C,             // s_add_i32 s12, s12, s3                                         // 00000000030C:
0xD2A00022, 0x00020B03, // v_pack_b32_f16 v34, v3, v5                                     // 000000000310:
0xD2A01823, 0x00020B03, // v_pack_b32_f16 v35, v3, v5 op_sel:[1,1,0]                      // 000000000318:
0x682E04FF, 0x0002D000, // v_add_u32_e32 v23, 0x2d000, v2                                 // 000000000320:
0xE0541000, 0x80020604, // buffer_load_dwordx2 v[6:7], v4, s[8:11], 0 offen               // 000000000328:
0xE0541900, 0x80020804, // buffer_load_dwordx2 v[8:9], v4, s[8:11], 0 offen offset:2304   // 000000000330:
0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen              // 000000000338:
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 000000000340:
0x7E0A032A,             // v_mov_b32_e32 v5, v42                                          // 000000000344:
0x7E080328,             // v_mov_b32_e32 v4, v40                                          // 000000000348:
0xBF07FF0C, 0x00B1C000, // s_cmp_lg_u32 s12, 0xb1c000                                     // 00000000034C:
0x7E4C0325,             // v_mov_b32_e32 v38, v37                                         // 000000000354:
0xD3CD8004, 0x0412091A, // v_mfma_f32_16x16x16f16 a[4:7], v[26:27], v[4:5], a[4:7]        // 000000000358:
0xE0541900, 0x80020417, // buffer_load_dwordx2 v[4:5], v23, s[8:11], 0 offen offset:2304  // 000000000360:
0x7E540329,             // v_mov_b32_e32 v42, v41                                         // 000000000368:
0x682828C0,             // v_add_u32_e32 v20, 64, v20                                     // 00000000036C:
0xD9BE0000, 0x00001C0E, // ds_write_b128 v14, v[28:31]                                    // 000000000370:
0xD9BE0000, 0x0000200F, // ds_write_b128 v15, v[32:35]                                    // 000000000378:
0xD3CD8000, 0x04024D18, // v_mfma_f32_16x16x16f16 a[0:3], v[24:25], v[38:39], a[0:3]      // 000000000380:
0xD3CD8000, 0x0402551A, // v_mfma_f32_16x16x16f16 a[0:3], v[26:27], v[42:43], a[0:3]      // 000000000388:
0xBF85FFB4,             // s_cbranch_scc1 BB0_1                                           // 000000000390:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000394:
0xBF8A0000,             // s_barrier                                                      // 000000000398:
0xD9FE0000, 0x14000011, // ds_read_b128 v[20:23], v17                                     // 00000000039C:
0xD8700100, 0x18000010, // ds_read2st64_b32 v[24:25], v16 offset1:1                       // 0000000003A4:
0xD86EC484, 0x1C000010, // ds_read2_b32 v[28:29], v16 offset0:132 offset1:196             // 0000000003AC:
0x682620A0,             // v_add_u32_e32 v19, 32, v16                                     // 0000000003B4:
0x684420B0,             // v_add_u32_e32 v34, 48, v16                                     // 0000000003B8:
0x24000082,             // v_lshlrev_b32_e32 v0, 2, v0                                    // 0000000003BC:
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 0000000003C0:
0x7E340318,             // v_mov_b32_e32 v26, v24                                         // 0000000003C4:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000003C8:
0x7E36031C,             // v_mov_b32_e32 v27, v28                                         // 0000000003CC:
0x7E380319,             // v_mov_b32_e32 v28, v25                                         // 0000000003D0:
0xBF800000,             // s_nop 0                                                        // 0000000003D4:
0xD3CD8004, 0x04123514, // v_mfma_f32_16x16x16f16 a[4:7], v[20:21], v[26:27], a[4:7]      // 0000000003D8:
0xD8700504, 0x1A000013, // ds_read2st64_b32 v[26:27], v19 offset0:4 offset1:5             // 0000000003E0:
0xD8700706, 0x1E000022, // ds_read2st64_b32 v[30:31], v34 offset0:6 offset1:7             // 0000000003E8:
0xB0030480,             // s_movk_i32 s3, 0x480                                           // 0000000003F0:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000003F4:
0xBF8A0000,             // s_barrier                                                      // 0000000003F8:
0xBF8C0F74,             // s_waitcnt vmcnt(4)                                             // 0000000003FC:
0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                          // 000000000400:
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 000000000408:
0x7E40031A,             // v_mov_b32_e32 v32, v26                                         // 00000000040C:
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 000000000410:
0x7E42031E,             // v_mov_b32_e32 v33, v30                                         // 000000000414:
0xBF8C0F72,             // s_waitcnt vmcnt(2)                                             // 000000000418:
0xD2A00018, 0x00021106, // v_pack_b32_f16 v24, v6, v8                                     // 00000000041C:
0xD2A01819, 0x00021106, // v_pack_b32_f16 v25, v6, v8 op_sel:[1,1,0]                      // 000000000424:
0x7E3C031B,             // v_mov_b32_e32 v30, v27                                         // 00000000042C:
0xD2A0001A, 0x00021307, // v_pack_b32_f16 v26, v7, v9                                     // 000000000430:
0xD2A0181B, 0x00021307, // v_pack_b32_f16 v27, v7, v9 op_sel:[1,1,0]                      // 000000000438:
0xBF8C0F70,             // s_waitcnt vmcnt(0)                                             // 000000000440:
0xD2A00006, 0x00020902, // v_pack_b32_f16 v6, v2, v4                                      // 000000000444:
0xD2A01807, 0x00020902, // v_pack_b32_f16 v7, v2, v4 op_sel:[1,1,0]                       // 00000000044C:
0xD2A00008, 0x00020B03, // v_pack_b32_f16 v8, v3, v5                                      // 000000000454:
0xD2A01809, 0x00020B03, // v_pack_b32_f16 v9, v3, v5 op_sel:[1,1,0]                       // 00000000045C:
0xD9BE0000, 0x0000180E, // ds_write_b128 v14, v[24:27]                                    // 000000000464:
0xD9BE0000, 0x0000060F, // ds_write_b128 v15, v[6:9]                                      // 00000000046C:
0xD3CD8004, 0x04124116, // v_mfma_f32_16x16x16f16 a[4:7], v[22:23], v[32:33], a[4:7]      // 000000000474:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 00000000047C:
0xBF8A0000,             // s_barrier                                                      // 000000000480:
0xD9FE0000, 0x02000011, // ds_read_b128 v[2:5], v17                                       // 000000000484:
0xD8700100, 0x06000010, // ds_read2st64_b32 v[6:7], v16 offset1:1                         // 00000000048C:
0xD86EC484, 0x08000010, // ds_read2_b32 v[8:9], v16 offset0:132 offset1:196               // 000000000494:
0xD8700504, 0x10000013, // ds_read2st64_b32 v[16:17], v19 offset0:4 offset1:5             // 00000000049C:
0xD8700706, 0x12000022, // ds_read2st64_b32 v[18:19], v34 offset0:6 offset1:7             // 0000000004A4:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000004AC:
0xBF8A0000,             // s_barrier                                                      // 0000000004B0:
0xD3CD8000, 0x04023914, // v_mfma_f32_16x16x16f16 a[0:3], v[20:21], v[28:29], a[0:3]      // 0000000004B4:
0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                           // 0000000004BC:
0x7E1C0306,             // v_mov_b32_e32 v14, v6                                          // 0000000004C0:
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 0000000004C4:
0x7E1E0308,             // v_mov_b32_e32 v15, v8                                          // 0000000004C8:
0x7E100307,             // v_mov_b32_e32 v8, v7                                           // 0000000004CC:
0x260C188C,             // v_and_b32_e32 v6, 12, v12                                      // 0000000004D0:
0xD200000C, 0x04011106, // v_lshl_or_b32 v12, v6, 8, v0                                   // 0000000004D4:
0x24000284,             // v_lshlrev_b32_e32 v0, 4, v1                                    // 0000000004DC:
0xD3CD8004, 0x04121D02, // v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[14:15], a[4:7]        // 0000000004E0:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000004E8:
0x7E1E0312,             // v_mov_b32_e32 v15, v18                                         // 0000000004EC:
0x7E1C0310,             // v_mov_b32_e32 v14, v16                                         // 0000000004F0:
0x7E240311,             // v_mov_b32_e32 v18, v17                                         // 0000000004F4:
0xD3CD8000, 0x04023D16, // v_mfma_f32_16x16x16f16 a[0:3], v[22:23], v[30:31], a[0:3]      // 0000000004F8:
0xD3CD8004, 0x04121D04, // v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[14:15], a[4:7]        // 000000000500:
0xD3CD8000, 0x04021102, // v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[8:9], a[0:3]          // 000000000508:
0xD2000002, 0x0401110B, // v_lshl_or_b32 v2, v11, 8, v0                                   // 000000000510:
0xBF800007,             // s_nop 7                                                        // 000000000518:
0xBF800000,             // s_nop 0                                                        // 00000000051C:
0xDA1A0000, 0x0000040C, // ds_write_b32 v12, a4                                           // 000000000520:
0xDA1A0100, 0x0000050C, // ds_write_b32 v12, a5 offset:256                                // 000000000528:
0xDA1A0200, 0x0000060C, // ds_write_b32 v12, a6 offset:512                                // 000000000530:
0xDA1A0300, 0x0000070C, // ds_write_b32 v12, a7 offset:768                                // 000000000538:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000540:
0xBF8A0000,             // s_barrier                                                      // 000000000544:
0xD9FE0000, 0x06000002, // ds_read_b128 v[6:9], v2                                        // 000000000548:
0xD3CD8000, 0x04022504, // v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[18:19], a[0:3]        // 000000000550:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000558:
0x7E001506,             // v_cvt_f16_f32_e32 v0, v6                                       // 00000000055C:
0x7E061508,             // v_cvt_f16_f32_e32 v3, v8                                       // 000000000560:
0x7E0C1509,             // v_cvt_f16_f32_e32 v6, v9                                       // 000000000564:
0x7E0E1507,             // v_cvt_f16_f32_e32 v7, v7                                       // 000000000568:
0x24100282,             // v_lshlrev_b32_e32 v8, 2, v1                                    // 00000000056C:
0xD2A00001, 0x00020D03, // v_pack_b32_f16 v1, v3, v6                                      // 000000000570:
0xD2850003, 0x0000070A, // v_mul_lo_u32 v3, v10, s3                                       // 000000000578:
0x68060602,             // v_add_u32_e32 v3, s2, v3                                       // 000000000580:
0xD2A00000, 0x00020F00, // v_pack_b32_f16 v0, v0, v7                                      // 000000000584:
0xBE8300FF, 0x00020000, // s_mov_b32 s3, 0x20000                                          // 00000000058C:
0xBE8200FF, 0x00012000, // s_mov_b32 s2, 0x12000                                          // 000000000594:
0xD1FE0004, 0x02061103, // v_add_lshl_u32 v4, v3, v8, 1                                   // 00000000059C:
0xE0741000, 0x80000004, // buffer_store_dwordx2 v[0:1], v4, s[0:3], 0 offen               // 0000000005A4:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005AC:
0xBF8A0000,             // s_barrier                                                      // 0000000005B0:
0xDA1A0000, 0x0000000C, // ds_write_b32 v12, a0                                           // 0000000005B4:
0xDA1A0100, 0x0000010C, // ds_write_b32 v12, a1 offset:256                                // 0000000005BC:
0xDA1A0200, 0x0000020C, // ds_write_b32 v12, a2 offset:512                                // 0000000005C4:
0xDA1A0300, 0x0000030C, // ds_write_b32 v12, a3 offset:768                                // 0000000005CC:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005D4:
0xBF8A0000,             // s_barrier                                                      // 0000000005D8:
0xD9FE0000, 0x00000002, // ds_read_b128 v[0:3], v2                                        // 0000000005DC:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005E4:
0x7E001500,             // v_cvt_f16_f32_e32 v0, v0                                       // 0000000005E8:
0x7E0A1501,             // v_cvt_f16_f32_e32 v5, v1                                       // 0000000005EC:
0x7E041502,             // v_cvt_f16_f32_e32 v2, v2                                       // 0000000005F0:
0x7E061503,             // v_cvt_f16_f32_e32 v3, v3                                       // 0000000005F4:
0xD2A00000, 0x00020B00, // v_pack_b32_f16 v0, v0, v5                                      // 0000000005F8:
0xD2A00001, 0x00020702, // v_pack_b32_f16 v1, v2, v3                                      // 000000000600:
0xE0741080, 0x80000004, // buffer_store_dwordx2 v[0:1], v4, s[0:3], 0 offen offset:128    // 000000000608:
0xBF810000              // s_endpgm                                                       // 000000000610:
#endif
};

// GEMM kernels using atomic instructions (buffer_atomic_pk_add_f16) not working for some reason.
//
//const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_1152_5120[] = {
// #if EMPTY_GEMM_KERNEL
// 0xBF810000,              // s_endpgm                                                         // 000000000660:
// #else
// // Original ISA:
// // s4-s5 : kernarg pointer
// // s6 : workgroup_id_x
// // s7 : workgroup_id_y
// // s8 : workgroup_id_z
// //0xC00A0302, 0x00000000, // s_load_dwordx4 s[12:15], s[4:5], 0x0                             // 000000000000: 
// //0xC00A0002, 0x00000010, // s_load_dwordx4 s[0:3], s[4:5], 0x10                              // 000000000008: 
// //0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000010: 
// 
// // Hacked ISA
// // s0-s1: matrix C pointer
// // s2-s3: matrix A pointer
// // s4-s5: matrix B pointer
// // s6 : workgroup_id_x
// // s7 : workgroup_id_y
// // s8 : workgroup_id_z
// 0xBE8C0002,             //     s_mov_b32 s12, s2
// 0xBE8D0003,             //     s_mov_b32 s13, s3
// 0xBE8E0004,             //     s_mov_b32 s14, s4
// 0xBE8F0005,             //     s_mov_b32 s15, s5
// 0xBF800000,             // s_nop 0
// 
// // The rest of GEMM algorithm.
// 0x8E038407,             // s_lshl_b32 s3, s7, 4                                             // 000000000014: 
// 0x20140084,             // v_lshrrev_b32_e32 v10, 4, v0                                     // 000000000018: 
// 0x24020082,             // v_lshlrev_b32_e32 v1, 2, v0                                      // 00000000001C: 
// 0x261802FF, 0x0000007C, // v_and_b32_e32 v12, 0x7c, v1                                      // 000000000020: 
// 0x68021403,             // v_add_u32_e32 v1, s3, v10                                        // 000000000028: 
// 0xB0031400,             // s_movk_i32 s3, 0x1400                                            // 00000000002C: 
// 0xD2850004, 0x00000701, // v_mul_lo_u32 v4, v1, s3                                          // 000000000030: 
// 0x8E038508,             // s_lshl_b32 s3, s8, 5                                             // 000000000038: 
// 0x2606148E,             // v_and_b32_e32 v3, 14, v10                                        // 00000000003C: 
// 0x68080803,             // v_add_u32_e32 v4, s3, v4                                         // 000000000040: 
// 0x280A0603,             // v_or_b32_e32 v5, s3, v3                                          // 000000000044: 
// 0xB0030480,             // s_movk_i32 s3, 0x480                                             // 000000000048: 
// 0xD2850005, 0x00000705, // v_mul_lo_u32 v5, v5, s3                                          // 00000000004C: 
// 0x8E028706,             // s_lshl_b32 s2, s6, 7                                             // 000000000054: 
// 0x28261905,             // v_or_b32_e32 v19, v5, v12                                        // 000000000058: 
// 0x2616008F,             // v_and_b32_e32 v11, 15, v0                                        // 00000000005C: 
// 0xD1FE0015, 0x02040513, // v_add_lshl_u32 v21, v19, s2, 1                                   // 000000000060: 
// 0x24041681,             // v_lshlrev_b32_e32 v2, 1, v11                                     // 000000000068: 
// 0xBE8700FF, 0x00020000, // s_mov_b32 s7, 0x20000                                            // 00000000006C: 
// 0x28080504,             // v_or_b32_e32 v4, v4, v2                                          // 000000000074: 
// 0xBE88000E,             // s_mov_b32 s8, s14                                                // 000000000078: 
// 0xBE8A00FF, 0x01680000, // s_mov_b32 s10, 0x1680000                                         // 00000000007C: 
// 0xBE89000F,             // s_mov_b32 s9, s15                                                // 000000000084: 
// 0xBE8B0007,             // s_mov_b32 s11, s7                                                // 000000000088: 
// 0x681A2AFF, 0x00009000, // v_add_u32_e32 v13, 0x9000, v21                                   // 00000000008C: 
// 0x24280881,             // v_lshlrev_b32_e32 v20, 1, v4                                     // 000000000094: 
// 0xE0541000, 0x80020415, // buffer_load_dwordx2 v[4:5], v21, s[8:11], 0 offen                // 000000000098: 
// 0xE0541900, 0x80020615, // buffer_load_dwordx2 v[6:7], v21, s[8:11], 0 offen offset:2304    // 0000000000A0: 
// 0xE0541000, 0x8002080D, // buffer_load_dwordx2 v[8:9], v13, s[8:11], 0 offen                // 0000000000A8: 
// 0xE0541900, 0x8002100D, // buffer_load_dwordx2 v[16:17], v13, s[8:11], 0 offen offset:2304  // 0000000000B0: 
// 0x241A0081,             // v_lshlrev_b32_e32 v13, 1, v0                                     // 0000000000B8: 
// 0xD2010002, 0x0409410D, // v_and_or_b32 v2, v13, 32, v2                                     // 0000000000BC: 
// 0x201C0085,             // v_lshrrev_b32_e32 v14, 5, v0                                     // 0000000000C4: 
// 0xB00E0050,             // s_movk_i32 s14, 0x50                                             // 0000000000C8: 
// 0x101A1C0E,             // v_mul_u32_u24_e32 v13, s14, v14                                  // 0000000000CC: 
// 0x24040481,             // v_lshlrev_b32_e32 v2, 1, v2                                      // 0000000000D0: 
// 0xB0030108,             // s_movk_i32 s3, 0x108                                             // 0000000000D4: 
// 0xD1FD000D, 0x0409030D, // v_lshl_add_u32 v13, v13, 1, v2                                   // 0000000000D8: 
// 0x10041C03,             // v_mul_u32_u24_e32 v2, s3, v14                                    // 0000000000E0: 
// 0x24181881,             // v_lshlrev_b32_e32 v12, 1, v12                                    // 0000000000E4: 
// 0xD1FE0016, 0x0206050C, // v_add_lshl_u32 v22, v12, v2, 1                                   // 0000000000E8: 
// 0x28040690,             // v_or_b32_e32 v2, 16, v3                                          // 0000000000F0: 
// 0x20040481,             // v_lshrrev_b32_e32 v2, 1, v2                                      // 0000000000F4: 
// 0x10040403,             // v_mul_u32_u24_e32 v2, s3, v2                                     // 0000000000F8: 
// 0xBE84000C,             // s_mov_b32 s4, s12                                                // 0000000000FC: 
// 0xBE85000D,             // s_mov_b32 s5, s13                                                // 000000000100: 
// 0xBE8600FF, 0x00050000, // s_mov_b32 s6, 0x50000                                            // 000000000104: 
// 0xD1FE000C, 0x02061902, // v_add_lshl_u32 v12, v2, v12, 1                                   // 00000000010C: 
// 0xE0501000, 0x80010214, // buffer_load_dword v2, v20, s[4:7], 0 offen                       // 000000000114: 
// 0xE0501200, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen offset:512           // 00000000011C: 
// 0xBE8300FF, 0x00090000, // s_mov_b32 s3, 0x90000                                            // 000000000124: 
// 0x682E2A03,             // v_add_u32_e32 v23, s3, v21                                       // 00000000012C: 
// 0x682A2AFF, 0x00099000, // v_add_u32_e32 v21, 0x99000, v21                                  // 000000000130: 
// 0xB00D0500,             // s_movk_i32 s13, 0x500                                            // 000000000138: 
// 0x681E180D,             // v_add_u32_e32 v15, s13, v12                                      // 00000000013C: 
// 0xD3D94004, 0x18000080, // v_accvgpr_write_b32 a4, 0                                        // 000000000140: 
// 0x7E0AA504,             // v_accvgpr_mov_b32 a5, a4                                         // 000000000148: 
// 0x7E0CA504,             // v_accvgpr_mov_b32 a6, a4                                         // 00000000014C: 
// 0x7E0EA504,             // v_accvgpr_mov_b32 a7, a4                                         // 000000000150: 
// 0xD3D94000, 0x18000080, // v_accvgpr_write_b32 a0, 0                                        // 000000000154: 
// 0x24262681,             // v_lshlrev_b32_e32 v19, 1, v19                                    // 00000000015C: 
// 0xBE8C0080,             // s_mov_b32 s12, 0                                                 // 000000000160: 
// 0x681C2C0D,             // v_add_u32_e32 v14, s13, v22                                      // 000000000164: 
// 0xD1FD0013, 0x044D0202, // v_lshl_add_u32 v19, s2, 1, v19                                   // 000000000168: 
// 0x682828FF, 0x00000400, // v_add_u32_e32 v20, 0x400, v20                                    // 000000000170: 
// 0xBF8C0F71,             // s_waitcnt vmcnt(1)                                               // 000000000178: 
// 0xD81A0000, 0x0000020D, // ds_write_b32 v13, v2                                             // 00000000017C: 
// 0xD2A00002, 0x00020D04, // v_pack_b32_f16 v2, v4, v6                                        // 000000000184: 
// 0xD2A01803, 0x00020D04, // v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                         // 00000000018C: 
// 0xD2A00004, 0x00020F05, // v_pack_b32_f16 v4, v5, v7                                        // 000000000194: 
// 0xD2A01805, 0x00020F05, // v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                         // 00000000019C: 
// 0xD2A00006, 0x00022108, // v_pack_b32_f16 v6, v8, v16                                       // 0000000001A4: 
// 0xD2A01807, 0x00022108, // v_pack_b32_f16 v7, v8, v16 op_sel:[1,1,0]                        // 0000000001AC: 
// 0xD2A00008, 0x00022309, // v_pack_b32_f16 v8, v9, v17                                       // 0000000001B4: 
// 0xD2A01809, 0x00022309, // v_pack_b32_f16 v9, v9, v17 op_sel:[1,1,0]                        // 0000000001BC: 
// 0xD9BE0500, 0x00000216, // ds_write_b128 v22, v[2:5] offset:1280                            // 0000000001C4: 
// 0xD9BE0500, 0x0000060C, // ds_write_b128 v12, v[6:9] offset:1280                            // 0000000001CC: 
// 0xE0541000, 0x80020617, // buffer_load_dwordx2 v[6:7], v23, s[8:11], 0 offen                // 0000000001D4: 
// 0xE0541900, 0x80020817, // buffer_load_dwordx2 v[8:9], v23, s[8:11], 0 offen offset:2304    // 0000000001DC: 
// 0xE0541000, 0x80020215, // buffer_load_dwordx2 v[2:3], v21, s[8:11], 0 offen                // 0000000001E4: 
// 0xE0541900, 0x80020415, // buffer_load_dwordx2 v[4:5], v21, s[8:11], 0 offen offset:2304    // 0000000001EC: 
// 0x24220085,             // v_lshlrev_b32_e32 v17, 5, v0                                     // 0000000001F4: 
// 0xD1C8000C, 0x020D0300, // v_bfe_u32 v12, v0, 1, 3                                          // 0000000001F8: 
// 0xD1C80010, 0x02090900, // v_bfe_u32 v16, v0, 4, 2                                          // 000000000200: 
// 0x262222A0,             // v_and_b32_e32 v17, 32, v17                                       // 000000000208: 
// 0xD2000011, 0x04450710, // v_lshl_or_b32 v17, v16, 3, v17                                   // 00000000020C: 
// 0x1018180E,             // v_mul_u32_u24_e32 v12, s14, v12                                  // 000000000214: 
// 0xD1FE0011, 0x02061911, // v_add_lshl_u32 v17, v17, v12, 1                                  // 000000000218: 
// 0x20180082,             // v_lshrrev_b32_e32 v12, 2, v0                                     // 000000000220: 
// 0xD3D84017, 0x18000105, // v_accvgpr_read_b32 v23, a5                                       // 000000000224: 
// 0xD2010000, 0x042D610C, // v_and_or_b32 v0, v12, 48, v11                                    // 00000000022C: 
// 0x102020FF, 0x00000420, // v_mul_u32_u24_e32 v16, 0x420, v16                                // 000000000234: 
// 0xD1FD0010, 0x04410300, // v_lshl_add_u32 v16, v0, 1, v16                                   // 00000000023C: 
// 0xD3D94001, 0x18000117, // v_accvgpr_write_b32 a1, v23                                      // 000000000244: 
// 0xD3D84017, 0x18000106, // v_accvgpr_read_b32 v23, a6                                       // 00000000024C: 
// 0xD1FD0010, 0x00350310, // v_lshl_add_u32 v16, v16, 1, s13                                  // 000000000254: 
// 0xD3D94002, 0x18000117, // v_accvgpr_write_b32 a2, v23                                      // 00000000025C: 
// 0xD3D84017, 0x18000107, // v_accvgpr_read_b32 v23, a7                                       // 000000000264: 
// 0x682A20A0,             // v_add_u32_e32 v21, 32, v16                                       // 00000000026C: 
// 0x682C20B0,             // v_add_u32_e32 v22, 48, v16                                       // 000000000270: 
// 0xD3D94003, 0x18000117, // v_accvgpr_write_b32 a3, v23                                      // 000000000274: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0) // 27c <BB0_1>                              // 00000000027C: 
// 0xBF8A0000,             // s_barrier                                                        // 000000000280: 
// 0xD9FE0000, 0x18000011, // ds_read_b128 v[24:27], v17                                       // 000000000284: 
// 0xD8700100, 0x24000010, // ds_read2st64_b32 v[36:37], v16 offset1:1                         // 00000000028C: 
// 0xD86EC484, 0x26000010, // ds_read2_b32 v[38:39], v16 offset0:132 offset1:196               // 000000000294: 
// 0xD8700504, 0x28000015, // ds_read2st64_b32 v[40:41], v21 offset0:4 offset1:5               // 00000000029C: 
// 0xD8700706, 0x2A000016, // ds_read2st64_b32 v[42:43], v22 offset0:6 offset1:7               // 0000000002A4: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000002AC: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000002B0: 
// 0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                             // 0000000002B4: 
// 0x7E400324,             // v_mov_b32_e32 v32, v36                                           // 0000000002B8: 
// 0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                             // 0000000002BC: 
// 0x7E420326,             // v_mov_b32_e32 v33, v38                                           // 0000000002C0: 
// 0xBF8C0F74,             // s_waitcnt vmcnt(4)                                               // 0000000002C4: 
// 0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                            // 0000000002C8: 
// 0xE0501000, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen                      // 0000000002D0: 
// 0xD3CD8004, 0x04124118, // v_mfma_f32_16x16x16f16 a[4:7], v[24:25], v[32:33], a[4:7]        // 0000000002D8: 
// 0xBF8C0F73,             // s_waitcnt vmcnt(3)                                               // 0000000002E0: 
// 0xD2A0001C, 0x00021106, // v_pack_b32_f16 v28, v6, v8                                       // 0000000002E4: 
// 0xD2A0181D, 0x00021106, // v_pack_b32_f16 v29, v6, v8 op_sel:[1,1,0]                        // 0000000002EC: 
// 0xD2A0001E, 0x00021307, // v_pack_b32_f16 v30, v7, v9                                       // 0000000002F4: 
// 0xD2A0181F, 0x00021307, // v_pack_b32_f16 v31, v7, v9 op_sel:[1,1,0]                        // 0000000002FC: 
// 0xBF8C0F71,             // s_waitcnt vmcnt(1)                                               // 000000000304: 
// 0xD2A00020, 0x00020902, // v_pack_b32_f16 v32, v2, v4                                       // 000000000308: 
// 0xD2A01821, 0x00020902, // v_pack_b32_f16 v33, v2, v4 op_sel:[1,1,0]                        // 000000000310: 
// 0x6804260C,             // v_add_u32_e32 v2, s12, v19                                       // 000000000318: 
// 0x680804FF, 0x00120000, // v_add_u32_e32 v4, 0x120000, v2                                   // 00000000031C: 
// 0x810C030C,             // s_add_i32 s12, s12, s3                                           // 000000000324: 
// 0xD2A00022, 0x00020B03, // v_pack_b32_f16 v34, v3, v5                                       // 000000000328: 
// 0xD2A01823, 0x00020B03, // v_pack_b32_f16 v35, v3, v5 op_sel:[1,1,0]                        // 000000000330: 
// 0x682E04FF, 0x00129000, // v_add_u32_e32 v23, 0x129000, v2                                  // 000000000338: 
// 0xE0541000, 0x80020604, // buffer_load_dwordx2 v[6:7], v4, s[8:11], 0 offen                 // 000000000340: 
// 0xE0541900, 0x80020804, // buffer_load_dwordx2 v[8:9], v4, s[8:11], 0 offen offset:2304     // 000000000348: 
// 0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen                // 000000000350: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 000000000358: 
// 0x7E0A032A,             // v_mov_b32_e32 v5, v42                                            // 00000000035C: 
// 0x7E080328,             // v_mov_b32_e32 v4, v40                                            // 000000000360: 
// 0xBF07FF0C, 0x00A20000, // s_cmp_lg_u32 s12, 0xa20000                                       // 000000000364: 
// 0x7E4C0325,             // v_mov_b32_e32 v38, v37                                           // 00000000036C: 
// 0xD3CD8004, 0x0412091A, // v_mfma_f32_16x16x16f16 a[4:7], v[26:27], v[4:5], a[4:7]          // 000000000370: 
// 0xE0541900, 0x80020417, // buffer_load_dwordx2 v[4:5], v23, s[8:11], 0 offen offset:2304    // 000000000378: 
// 0x7E540329,             // v_mov_b32_e32 v42, v41                                           // 000000000380: 
// 0x682828FF, 0x00000200, // v_add_u32_e32 v20, 0x200, v20                                    // 000000000384: 
// 0xD9BE0000, 0x00001C0E, // ds_write_b128 v14, v[28:31]                                      // 00000000038C: 
// 0xD9BE0000, 0x0000200F, // ds_write_b128 v15, v[32:35]                                      // 000000000394: 
// 0xD3CD8000, 0x04024D18, // v_mfma_f32_16x16x16f16 a[0:3], v[24:25], v[38:39], a[0:3]        // 00000000039C: 
// 0xD3CD8000, 0x0402551A, // v_mfma_f32_16x16x16f16 a[0:3], v[26:27], v[42:43], a[0:3]        // 0000000003A4: 
// 0xBF85FFB3,             // s_cbranch_scc1 BB0_1                                             // 0000000003AC: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000003B0: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000003B4: 
// 0xD9FE0000, 0x14000011, // ds_read_b128 v[20:23], v17                                       // 0000000003B8: 
// 0xD8700100, 0x18000010, // ds_read2st64_b32 v[24:25], v16 offset1:1                         // 0000000003C0: 
// 0xD86EC484, 0x1C000010, // ds_read2_b32 v[28:29], v16 offset0:132 offset1:196               // 0000000003C8: 
// 0x682620A0,             // v_add_u32_e32 v19, 32, v16                                       // 0000000003D0: 
// 0x684420B0,             // v_add_u32_e32 v34, 48, v16                                       // 0000000003D4: 
// 0x24000082,             // v_lshlrev_b32_e32 v0, 2, v0                                      // 0000000003D8: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 0000000003DC: 
// 0x7E340318,             // v_mov_b32_e32 v26, v24                                           // 0000000003E0: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000003E4: 
// 0x7E36031C,             // v_mov_b32_e32 v27, v28                                           // 0000000003E8: 
// 0x7E380319,             // v_mov_b32_e32 v28, v25                                           // 0000000003EC: 
// 0xBF800000,             // s_nop 0                                                          // 0000000003F0: 
// 0xD3CD8004, 0x04123514, // v_mfma_f32_16x16x16f16 a[4:7], v[20:21], v[26:27], a[4:7]        // 0000000003F4: 
// 0xD8700504, 0x1A000013, // ds_read2st64_b32 v[26:27], v19 offset0:4 offset1:5               // 0000000003FC: 
// 0xD8700706, 0x1E000022, // ds_read2st64_b32 v[30:31], v34 offset0:6 offset1:7               // 000000000404: 
// 0xB0030480,             // s_movk_i32 s3, 0x480                                             // 00000000040C: 
// 0xD2850001, 0x00000701, // v_mul_lo_u32 v1, v1, s3                                          // 000000000410: 
// 0x68020202,             // v_add_u32_e32 v1, s2, v1                                         // 000000000418: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 00000000041C: 
// 0x7E40031A,             // v_mov_b32_e32 v32, v26                                           // 000000000420: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000424: 
// 0x7E42031E,             // v_mov_b32_e32 v33, v30                                           // 000000000428: 
// 0xBE8300FF, 0x00020000, // s_mov_b32 s3, 0x20000                                            // 00000000042C: 
// 0xBE8200FF, 0x00012000, // s_mov_b32 s2, 0x12000                                            // 000000000434: 
// 0xD3CD8004, 0x04124116, // v_mfma_f32_16x16x16f16 a[4:7], v[22:23], v[32:33], a[4:7]        // 00000000043C: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000444: 
// 0xBF8A0000,             // s_barrier                                                        // 000000000448: 
// 0xBF8C0F74,             // s_waitcnt vmcnt(4)                                               // 00000000044C: 
// 0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                            // 000000000450: 
// 0xBF8C0F72,             // s_waitcnt vmcnt(2)                                               // 000000000458: 
// 0xD2A00018, 0x00021106, // v_pack_b32_f16 v24, v6, v8                                       // 00000000045C: 
// 0xD2A01819, 0x00021106, // v_pack_b32_f16 v25, v6, v8 op_sel:[1,1,0]                        // 000000000464: 
// 0x7E3C031B,             // v_mov_b32_e32 v30, v27                                           // 00000000046C: 
// 0xD2A0001A, 0x00021307, // v_pack_b32_f16 v26, v7, v9                                       // 000000000470: 
// 0xD2A0181B, 0x00021307, // v_pack_b32_f16 v27, v7, v9 op_sel:[1,1,0]                        // 000000000478: 
// 0xBF8C0F70,             // s_waitcnt vmcnt(0)                                               // 000000000480: 
// 0xD2A00006, 0x00020902, // v_pack_b32_f16 v6, v2, v4                                        // 000000000484: 
// 0xD2A01807, 0x00020902, // v_pack_b32_f16 v7, v2, v4 op_sel:[1,1,0]                         // 00000000048C: 
// 0xD2A00008, 0x00020B03, // v_pack_b32_f16 v8, v3, v5                                        // 000000000494: 
// 0xD2A01809, 0x00020B03, // v_pack_b32_f16 v9, v3, v5 op_sel:[1,1,0]                         // 00000000049C: 
// 0xD9BE0000, 0x0000180E, // ds_write_b128 v14, v[24:27]                                      // 0000000004A4: 
// 0xD9BE0000, 0x0000060F, // ds_write_b128 v15, v[6:9]                                        // 0000000004AC: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000004B4: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000004B8: 
// 0xD9FE0000, 0x02000011, // ds_read_b128 v[2:5], v17                                         // 0000000004BC: 
// 0xD8700100, 0x06000010, // ds_read2st64_b32 v[6:7], v16 offset1:1                           // 0000000004C4: 
// 0xD86EC484, 0x08000010, // ds_read2_b32 v[8:9], v16 offset0:132 offset1:196                 // 0000000004CC: 
// 0xD8700504, 0x10000013, // ds_read2st64_b32 v[16:17], v19 offset0:4 offset1:5               // 0000000004D4: 
// 0xD8700706, 0x12000022, // ds_read2st64_b32 v[18:19], v34 offset0:6 offset1:7               // 0000000004DC: 
// 0xD3CD8000, 0x04023914, // v_mfma_f32_16x16x16f16 a[0:3], v[20:21], v[28:29], a[0:3]        // 0000000004E4: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000004EC: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000004F0: 
// 0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                             // 0000000004F4: 
// 0x7E1C0306,             // v_mov_b32_e32 v14, v6                                            // 0000000004F8: 
// 0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                             // 0000000004FC: 
// 0x7E1E0308,             // v_mov_b32_e32 v15, v8                                            // 000000000500: 
// 0x7E100307,             // v_mov_b32_e32 v8, v7                                             // 000000000504: 
// 0x260C188C,             // v_and_b32_e32 v6, 12, v12                                        // 000000000508: 
// 0xD2000000, 0x04011106, // v_lshl_or_b32 v0, v6, 8, v0                                      // 00000000050C: 
// 0x24181682,             // v_lshlrev_b32_e32 v12, 2, v11                                    // 000000000514: 
// 0xD3CD8004, 0x04121D02, // v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[14:15], a[4:7]          // 000000000518: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000520: 
// 0x7E1E0312,             // v_mov_b32_e32 v15, v18                                           // 000000000524: 
// 0x7E1C0310,             // v_mov_b32_e32 v14, v16                                           // 000000000528: 
// 0x7E240311,             // v_mov_b32_e32 v18, v17                                           // 00000000052C: 
// 0xD3CD8000, 0x04023D16, // v_mfma_f32_16x16x16f16 a[0:3], v[22:23], v[30:31], a[0:3]        // 000000000530: 
// 0xD3CD8004, 0x04121D04, // v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[14:15], a[4:7]          // 000000000538: 
// 0xD3CD8000, 0x04021102, // v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[8:9], a[0:3]            // 000000000540: 
// 0x24041684,             // v_lshlrev_b32_e32 v2, 4, v11                                     // 000000000548: 
// 0xD2000002, 0x0409110A, // v_lshl_or_b32 v2, v10, 8, v2                                     // 00000000054C: 
// 0xBF800007,             // s_nop 7                                                          // 000000000554: 
// 0xDA1A0000, 0x00000400, // ds_write_b32 v0, a4                                              // 000000000558: 
// 0xDA1A0100, 0x00000500, // ds_write_b32 v0, a5 offset:256                                   // 000000000560: 
// 0xDA1A0200, 0x00000600, // ds_write_b32 v0, a6 offset:512                                   // 000000000568: 
// 0xDA1A0300, 0x00000700, // ds_write_b32 v0, a7 offset:768                                   // 000000000570: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000578: 
// 0xBF8A0000,             // s_barrier                                                        // 00000000057C: 
// 0xD9FE0000, 0x06000002, // ds_read_b128 v[6:9], v2                                          // 000000000580: 
// 0xD3CD8000, 0x04022504, // v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[18:19], a[0:3]          // 000000000588: 
// 0xD1FE0004, 0x02061901, // v_add_lshl_u32 v4, v1, v12, 1                                    // 000000000590: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000598: 
// 0x7E061506,             // v_cvt_f16_f32_e32 v3, v6                                         // 00000000059C: 
// 0x7E0C1507,             // v_cvt_f16_f32_e32 v6, v7                                         // 0000000005A0: 
// 0x7E0E1508,             // v_cvt_f16_f32_e32 v7, v8                                         // 0000000005A4: 
// 0x7E101509,             // v_cvt_f16_f32_e32 v8, v9                                         // 0000000005A8: 
// 0xD2A00001, 0x00020D03, // v_pack_b32_f16 v1, v3, v6                                        // 0000000005AC: 
// 0xE1381000, 0x80000104, // buffer_atomic_pk_add_f16 v1, v4, s[0:3], 0 offen                 // 0000000005B4: 
// 0xD2A00001, 0x00021107, // v_pack_b32_f16 v1, v7, v8                                        // 0000000005BC: 
// 0xE1381000, 0x84000104, // buffer_atomic_pk_add_f16 v1, v4, s[0:3], 4 offen                 // 0000000005C4: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000005CC: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000005D0: 
// 0xBF800000,             // s_nop 0                                                          // 0000000005D4: 
// 0xDA1A0000, 0x00000000, // ds_write_b32 v0, a0                                              // 0000000005D8: 
// 0xDA1A0100, 0x00000100, // ds_write_b32 v0, a1 offset:256                                   // 0000000005E0: 
// 0xDA1A0200, 0x00000200, // ds_write_b32 v0, a2 offset:512                                   // 0000000005E8: 
// 0xDA1A0300, 0x00000300, // ds_write_b32 v0, a3 offset:768                                   // 0000000005F0: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000005F8: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000005FC: 
// 0xD9FE0000, 0x00000002, // ds_read_b128 v[0:3], v2                                          // 000000000600: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000608: 
// 0x7E001500,             // v_cvt_f16_f32_e32 v0, v0                                         // 00000000060C: 
// 0x7E021501,             // v_cvt_f16_f32_e32 v1, v1                                         // 000000000610: 
// 0x7E041502,             // v_cvt_f16_f32_e32 v2, v2                                         // 000000000614: 
// 0x7E061503,             // v_cvt_f16_f32_e32 v3, v3                                         // 000000000618: 
// 0xD2A00000, 0x00020300, // v_pack_b32_f16 v0, v0, v1                                        // 00000000061C: 
// 0xE1381080, 0x80000004, // buffer_atomic_pk_add_f16 v0, v4, s[0:3], 0 offen offset:128      // 000000000624: 
// 0xD2A00000, 0x00020702, // v_pack_b32_f16 v0, v2, v3                                        // 00000000062C: 
// 0xE1381080, 0x84000004, // buffer_atomic_pk_add_f16 v0, v4, s[0:3], 4 offen offset:128      // 000000000634: 
// 
// // test write output.
// 0xD2A00004, 0x00020702, // v_pack_b32_f16 v4, v2, v3                                        // 00000000062C: 
//     0x7e000200,               // v_mov_b32 v0, s0
//     0x7e020201,               // v_mov_b32 v1, s1
//     0x7e040202,               // v_mov_b32 v2, s2
//     0x7e060203,               // v_mov_b32 v3, s3
//     //0xdc730000, 0x007f0200,   // flat_store_dword v[0:1], v2 slc glc
//     0xdc730000, 0x007f0300,   // flat_store_dword v[0:1], v3 slc glc
// 
// 0xBF810000              // s_endpgm                                                         // 00000000063C: 
// #endif
// };

const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_5120_384[] = {
#if EMPTY_GEMM_KERNEL
0xBF810000,              // s_endpgm                                                         // 000000000660:
#else
//0xC00A0302, 0x00000000, // s_load_dwordx4 s[12:15], s[4:5], 0x0                           // 000000000000:
//0xC00A0002, 0x00000010, // s_load_dwordx4 s[0:3], s[4:5], 0x10                            // 000000000008:
0x20060085,             // v_lshrrev_b32_e32 v3, 5, v0                                    // 000000000010:
0x24080082,             // v_lshlrev_b32_e32 v4, 2, v0                                    // 000000000014:
//0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000018:

0xBE8C0002,             //     s_mov_b32 s12, s2
0xBE8D0003,             //     s_mov_b32 s13, s3
0xBE8E0004,             //     s_mov_b32 s14, s4
0xBE8F0005,             //     s_mov_b32 s15, s5
0xBF800000,             // s_nop 0

0x8E038407,             // s_lshl_b32 s3, s7, 4                                           // 00000000001C:
0x20160084,             // v_lshrrev_b32_e32 v11, 4, v0                                   // 000000000020:
0x261A08FF, 0x0000007C, // v_and_b32_e32 v13, 0x7c, v4                                    // 000000000024:
0x100A06FF, 0x00002800, // v_mul_u32_u24_e32 v5, 0x2800, v3                               // 00000000002C:
0x8E028706,             // s_lshl_b32 s2, s6, 7                                           // 000000000034:
0x28261B05,             // v_or_b32_e32 v19, v5, v13                                      // 000000000038:
0x2602008F,             // v_and_b32_e32 v1, 15, v0                                       // 00000000003C:
0x68141603,             // v_add_u32_e32 v10, s3, v11                                     // 000000000040:
0xB0030180,             // s_movk_i32 s3, 0x180                                           // 000000000044:
0xD1FE0012, 0x02040513, // v_add_lshl_u32 v18, v19, s2, 1                                 // 000000000048:
0xD2850004, 0x0000070A, // v_mul_lo_u32 v4, v10, s3                                       // 000000000050:
0x24040281,             // v_lshlrev_b32_e32 v2, 1, v1                                    // 000000000058:
0xBE8700FF, 0x00020000, // s_mov_b32 s7, 0x20000                                          // 00000000005C:
0x28080504,             // v_or_b32_e32 v4, v4, v2                                        // 000000000064:
0xBE88000E,             // s_mov_b32 s8, s14                                              // 000000000068:
0xBE8A00FF, 0x00780000, // s_mov_b32 s10, 0x780000                                        // 00000000006C:
0xBE89000F,             // s_mov_b32 s9, s15                                              // 000000000074:
0xBE8B0007,             // s_mov_b32 s11, s7                                              // 000000000078:
0x681824FF, 0x00002000, // v_add_u32_e32 v12, 0x2000, v18                                 // 00000000007C:
0x24280881,             // v_lshlrev_b32_e32 v20, 1, v4                                   // 000000000084:
0x681C24FF, 0x00028000, // v_add_u32_e32 v14, 0x28000, v18                                // 000000000088:
0x681E24FF, 0x0002A000, // v_add_u32_e32 v15, 0x2a000, v18                                // 000000000090:
0xE0541000, 0x80020412, // buffer_load_dwordx2 v[4:5], v18, s[8:11], 0 offen              // 000000000098:
0xE0541800, 0x8002060C, // buffer_load_dwordx2 v[6:7], v12, s[8:11], 0 offen offset:2048  // 0000000000A0:
0xE0541000, 0x8002080E, // buffer_load_dwordx2 v[8:9], v14, s[8:11], 0 offen              // 0000000000A8:
0xE0541800, 0x8002100F, // buffer_load_dwordx2 v[16:17], v15, s[8:11], 0 offen offset:2048// 0000000000B0:
0x24180081,             // v_lshlrev_b32_e32 v12, 1, v0                                   // 0000000000B8:
0xD2010002, 0x0409410C, // v_and_or_b32 v2, v12, 32, v2                                   // 0000000000BC:
0xB00E0050,             // s_movk_i32 s14, 0x50                                           // 0000000000C4:
0x1018060E,             // v_mul_u32_u24_e32 v12, s14, v3                                 // 0000000000C8:
0x24040481,             // v_lshlrev_b32_e32 v2, 1, v2                                    // 0000000000CC:
0xB0030108,             // s_movk_i32 s3, 0x108                                           // 0000000000D0:
0xD1FD000C, 0x0409030C, // v_lshl_add_u32 v12, v12, 1, v2                                 // 0000000000D4:
0x10040603,             // v_mul_u32_u24_e32 v2, s3, v3                                   // 0000000000DC:
0x241A1A81,             // v_lshlrev_b32_e32 v13, 1, v13                                  // 0000000000E0:
0xD1FE0015, 0x0206050D, // v_add_lshl_u32 v21, v13, v2, 1                                 // 0000000000E4:
0x28040688,             // v_or_b32_e32 v2, 8, v3                                         // 0000000000EC:
0x10040403,             // v_mul_u32_u24_e32 v2, s3, v2                                   // 0000000000F0:
0xBE84000C,             // s_mov_b32 s4, s12                                              // 0000000000F4:
0xBE85000D,             // s_mov_b32 s5, s13                                              // 0000000000F8:
0xBE8300FF, 0x00050000, // s_mov_b32 s3, 0x50000                                          // 0000000000FC:
0xB0066000,             // s_movk_i32 s6, 0x6000                                          // 000000000104:
0xD1FE000D, 0x02061B02, // v_add_lshl_u32 v13, v2, v13, 1                                 // 000000000108:
0x682C2403,             // v_add_u32_e32 v22, s3, v18                                     // 000000000110:
0x682E24FF, 0x00052000, // v_add_u32_e32 v23, 0x52000, v18                                // 000000000114:
0x683024FF, 0x00078000, // v_add_u32_e32 v24, 0x78000, v18                                // 00000000011C:
0x683224FF, 0x0007A000, // v_add_u32_e32 v25, 0x7a000, v18                                // 000000000124:
0xE0501000, 0x80010214, // buffer_load_dword v2, v20, s[4:7], 0 offen                     // 00000000012C:
0xE0501040, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen offset:64          // 000000000134:
0xB00D0500,             // s_movk_i32 s13, 0x500                                          // 00000000013C:
0x681E1A0D,             // v_add_u32_e32 v15, s13, v13                                    // 000000000140:
0xD3D94004, 0x18000080, // v_accvgpr_write_b32 a4, 0                                      // 000000000144:
0x7E0AA504,             // v_accvgpr_mov_b32 a5, a4                                       // 00000000014C:
0x7E0CA504,             // v_accvgpr_mov_b32 a6, a4                                       // 000000000150:
0x7E0EA504,             // v_accvgpr_mov_b32 a7, a4                                       // 000000000154:
0xD3D94000, 0x18000080, // v_accvgpr_write_b32 a0, 0                                      // 000000000158:
0x24262681,             // v_lshlrev_b32_e32 v19, 1, v19                                  // 000000000160:
0xBE8C0080,             // s_mov_b32 s12, 0                                               // 000000000164:
0x681C2A0D,             // v_add_u32_e32 v14, s13, v21                                    // 000000000168:
0xD1FD0013, 0x044D0202, // v_lshl_add_u32 v19, s2, 1, v19                                 // 00000000016C:
0x282828FF, 0x00000080, // v_or_b32_e32 v20, 0x80, v20                                    // 000000000174:
0xBF8C0F71,             // s_waitcnt vmcnt(1)                                             // 00000000017C:
0xD81A0000, 0x0000020C, // ds_write_b32 v12, v2                                           // 000000000180:
0xD2A00002, 0x00020D04, // v_pack_b32_f16 v2, v4, v6                                      // 000000000188:
0xD2A01803, 0x00020D04, // v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                       // 000000000190:
0xD2A00004, 0x00020F05, // v_pack_b32_f16 v4, v5, v7                                      // 000000000198:
0xD2A01805, 0x00020F05, // v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                       // 0000000001A0:
0xD2A00006, 0x00022108, // v_pack_b32_f16 v6, v8, v16                                     // 0000000001A8:
0xD2A01807, 0x00022108, // v_pack_b32_f16 v7, v8, v16 op_sel:[1,1,0]                      // 0000000001B0:
0xD2A00008, 0x00022309, // v_pack_b32_f16 v8, v9, v17                                     // 0000000001B8:
0xD2A01809, 0x00022309, // v_pack_b32_f16 v9, v9, v17 op_sel:[1,1,0]                      // 0000000001C0:
0xD9BE0500, 0x00000215, // ds_write_b128 v21, v[2:5] offset:1280                          // 0000000001C8:
0xD9BE0500, 0x0000060D, // ds_write_b128 v13, v[6:9] offset:1280                          // 0000000001D0:
0xE0541000, 0x80020616, // buffer_load_dwordx2 v[6:7], v22, s[8:11], 0 offen              // 0000000001D8:
0xE0541800, 0x80020817, // buffer_load_dwordx2 v[8:9], v23, s[8:11], 0 offen offset:2048  // 0000000001E0:
0xE0541000, 0x80020218, // buffer_load_dwordx2 v[2:3], v24, s[8:11], 0 offen              // 0000000001E8:
0xE0541800, 0x80020419, // buffer_load_dwordx2 v[4:5], v25, s[8:11], 0 offen offset:2048  // 0000000001F0:
0x24220085,             // v_lshlrev_b32_e32 v17, 5, v0                                   // 0000000001F8:
0xD1C8000D, 0x020D0300, // v_bfe_u32 v13, v0, 1, 3                                        // 0000000001FC:
0xD1C80010, 0x02090900, // v_bfe_u32 v16, v0, 4, 2                                        // 000000000204:
0x262222A0,             // v_and_b32_e32 v17, 32, v17                                     // 00000000020C:
0xD2000011, 0x04450710, // v_lshl_or_b32 v17, v16, 3, v17                                 // 000000000210:
0x101A1A0E,             // v_mul_u32_u24_e32 v13, s14, v13                                // 000000000218:
0xD1FE0011, 0x02061B11, // v_add_lshl_u32 v17, v17, v13, 1                                // 00000000021C:
0x201A0082,             // v_lshrrev_b32_e32 v13, 2, v0                                   // 000000000224:
0xD3D84017, 0x18000105, // v_accvgpr_read_b32 v23, a5                                     // 000000000228:
0xD2010000, 0x0405610D, // v_and_or_b32 v0, v13, 48, v1                                   // 000000000230:
0x102020FF, 0x00000420, // v_mul_u32_u24_e32 v16, 0x420, v16                              // 000000000238:
0xD1FD0010, 0x04410300, // v_lshl_add_u32 v16, v0, 1, v16                                 // 000000000240:
0xD3D94001, 0x18000117, // v_accvgpr_write_b32 a1, v23                                    // 000000000248:
0xD3D84017, 0x18000106, // v_accvgpr_read_b32 v23, a6                                     // 000000000250:
0xD1FD0010, 0x00350310, // v_lshl_add_u32 v16, v16, 1, s13                                // 000000000258:
0xD3D94002, 0x18000117, // v_accvgpr_write_b32 a2, v23                                    // 000000000260:
0xD3D84017, 0x18000107, // v_accvgpr_read_b32 v23, a7                                     // 000000000268:
0x682A20A0,             // v_add_u32_e32 v21, 32, v16                                     // 000000000270:
0x682C20B0,             // v_add_u32_e32 v22, 48, v16                                     // 000000000274:
0xD3D94003, 0x18000117, // v_accvgpr_write_b32 a3, v23                                    // 000000000278:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0) <BB0_1>                                   // 000000000280:
0xBF8A0000,             // s_barrier                                                      // 000000000284:
0xD9FE0000, 0x18000011, // ds_read_b128 v[24:27], v17                                     // 000000000288:
0xD8700100, 0x24000010, // ds_read2st64_b32 v[36:37], v16 offset1:1                       // 000000000290:
0xD86EC484, 0x26000010, // ds_read2_b32 v[38:39], v16 offset0:132 offset1:196             // 000000000298:
0xD8700504, 0x28000015, // ds_read2st64_b32 v[40:41], v21 offset0:4 offset1:5             // 0000000002A0:
0xD8700706, 0x2A000016, // ds_read2st64_b32 v[42:43], v22 offset0:6 offset1:7             // 0000000002A8:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000002B0:
0xBF8A0000,             // s_barrier                                                      // 0000000002B4:
0xBF8C0F74,             // s_waitcnt vmcnt(4)                                             // 0000000002B8:
0xD81A0000, 0x0000120C, // ds_write_b32 v12, v18                                          // 0000000002BC:
0xE0501000, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen                    // 0000000002C4:
0xBF8C0F73,             // s_waitcnt vmcnt(3)                                             // 0000000002CC:
0xD2A0001C, 0x00021106, // v_pack_b32_f16 v28, v6, v8                                     // 0000000002D0:
0xD2A0181D, 0x00021106, // v_pack_b32_f16 v29, v6, v8 op_sel:[1,1,0]                      // 0000000002D8:
0xD2A0001E, 0x00021307, // v_pack_b32_f16 v30, v7, v9                                     // 0000000002E0:
0xD2A0181F, 0x00021307, // v_pack_b32_f16 v31, v7, v9 op_sel:[1,1,0]                      // 0000000002E8:
0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                           // 0000000002F0:
0x7E0E0326,             // v_mov_b32_e32 v7, v38                                          // 0000000002F4:
0x7E0C0324,             // v_mov_b32_e32 v6, v36                                          // 0000000002F8:
0xBF8C0F71,             // s_waitcnt vmcnt(1)                                             // 0000000002FC:
0xD2A00020, 0x00020902, // v_pack_b32_f16 v32, v2, v4                                     // 000000000300:
0xD2A01821, 0x00020902, // v_pack_b32_f16 v33, v2, v4 op_sel:[1,1,0]                      // 000000000308:
0x6804260C,             // v_add_u32_e32 v2, s12, v19                                     // 000000000310:
0xD2A00022, 0x00020B03, // v_pack_b32_f16 v34, v3, v5                                     // 000000000314:
0xD2A01823, 0x00020B03, // v_pack_b32_f16 v35, v3, v5 op_sel:[1,1,0]                      // 00000000031C:
0x7E4C0325,             // v_mov_b32_e32 v38, v37                                         // 000000000324:
0xD3CD8004, 0x04120D18, // v_mfma_f32_16x16x16f16 a[4:7], v[24:25], v[6:7], a[4:7]        // 000000000328:
0x810C030C,             // s_add_i32 s12, s12, s3                                         // 000000000330:
0x680804FF, 0x000A0000, // v_add_u32_e32 v4, 0xa0000, v2                                  // 000000000334:
0x680A04FF, 0x000A2000, // v_add_u32_e32 v5, 0xa2000, v2                                  // 00000000033C:
0x682E04FF, 0x000C8000, // v_add_u32_e32 v23, 0xc8000, v2                                 // 000000000344:
0xBF07FF0C, 0x00320000, // s_cmp_lg_u32 s12, 0x320000                                     // 00000000034C:
0x682828C0,             // v_add_u32_e32 v20, 64, v20                                     // 000000000354:
0xD3CD8000, 0x04024D18, // v_mfma_f32_16x16x16f16 a[0:3], v[24:25], v[38:39], a[0:3]      // 000000000358:
0x683004FF, 0x000CA000, // v_add_u32_e32 v24, 0xca000, v2                                 // 000000000360:
0xE0541000, 0x80020604, // buffer_load_dwordx2 v[6:7], v4, s[8:11], 0 offen               // 000000000368:
0xE0541800, 0x80020805, // buffer_load_dwordx2 v[8:9], v5, s[8:11], 0 offen offset:2048   // 000000000370:
0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen              // 000000000378:
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 000000000380:
0x7E0A032A,             // v_mov_b32_e32 v5, v42                                          // 000000000384:
0x7E080328,             // v_mov_b32_e32 v4, v40                                          // 000000000388:
0x7E540329,             // v_mov_b32_e32 v42, v41                                         // 00000000038C:
0xD9BE0000, 0x00001C0E, // ds_write_b128 v14, v[28:31]                                    // 000000000390:
0xD3CD8004, 0x0412091A, // v_mfma_f32_16x16x16f16 a[4:7], v[26:27], v[4:5], a[4:7]        // 000000000398:
0xE0541800, 0x80020418, // buffer_load_dwordx2 v[4:5], v24, s[8:11], 0 offen offset:2048  // 0000000003A0:
0xD9BE0000, 0x0000200F, // ds_write_b128 v15, v[32:35]                                    // 0000000003A8:
0xD3CD8000, 0x0402551A, // v_mfma_f32_16x16x16f16 a[0:3], v[26:27], v[42:43], a[0:3]      // 0000000003B0:
0xBF85FFB1,             // s_cbranch_scc1 BB0_1                                           // 0000000003B8:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000003BC:
0xBF8A0000,             // s_barrier                                                      // 0000000003C0:
0xD9FE0000, 0x14000011, // ds_read_b128 v[20:23], v17                                     // 0000000003C4:
0xD8700100, 0x18000010, // ds_read2st64_b32 v[24:25], v16 offset1:1                       // 0000000003CC:
0xD86EC484, 0x1C000010, // ds_read2_b32 v[28:29], v16 offset0:132 offset1:196             // 0000000003D4:
0x682620A0,             // v_add_u32_e32 v19, 32, v16                                     // 0000000003DC:
0x684420B0,             // v_add_u32_e32 v34, 48, v16                                     // 0000000003E0:
0x24000082,             // v_lshlrev_b32_e32 v0, 2, v0                                    // 0000000003E4:
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 0000000003E8:
0x7E340318,             // v_mov_b32_e32 v26, v24                                         // 0000000003EC:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000003F0:
0x7E36031C,             // v_mov_b32_e32 v27, v28                                         // 0000000003F4:
0x7E380319,             // v_mov_b32_e32 v28, v25                                         // 0000000003F8:
0xBF800000,             // s_nop 0                                                        // 0000000003FC:
0xD3CD8004, 0x04123514, // v_mfma_f32_16x16x16f16 a[4:7], v[20:21], v[26:27], a[4:7]      // 000000000400:
0xD8700504, 0x1A000013, // ds_read2st64_b32 v[26:27], v19 offset0:4 offset1:5             // 000000000408:
0xD8700706, 0x1E000022, // ds_read2st64_b32 v[30:31], v34 offset0:6 offset1:7             // 000000000410:
0xB0031400,             // s_movk_i32 s3, 0x1400                                          // 000000000418:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 00000000041C:
0xBF8A0000,             // s_barrier                                                      // 000000000420:
0xBF8C0F74,             // s_waitcnt vmcnt(4)                                             // 000000000424:
0xD81A0000, 0x0000120C, // ds_write_b32 v12, v18                                          // 000000000428:
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 000000000430:
0x7E40031A,             // v_mov_b32_e32 v32, v26                                         // 000000000434:
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 000000000438:
0x7E42031E,             // v_mov_b32_e32 v33, v30                                         // 00000000043C:
0xBF8C0F72,             // s_waitcnt vmcnt(2)                                             // 000000000440:
0xD2A00018, 0x00021106, // v_pack_b32_f16 v24, v6, v8                                     // 000000000444:
0xD2A01819, 0x00021106, // v_pack_b32_f16 v25, v6, v8 op_sel:[1,1,0]                      // 00000000044C:
0x7E3C031B,             // v_mov_b32_e32 v30, v27                                         // 000000000454:
0xD2A0001A, 0x00021307, // v_pack_b32_f16 v26, v7, v9                                     // 000000000458:
0xD2A0181B, 0x00021307, // v_pack_b32_f16 v27, v7, v9 op_sel:[1,1,0]                      // 000000000460:
0xBF8C0F70,             // s_waitcnt vmcnt(0)                                             // 000000000468:
0xD2A00006, 0x00020902, // v_pack_b32_f16 v6, v2, v4                                      // 00000000046C:
0xD2A01807, 0x00020902, // v_pack_b32_f16 v7, v2, v4 op_sel:[1,1,0]                       // 000000000474:
0xD2A00008, 0x00020B03, // v_pack_b32_f16 v8, v3, v5                                      // 00000000047C:
0xD2A01809, 0x00020B03, // v_pack_b32_f16 v9, v3, v5 op_sel:[1,1,0]                       // 000000000484:
0xD9BE0000, 0x0000180E, // ds_write_b128 v14, v[24:27]                                    // 00000000048C:
0xD9BE0000, 0x0000060F, // ds_write_b128 v15, v[6:9]                                      // 000000000494:
0xD3CD8004, 0x04124116, // v_mfma_f32_16x16x16f16 a[4:7], v[22:23], v[32:33], a[4:7]      // 00000000049C:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000004A4:
0xBF8A0000,             // s_barrier                                                      // 0000000004A8:
0xD9FE0000, 0x02000011, // ds_read_b128 v[2:5], v17                                       // 0000000004AC:
0xD8700100, 0x06000010, // ds_read2st64_b32 v[6:7], v16 offset1:1                         // 0000000004B4:
0xD86EC484, 0x08000010, // ds_read2_b32 v[8:9], v16 offset0:132 offset1:196               // 0000000004BC:
0xD8700504, 0x10000013, // ds_read2st64_b32 v[16:17], v19 offset0:4 offset1:5             // 0000000004C4:
0xD8700706, 0x12000022, // ds_read2st64_b32 v[18:19], v34 offset0:6 offset1:7             // 0000000004CC:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000004D4:
0xBF8A0000,             // s_barrier                                                      // 0000000004D8:
0xD3CD8000, 0x04023914, // v_mfma_f32_16x16x16f16 a[0:3], v[20:21], v[28:29], a[0:3]      // 0000000004DC:
0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                           // 0000000004E4:
0x7E1C0306,             // v_mov_b32_e32 v14, v6                                          // 0000000004E8:
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 0000000004EC:
0x7E1E0308,             // v_mov_b32_e32 v15, v8                                          // 0000000004F0:
0x7E100307,             // v_mov_b32_e32 v8, v7                                           // 0000000004F4:
0x260C1A8C,             // v_and_b32_e32 v6, 12, v13                                      // 0000000004F8:
0xD200000C, 0x04011106, // v_lshl_or_b32 v12, v6, 8, v0                                   // 0000000004FC:
0x24000284,             // v_lshlrev_b32_e32 v0, 4, v1                                    // 000000000504:
0xD3CD8004, 0x04121D02, // v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[14:15], a[4:7]        // 000000000508:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000510:
0x7E1E0312,             // v_mov_b32_e32 v15, v18                                         // 000000000514:
0x7E1C0310,             // v_mov_b32_e32 v14, v16                                         // 000000000518:
0x7E240311,             // v_mov_b32_e32 v18, v17                                         // 00000000051C:
0xD3CD8000, 0x04023D16, // v_mfma_f32_16x16x16f16 a[0:3], v[22:23], v[30:31], a[0:3]      // 000000000520:
0xD3CD8004, 0x04121D04, // v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[14:15], a[4:7]        // 000000000528:
0xD3CD8000, 0x04021102, // v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[8:9], a[0:3]          // 000000000530:
0xD2000002, 0x0401110B, // v_lshl_or_b32 v2, v11, 8, v0                                   // 000000000538:
0xBF800007,             // s_nop 7                                                        // 000000000540:
0xBF800000,             // s_nop 0                                                        // 000000000544:
0xDA1A0000, 0x0000040C, // ds_write_b32 v12, a4                                           // 000000000548:
0xDA1A0100, 0x0000050C, // ds_write_b32 v12, a5 offset:256                                // 000000000550:
0xDA1A0200, 0x0000060C, // ds_write_b32 v12, a6 offset:512                                // 000000000558:
0xDA1A0300, 0x0000070C, // ds_write_b32 v12, a7 offset:768                                // 000000000560:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000568:
0xBF8A0000,             // s_barrier                                                      // 00000000056C:
0xD9FE0000, 0x06000002, // ds_read_b128 v[6:9], v2                                        // 000000000570:
0xD3CD8000, 0x04022504, // v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[18:19], a[0:3]        // 000000000578:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000580:
0x7E001506,             // v_cvt_f16_f32_e32 v0, v6                                       // 000000000584:
0x7E061508,             // v_cvt_f16_f32_e32 v3, v8                                       // 000000000588:
0x7E0C1509,             // v_cvt_f16_f32_e32 v6, v9                                       // 00000000058C:
0x7E0E1507,             // v_cvt_f16_f32_e32 v7, v7                                       // 000000000590:
0x24100282,             // v_lshlrev_b32_e32 v8, 2, v1                                    // 000000000594:
0xD2A00001, 0x00020D03, // v_pack_b32_f16 v1, v3, v6                                      // 000000000598:
0xD2850003, 0x0000070A, // v_mul_lo_u32 v3, v10, s3                                       // 0000000005A0:
0x68060602,             // v_add_u32_e32 v3, s2, v3                                       // 0000000005A8:
0xD2A00000, 0x00020F00, // v_pack_b32_f16 v0, v0, v7                                      // 0000000005AC:
0xBE8300FF, 0x00020000, // s_mov_b32 s3, 0x20000                                          // 0000000005B4:
0xBE8200FF, 0x00050000, // s_mov_b32 s2, 0x50000                                          // 0000000005BC:
0xD1FE0004, 0x02061103, // v_add_lshl_u32 v4, v3, v8, 1                                   // 0000000005C4:
0xE0741000, 0x80000004, // buffer_store_dwordx2 v[0:1], v4, s[0:3], 0 offen               // 0000000005CC:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005D4:
0xBF8A0000,             // s_barrier                                                      // 0000000005D8:
0xDA1A0000, 0x0000000C, // ds_write_b32 v12, a0                                           // 0000000005DC:
0xDA1A0100, 0x0000010C, // ds_write_b32 v12, a1 offset:256                                // 0000000005E4:
0xDA1A0200, 0x0000020C, // ds_write_b32 v12, a2 offset:512                                // 0000000005EC:
0xDA1A0300, 0x0000030C, // ds_write_b32 v12, a3 offset:768                                // 0000000005F4:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005FC:
0xBF8A0000,             // s_barrier                                                      // 000000000600:
0xD9FE0000, 0x00000002, // ds_read_b128 v[0:3], v2                                        // 000000000604:
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 00000000060C:
0x7E001500,             // v_cvt_f16_f32_e32 v0, v0                                       // 000000000610:
0x7E0A1501,             // v_cvt_f16_f32_e32 v5, v1                                       // 000000000614:
0x7E041502,             // v_cvt_f16_f32_e32 v2, v2                                       // 000000000618:
0x7E061503,             // v_cvt_f16_f32_e32 v3, v3                                       // 00000000061C:
0xD2A00000, 0x00020B00, // v_pack_b32_f16 v0, v0, v5                                      // 000000000620:
0xD2A00001, 0x00020702, // v_pack_b32_f16 v1, v2, v3                                      // 000000000628:
0xE0741080, 0x80000004, // buffer_store_dwordx2 v[0:1], v4, s[0:3], 0 offen offset:128    // 000000000630:
0xBF810000              // s_endpgm                                                       // 000000000638:
#endif
};

// GEMM kernels using atomic instructions (buffer_atomic_pk_add_f16) not working for some reason.
//
// const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_5120_384[] = {
// #if EMPTY_GEMM_KERNEL
// 0xBF810000,              // s_endpgm                                                         // 000000000660:
// #else
//  // Original ISA:
//  // s4-s5 : kernarg pointer
//  // s6 : workgroup_id_x
//  // s7 : workgroup_id_y
//  // s8 : workgroup_id_z
//  // 0xC00A0402, 0x00000000, //	s_load_dwordx4 s[16:19], s[4:5], 0x0                       
//  // 0xC00A0002, 0x00000010, //	s_load_dwordx4 s[0:3], s[4:5], 0x10                        
//  // 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
// 
//  // Hacked ISA
//  // s0-s1: matrix C pointer
//  // s2-s3: matrix A pointer
//  // s4-s5: matrix B pointer
//  // s6 : workgroup_id_x
//  // s7 : workgroup_id_y
//  // s8 : workgroup_id_z
//  0xBE900002,             //     s_mov_b32 s16, s2
//  0xBE910003,             //     s_mov_b32 s17, s3
//  0xBE920004,		 //     s_mov_b32 s18, s4
//  0xBE930005,		 //     s_mov_b32 s19, s5
//  0xBF800000,             // s_nop 0
// 
//  // The rest of GEMM algorithm.
//  0x8E028407,             //	s_lshl_b32 s2, s7, 4                                       
//  0x20300084,             //	v_lshrrev_b32_e32 v24, 4, v0                               
//  0x68343002,             //	v_add_u32_e32 v26, s2, v24                                 
//  0xB0020180,             //	s_movk_i32 s2, 0x180                                       
//  0xD2850004, 0x0000051A, //	v_mul_lo_u32 v4, v26, s2                                   
//  0x8E028508,             //	s_lshl_b32 s2, s8, 5                                       
//  0x2604308E,             //	v_and_b32_e32 v2, 14, v24                                  
//  0x24060082,             //	v_lshlrev_b32_e32 v3, 2, v0                                
//  0x280A0402,             //	v_or_b32_e32 v5, s2, v2                                    
//  0xB00D1400,             //	s_movk_i32 s13, 0x1400                                     
//  0x260606FF, 0x0000007C, //	v_and_b32_e32 v3, 0x7c, v3                                 
//  0xD2850005, 0x00001B05, //	v_mul_lo_u32 v5, v5, s13                                   
//  0x8E0C8706,             //	s_lshl_b32 s12, s6, 7                                      
//  0x2632008F,             //	v_and_b32_e32 v25, 15, v0                                  
//  0x280A0705,             //	v_or_b32_e32 v5, v5, v3                                    
//  0xD1FE0015, 0x02041905, //	v_add_lshl_u32 v21, v5, s12, 1                             
//  0x68080802,             //	v_add_u32_e32 v4, s2, v4                                   
//  0x24023281,             //	v_lshlrev_b32_e32 v1, 1, v25                               
//  0xBE8700FF, 0x00020000, //	s_mov_b32 s7, 0x20000                                      
//  0x28080304,             //	v_or_b32_e32 v4, v4, v1                                    
//  0xBE8A00FF, 0x00780000, //	s_mov_b32 s10, 0x780000                                    
//  0xBE880012,             //	s_mov_b32 s8, s18                                          
//  0xBE890013,             //	s_mov_b32 s9, s19                                          
//  0xBE8B0007,             //	s_mov_b32 s11, s7                                          
//  0x68182AFF, 0x00002000, //	v_add_u32_e32 v12, 0x2000, v21                             
//  0x24280881,             //	v_lshlrev_b32_e32 v20, 1, v4                               
//  0x681A2AFF, 0x00028000, //	v_add_u32_e32 v13, 0x28000, v21                            
//  0x681C2AFF, 0x0002A000, //	v_add_u32_e32 v14, 0x2a000, v21                            
//  0xE0541000, 0x80020415, //	buffer_load_dwordx2 v[4:5], v21, s[8:11], 0 offen          
//  0xE0541800, 0x8002060C, //	buffer_load_dwordx2 v[6:7], v12, s[8:11], 0 offen offset:2048
//  0xE0541000, 0x8002080D, //	buffer_load_dwordx2 v[8:9], v13, s[8:11], 0 offen          
//  0xE0541800, 0x80020A0E, //	buffer_load_dwordx2 v[10:11], v14, s[8:11], 0 offen offset:2048
//  0x24180081,             //	v_lshlrev_b32_e32 v12, 1, v0                               
//  0xD2010001, 0x0405410C, //	v_and_or_b32 v1, v12, 32, v1                               
//  0x201A0085,             //	v_lshrrev_b32_e32 v13, 5, v0                               
//  0xB0030050,             //	s_movk_i32 s3, 0x50                                        
//  0x24240681,             //	v_lshlrev_b32_e32 v18, 1, v3                               
//  0xD1C80003, 0x020D0300, //	v_bfe_u32 v3, v0, 1, 3                                     
//  0x10181A03,             //	v_mul_u32_u24_e32 v12, s3, v13                             
//  0x24020281,             //	v_lshlrev_b32_e32 v1, 1, v1                                
//  0xB0020108,             //	s_movk_i32 s2, 0x108                                       
//  0xD1FD001B, 0x0405030C, //	v_lshl_add_u32 v27, v12, 1, v1                             
//  0x10021A02,             //	v_mul_u32_u24_e32 v1, s2, v13                              
//  0x10260603,             //	v_mul_u32_u24_e32 v19, s3, v3                              
//  0x24060085,             //	v_lshlrev_b32_e32 v3, 5, v0                                
//  0xD1FE001C, 0x02060312, //	v_add_lshl_u32 v28, v18, v1, 1                             
//  0x28020490,             //	v_or_b32_e32 v1, 16, v2                                    
//  0xD1C80002, 0x02090900, //	v_bfe_u32 v2, v0, 4, 2                                     
//  0x260606A0,             //	v_and_b32_e32 v3, 32, v3                                   
//  0xB0066000,             //	s_movk_i32 s6, 0x6000                                      
//  0xBE840010,             //	s_mov_b32 s4, s16                                          
//  0xBE850011,             //	s_mov_b32 s5, s17                                          
//  0xD2000016, 0x040D0702, //	v_lshl_or_b32 v22, v2, 3, v3                               
//  0x103C04FF, 0x00000420, //	v_mul_u32_u24_e32 v30, 0x420, v2                           
//  0xE0501000, 0x80010214, //	buffer_load_dword v2, v20, s[4:7], 0 offen                 
//  0xE0501100, 0x80011F14, //	buffer_load_dword v31, v20, s[4:7], 0 offen offset:256     
//  0x20060082,             //	v_lshrrev_b32_e32 v3, 2, v0                                
//  0x68402AFF, 0x00140000, //	v_add_u32_e32 v32, 0x140000, v21                           
//  0xD201001D, 0x04656103, //	v_and_or_b32 v29, v3, 48, v25                              
//  0x68422AFF, 0x00142000, //	v_add_u32_e32 v33, 0x142000, v21                           
//  0x68442AFF, 0x00168000, //	v_add_u32_e32 v34, 0x168000, v21                           
//  0x68462AFF, 0x0016A000, //	v_add_u32_e32 v35, 0x16a000, v21                           
//  0x20020281,             //	v_lshrrev_b32_e32 v1, 1, v1                                
//  0x242E3A81,             //	v_lshlrev_b32_e32 v23, 1, v29                              
//  0xD1FE001E, 0x02063D17, //	v_add_lshl_u32 v30, v23, v30, 1                            
//  0x10020202,             //	v_mul_u32_u24_e32 v1, s2, v1                               
//  0x68483CB0,             //	v_add_u32_e32 v36, 48, v30                                 
//  0x684A2AFF, 0x00282000, //	v_add_u32_e32 v37, 0x282000, v21                           
//  0x684C2AFF, 0x002A8000, //	v_add_u32_e32 v38, 0x2a8000, v21                           
//  0x684E2AFF, 0x002AA000, //	v_add_u32_e32 v39, 0x2aa000, v21                           
//  0xBF8C0F71,             //	s_waitcnt vmcnt(1)                                         
//  0xD81A0000, 0x0000021B, //	ds_write_b32 v27, v2                                       
//  0xD2A00002, 0x00020D04, //	v_pack_b32_f16 v2, v4, v6                                  
//  0xD2A01803, 0x00020D04, //	v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                   
//  0xD2A00004, 0x00020F05, //	v_pack_b32_f16 v4, v5, v7                                  
//  0xD2A01805, 0x00020F05, //	v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                   
//  0xD2A00006, 0x00021508, //	v_pack_b32_f16 v6, v8, v10                                 
//  0xD2A01807, 0x00021508, //	v_pack_b32_f16 v7, v8, v10 op_sel:[1,1,0]                  
//  0xD2A00008, 0x00021709, //	v_pack_b32_f16 v8, v9, v11                                 
//  0xD2A01809, 0x00021709, //	v_pack_b32_f16 v9, v9, v11 op_sel:[1,1,0]                  
//  0xE0541000, 0x80020A20, //	buffer_load_dwordx2 v[10:11], v32, s[8:11], 0 offen        
//  0xE0541800, 0x80020C21, //	buffer_load_dwordx2 v[12:13], v33, s[8:11], 0 offen offset:2048
//  0xE0541000, 0x80020E22, //	buffer_load_dwordx2 v[14:15], v34, s[8:11], 0 offen        
//  0xE0541800, 0x80021023, //	buffer_load_dwordx2 v[16:17], v35, s[8:11], 0 offen offset:2048
//  0xD1FE0020, 0x02062716, //	v_add_lshl_u32 v32, v22, v19, 1                            
//  0xD1FE0022, 0x02062501, //	v_add_lshl_u32 v34, v1, v18, 1                             
//  0xD9BE0500, 0x0000021C, //	ds_write_b128 v28, v[2:5] offset:1280                      
//  0xD9BE0500, 0x00000622, //	ds_write_b128 v34, v[6:9] offset:1280                      
//  0x68423C90,             //	v_add_u32_e32 v33, 16, v30                                 
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xD9FE0000, 0x02000020, //	ds_read_b128 v[2:5], v32                                   
//  0xD8700605, 0x0600001E, //	ds_read2st64_b32 v[6:7], v30 offset0:5 offset1:6           
//  0xD8700807, 0x08000021, //	ds_read2st64_b32 v[8:9], v33 offset0:7 offset1:8           
//  0x68463CA0,             //	v_add_u32_e32 v35, 32, v30                                 
//  0x68022AFF, 0x00280000, //	v_add_u32_e32 v1, 0x280000, v21                            
//  0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
//  0x7E240306,             //	v_mov_b32_e32 v18, v6                                      
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0x7E260308,             //	v_mov_b32_e32 v19, v8                                      
//  0x7E100307,             //	v_mov_b32_e32 v8, v7                                       
//  0xBF800000,             //	s_nop 0                                                    
//  0xD3CD8000, 0x02022502, //	v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[18:19], 0         
//  0xBE8200FF, 0x00050000, //	s_mov_b32 s2, 0x50000                                      
//  0xBE830007,             //	s_mov_b32 s3, s7                                           
//  0xD3CD8004, 0x02021102, //	v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[8:9], 0           
//  0xD8700A09, 0x02000023, //	ds_read2st64_b32 v[2:3], v35 offset0:9 offset1:10          
//  0xD8700C0B, 0x12000024, //	ds_read2st64_b32 v[18:19], v36 offset0:11 offset1:12       
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xBF8C0F74,             //	s_waitcnt vmcnt(4)                                         
//  0xD81A0000, 0x00001F1B, //	ds_write_b32 v27, v31                                      
//  0xE0501200, 0x80011F14, //	buffer_load_dword v31, v20, s[4:7], 0 offen offset:512     
//  0xBF8CC27F,             //	s_waitcnt lgkmcnt(2)                                       
//  0x7E0C0302,             //	v_mov_b32_e32 v6, v2                                       
//  0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
//  0x7E0E0312,             //	v_mov_b32_e32 v7, v18                                      
//  0xB0040C00,             //	s_movk_i32 s4, 0xc00                                       
//  0x7E240303,             //	v_mov_b32_e32 v18, v3                                      
//  0xD3CD8000, 0x04020D04, //	v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[6:7], a[0:3]      
//  0xBF800000,             //	s_nop 0                                                    
//  0xD3CD8004, 0x04122504, //	v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[18:19], a[4:7]    
//  0xBF8C0F73,             //	s_waitcnt vmcnt(3)                                         
//  0xD2A00006, 0x0002190A, //	v_pack_b32_f16 v6, v10, v12                                
//  0xD2A01807, 0x0002190A, //	v_pack_b32_f16 v7, v10, v12 op_sel:[1,1,0]                 
//  0xD2A00008, 0x00021B0B, //	v_pack_b32_f16 v8, v11, v13                                
//  0xD2A01809, 0x00021B0B, //	v_pack_b32_f16 v9, v11, v13 op_sel:[1,1,0]                 
//  0xBF8C0F71,             //	s_waitcnt vmcnt(1)                                         
//  0xD2A0000A, 0x0002210E, //	v_pack_b32_f16 v10, v14, v16                               
//  0xD2A0180B, 0x0002210E, //	v_pack_b32_f16 v11, v14, v16 op_sel:[1,1,0]                
//  0xD2A0000C, 0x0002230F, //	v_pack_b32_f16 v12, v15, v17                               
//  0xD2A0180D, 0x0002230F, //	v_pack_b32_f16 v13, v15, v17 op_sel:[1,1,0]                
//  0xE0541000, 0x80020E01, //	buffer_load_dwordx2 v[14:15], v1, s[8:11], 0 offen         
//  0xE0541800, 0x80021025, //	buffer_load_dwordx2 v[16:17], v37, s[8:11], 0 offen offset:2048
//  0xE0541000, 0x80021426, //	buffer_load_dwordx2 v[20:21], v38, s[8:11], 0 offen        
//  0xE0541800, 0x80021627, //	buffer_load_dwordx2 v[22:23], v39, s[8:11], 0 offen offset:2048
//  0xD9BE0500, 0x0000061C, //	ds_write_b128 v28, v[6:9] offset:1280                      
//  0xD9BE0500, 0x00000A22, //	ds_write_b128 v34, v[10:13] offset:1280                    
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xD9FE0000, 0x02000020, //	ds_read_b128 v[2:5], v32                                   
//  0xD8700605, 0x0600001E, //	ds_read2st64_b32 v[6:7], v30 offset0:5 offset1:6           
//  0xD8700807, 0x08000021, //	ds_read2st64_b32 v[8:9], v33 offset0:7 offset1:8           
//  0x24183284,             //	v_lshlrev_b32_e32 v12, 4, v25                              
//  0x241A3A82,             //	v_lshlrev_b32_e32 v13, 2, v29                              
//  0xD200000C, 0x04311118, //	v_lshl_or_b32 v12, v24, 8, v12                             
//  0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
//  0x7E140306,             //	v_mov_b32_e32 v10, v6                                      
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0x7E160308,             //	v_mov_b32_e32 v11, v8                                      
//  0x7E100307,             //	v_mov_b32_e32 v8, v7                                       
//  0xBF800000,             //	s_nop 0                                                    
//  0xD3CD8000, 0x04021502, //	v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[10:11], a[0:3]    
//  0x24140086,             //	v_lshlrev_b32_e32 v10, 6, v0                               
//  0x24163282,             //	v_lshlrev_b32_e32 v11, 2, v25                              
//  0xD201000A, 0x0434090A, //	v_and_or_b32 v10, v10, s4, v13                             
//  0xD3CD8004, 0x04121102, //	v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[8:9], a[4:7]      
//  0xD8700A09, 0x00000023, //	ds_read2st64_b32 v[0:1], v35 offset0:9 offset1:10          
//  0xD8700C0B, 0x02000024, //	ds_read2st64_b32 v[2:3], v36 offset0:11 offset1:12         
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xBF8C0F74,             //	s_waitcnt vmcnt(4)                                         
//  0xD81A0000, 0x00001F1B, //	ds_write_b32 v27, v31                                      
//  0xBF8CC27F,             //	s_waitcnt lgkmcnt(2)                                       
//  0x7E0C0300,             //	v_mov_b32_e32 v6, v0                                       
//  0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
//  0x7E0E0302,             //	v_mov_b32_e32 v7, v2                                       
//  0x7E040301,             //	v_mov_b32_e32 v2, v1                                       
//  0xBF800000,             //	s_nop 0                                                    
//  0xD3CD8000, 0x04020D04, //	v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[6:7], a[0:3]      
//  0xD3CD8004, 0x04120504, //	v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[2:3], a[4:7]      
//  0xBF8C0F72,             //	s_waitcnt vmcnt(2)                                         
//  0xD2A00000, 0x0002210E, //	v_pack_b32_f16 v0, v14, v16                                
//  0xD2A01801, 0x0002210E, //	v_pack_b32_f16 v1, v14, v16 op_sel:[1,1,0]                 
//  0xD2A00002, 0x0002230F, //	v_pack_b32_f16 v2, v15, v17                                
//  0xD2A01803, 0x0002230F, //	v_pack_b32_f16 v3, v15, v17 op_sel:[1,1,0]                 
//  0xBF8C0F70,             //	s_waitcnt vmcnt(0)                                         
//  0xD2A00004, 0x00022D14, //	v_pack_b32_f16 v4, v20, v22                                
//  0xD2A01805, 0x00022D14, //	v_pack_b32_f16 v5, v20, v22 op_sel:[1,1,0]                 
//  0xD2A00006, 0x00022F15, //	v_pack_b32_f16 v6, v21, v23                                
//  0xD2A01807, 0x00022F15, //	v_pack_b32_f16 v7, v21, v23 op_sel:[1,1,0]                 
//  0xD9BE0500, 0x0000001C, //	ds_write_b128 v28, v[0:3] offset:1280                      
//  0xD9BE0500, 0x00000422, //	ds_write_b128 v34, v[4:7] offset:1280                      
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xD9FE0000, 0x00000020, //	ds_read_b128 v[0:3], v32                                   
//  0xD8700605, 0x0400001E, //	ds_read2st64_b32 v[4:5], v30 offset0:5 offset1:6           
//  0xD8700807, 0x06000021, //	ds_read2st64_b32 v[6:7], v33 offset0:7 offset1:8           
//  0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
//  0x7E100304,             //	v_mov_b32_e32 v8, v4                                       
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0x7E120306,             //	v_mov_b32_e32 v9, v6                                       
//  0x7E0C0305,             //	v_mov_b32_e32 v6, v5                                       
//  0xD2850004, 0x00001B1A, //	v_mul_lo_u32 v4, v26, s13                                  
//  0xD3CD8000, 0x04021100, //	v_mfma_f32_16x16x16f16 a[0:3], v[0:1], v[8:9], a[0:3]      
//  0xD8700A09, 0x08000023, //	ds_read2st64_b32 v[8:9], v35 offset0:9 offset1:10          
//  0x6808080C,             //	v_add_u32_e32 v4, s12, v4                                  
//  0xD1FE000B, 0x02061704, //	v_add_lshl_u32 v11, v4, v11, 1                             
//  0xD3CD8004, 0x04120D00, //	v_mfma_f32_16x16x16f16 a[4:7], v[0:1], v[6:7], a[4:7]      
//  0xD8700C0B, 0x00000024, //	ds_read2st64_b32 v[0:1], v36 offset0:11 offset1:12         
//  0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
//  0x7E080308,             //	v_mov_b32_e32 v4, v8                                       
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0x7E0A0300,             //	v_mov_b32_e32 v5, v0                                       
//  0x7E000309,             //	v_mov_b32_e32 v0, v9                                       
//  0xBF800000,             //	s_nop 0                                                    
//  0xD3CD8000, 0x04020902, //	v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[4:5], a[0:3]      
//  0xBF800007,             //	s_nop 7                                                    
//  0xBF800002,             //	s_nop 2                                                    
//  0xDA1A0000, 0x0000000A, //	ds_write_b32 v10, a0                                       
//  0xDA1A0100, 0x0000010A, //	ds_write_b32 v10, a1 offset:256                            
//  0xDA1A0200, 0x0000020A, //	ds_write_b32 v10, a2 offset:512                            
//  0xDA1A0300, 0x0000030A, //	ds_write_b32 v10, a3 offset:768                            
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xD9FE0000, 0x0400000C, //	ds_read_b128 v[4:7], v12                                   
//  0xD3CD8000, 0x04120102, //	v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[0:1], a[4:7]      
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0x7E081504,             //	v_cvt_f16_f32_e32 v4, v4                                   
//  0x7E0A1505,             //	v_cvt_f16_f32_e32 v5, v5                                   
//  0x7E0C1506,             //	v_cvt_f16_f32_e32 v6, v6                                   
//  0x7E0E1507,             //	v_cvt_f16_f32_e32 v7, v7                                   
//  0xD2A00000, 0x00020B04, //	v_pack_b32_f16 v0, v4, v5                                  
//  0xD2A00001, 0x00020F06, //	v_pack_b32_f16 v1, v6, v7                                  
//  0xE1381000, 0x8000000B, //	buffer_atomic_pk_add_f16 v0, v11, s[0:3], 0 offen          
//  0xE1381000, 0x8400010B, //	buffer_atomic_pk_add_f16 v1, v11, s[0:3], 4 offen          
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xBF800001,             //	s_nop 1                                                    
//  0xDA1A0000, 0x0000000A, //	ds_write_b32 v10, a0                                       
//  0xDA1A0100, 0x0000010A, //	ds_write_b32 v10, a1 offset:256                            
//  0xDA1A0200, 0x0000020A, //	ds_write_b32 v10, a2 offset:512                            
//  0xDA1A0300, 0x0000030A, //	ds_write_b32 v10, a3 offset:768                            
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0xBF8A0000,             //	s_barrier                                                  
//  0xD9FE0000, 0x0000000C, //	ds_read_b128 v[0:3], v12                                   
//  0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
//  0x7E001500,             //	v_cvt_f16_f32_e32 v0, v0                                   
//  0x7E021501,             //	v_cvt_f16_f32_e32 v1, v1                                   
//  0x7E041502,             //	v_cvt_f16_f32_e32 v2, v2                                   
//  0x7E061503,             //	v_cvt_f16_f32_e32 v3, v3                                   
//  0xD2A00000, 0x00020300, //	v_pack_b32_f16 v0, v0, v1                                  
//  0xE1381080, 0x8000000B, //	buffer_atomic_pk_add_f16 v0, v11, s[0:3], 0 offen offset:128
//  0xD2A00000, 0x00020702, //	v_pack_b32_f16 v0, v2, v3                                  
//  0xE1381080, 0x8400000B, //	buffer_atomic_pk_add_f16 v0, v11, s[0:3], 4 offen offset:128
//  0xBF810000              //	s_endpgm                                                   
// #endif
// };

const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_1280_5120[] = {
#if EMPTY_GEMM_KERNEL
0xBF810000,              // s_endpgm                                                         // 000000000660:
#else
//0xC00A0302, 0x00000000, // s_load_dwordx4 s[12:15], s[4:5], 0x0                           // 000000000000: 
//0xC00A0002, 0x00000010, // s_load_dwordx4 s[0:3], s[4:5], 0x10                            // 000000000008: 
0x20060085,             // v_lshrrev_b32_e32 v3, 5, v0                                    // 000000000010: 
0x24080082,             // v_lshlrev_b32_e32 v4, 2, v0                                    // 000000000014: 
//0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000018: 

0xBE8C0002,             //     s_mov_b32 s12, s2
0xBE8D0003,             //     s_mov_b32 s13, s3
0xBE8E0004,             //     s_mov_b32 s14, s4
0xBE8F0005,             //     s_mov_b32 s15, s5
0xBF800000,             // s_nop 0

0x8E038407,             // s_lshl_b32 s3, s7, 4                                           // 00000000001C: 
0x20160084,             // v_lshrrev_b32_e32 v11, 4, v0                                   // 000000000020: 
0x261808FF, 0x0000007C, // v_and_b32_e32 v12, 0x7c, v4                                    // 000000000024: 
0x100A06FF, 0x00000A00, // v_mul_u32_u24_e32 v5, 0xa00, v3                                // 00000000002C: 
0x8E028706,             // s_lshl_b32 s2, s6, 7                                           // 000000000034: 
0x28261905,             // v_or_b32_e32 v19, v5, v12                                      // 000000000038: 
0x2602008F,             // v_and_b32_e32 v1, 15, v0                                       // 00000000003C: 
0x68141603,             // v_add_u32_e32 v10, s3, v11                                     // 000000000040: 
0xB0031400,             // s_movk_i32 s3, 0x1400                                          // 000000000044: 
0xD1FE0012, 0x02040513, // v_add_lshl_u32 v18, v19, s2, 1                                 // 000000000048: 
0xD2850004, 0x0000070A, // v_mul_lo_u32 v4, v10, s3                                       // 000000000050: 
0x24040281,             // v_lshlrev_b32_e32 v2, 1, v1                                    // 000000000058: 
0xBE8700FF, 0x00020000, // s_mov_b32 s7, 0x20000                                          // 00000000005C: 
0x28080504,             // v_or_b32_e32 v4, v4, v2                                        // 000000000064: 
0xBE88000E,             // s_mov_b32 s8, s14                                              // 000000000068: 
0xBE8A00FF, 0x01900000, // s_mov_b32 s10, 0x1900000                                       // 00000000006C: 
0xBE89000F,             // s_mov_b32 s9, s15                                              // 000000000074: 
0xBE8B0007,             // s_mov_b32 s11, s7                                              // 000000000078: 
0x681A24FF, 0x0000A000, // v_add_u32_e32 v13, 0xa000, v18                                 // 00000000007C: 
0x24280881,             // v_lshlrev_b32_e32 v20, 1, v4                                   // 000000000084: 
0xE0541000, 0x80020412, // buffer_load_dwordx2 v[4:5], v18, s[8:11], 0 offen              // 000000000088: 
0xE0541A00, 0x80020612, // buffer_load_dwordx2 v[6:7], v18, s[8:11], 0 offen offset:2560  // 000000000090: 
0xE0541000, 0x8002080D, // buffer_load_dwordx2 v[8:9], v13, s[8:11], 0 offen              // 000000000098: 
0xE0541A00, 0x8002100D, // buffer_load_dwordx2 v[16:17], v13, s[8:11], 0 offen offset:2560// 0000000000A0: 
0x241A0081,             // v_lshlrev_b32_e32 v13, 1, v0                                   // 0000000000A8: 
0xD2010002, 0x0409410D, // v_and_or_b32 v2, v13, 32, v2                                   // 0000000000AC: 
0xB00E0050,             // s_movk_i32 s14, 0x50                                           // 0000000000B4: 
0x101A060E,             // v_mul_u32_u24_e32 v13, s14, v3                                 // 0000000000B8: 
0x24040481,             // v_lshlrev_b32_e32 v2, 1, v2                                    // 0000000000BC: 
0xB0030108,             // s_movk_i32 s3, 0x108                                           // 0000000000C0: 
0xD1FD000D, 0x0409030D, // v_lshl_add_u32 v13, v13, 1, v2                                 // 0000000000C4: 
0x10040603,             // v_mul_u32_u24_e32 v2, s3, v3                                   // 0000000000CC: 
0x24181881,             // v_lshlrev_b32_e32 v12, 1, v12                                  // 0000000000D0: 
0xD1FE0015, 0x0206050C, // v_add_lshl_u32 v21, v12, v2, 1                                 // 0000000000D4: 
0x28040688,             // v_or_b32_e32 v2, 8, v3                                         // 0000000000DC: 
0x10040403,             // v_mul_u32_u24_e32 v2, s3, v2                                   // 0000000000E0: 
0xBE84000C,             // s_mov_b32 s4, s12                                              // 0000000000E4: 
0xBE85000D,             // s_mov_b32 s5, s13                                              // 0000000000E8: 
0xBE8300FF, 0x00014000, // s_mov_b32 s3, 0x14000                                          // 0000000000EC: 
0xBE8600FF, 0x00050000, // s_mov_b32 s6, 0x50000                                          // 0000000000F4: 
0xD1FE000C, 0x02061902, // v_add_lshl_u32 v12, v2, v12, 1                                 // 0000000000FC: 
0x682C2403,             // v_add_u32_e32 v22, s3, v18                                     // 000000000104: 
0x682E24FF, 0x0001E000, // v_add_u32_e32 v23, 0x1e000, v18                                // 000000000108: 
0xE0501000, 0x80010214, // buffer_load_dword v2, v20, s[4:7], 0 offen                     // 000000000110: 
0xE0501040, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen offset:64          // 000000000118: 
0xB00D0500,             // s_movk_i32 s13, 0x500                                          // 000000000120: 
0x681E180D,             // v_add_u32_e32 v15, s13, v12                                    // 000000000124: 
0xD3D94004, 0x18000080, // v_accvgpr_write_b32 a4, 0                                      // 000000000128: 
0x7E0AA504,             // v_accvgpr_mov_b32 a5, a4                                       // 000000000130: 
0x7E0CA504,             // v_accvgpr_mov_b32 a6, a4                                       // 000000000134: 
0x7E0EA504,             // v_accvgpr_mov_b32 a7, a4                                       // 000000000138: 
0xD3D94000, 0x18000080, // v_accvgpr_write_b32 a0, 0                                      // 00000000013C: 
0x24262681,             // v_lshlrev_b32_e32 v19, 1, v19                                  // 000000000144: 
0xBE8C0080,             // s_mov_b32 s12, 0                                               // 000000000148: 
0x681C2A0D,             // v_add_u32_e32 v14, s13, v21                                    // 00000000014C: 
0xD1FD0013, 0x044D0202, // v_lshl_add_u32 v19, s2, 1, v19                                 // 000000000150: 
0x282828FF, 0x00000080, // v_or_b32_e32 v20, 0x80, v20                                    // 000000000158: 
0xBF8C0F71,             // s_waitcnt vmcnt(1)                                             // 000000000160: 
0xD81A0000, 0x0000020D, // ds_write_b32 v13, v2                                           // 000000000164: 
0xD2A00002, 0x00020D04, // v_pack_b32_f16 v2, v4, v6                                      // 00000000016C: 
0xD2A01803, 0x00020D04, // v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                       // 000000000174: 
0xD2A00004, 0x00020F05, // v_pack_b32_f16 v4, v5, v7                                      // 00000000017C: 
0xD2A01805, 0x00020F05, // v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                       // 000000000184: 
0xD2A00006, 0x00022108, // v_pack_b32_f16 v6, v8, v16                                     // 00000000018C: 
0xD2A01807, 0x00022108, // v_pack_b32_f16 v7, v8, v16 op_sel:[1,1,0]                      // 000000000194: 
0xD2A00008, 0x00022309, // v_pack_b32_f16 v8, v9, v17                                     // 00000000019C: 
0xD2A01809, 0x00022309, // v_pack_b32_f16 v9, v9, v17 op_sel:[1,1,0]                      // 0000000001A4: 
0xD9BE0500, 0x00000215, // ds_write_b128 v21, v[2:5] offset:1280                          // 0000000001AC: 
0xD9BE0500, 0x0000060C, // ds_write_b128 v12, v[6:9] offset:1280                          // 0000000001B4: 
0xE0541000, 0x80020616, // buffer_load_dwordx2 v[6:7], v22, s[8:11], 0 offen              // 0000000001BC: 
0xE0541A00, 0x80020816, // buffer_load_dwordx2 v[8:9], v22, s[8:11], 0 offen offset:2560  // 0000000001C4: 
0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen              // 0000000001CC: 
0xE0541A00, 0x80020417, // buffer_load_dwordx2 v[4:5], v23, s[8:11], 0 offen offset:2560  // 0000000001D4: 
0x24220085,             // v_lshlrev_b32_e32 v17, 5, v0                                   // 0000000001DC: 
0xD1C8000C, 0x020D0300, // v_bfe_u32 v12, v0, 1, 3                                        // 0000000001E0: 
0xD1C80010, 0x02090900, // v_bfe_u32 v16, v0, 4, 2                                        // 0000000001E8: 
0x262222A0,             // v_and_b32_e32 v17, 32, v17                                     // 0000000001F0: 
0xD2000011, 0x04450710, // v_lshl_or_b32 v17, v16, 3, v17                                 // 0000000001F4: 
0x1018180E,             // v_mul_u32_u24_e32 v12, s14, v12                                // 0000000001FC: 
0xD1FE0011, 0x02061911, // v_add_lshl_u32 v17, v17, v12, 1                                // 000000000200: 
0x20180082,             // v_lshrrev_b32_e32 v12, 2, v0                                   // 000000000208: 
0xD3D84017, 0x18000105, // v_accvgpr_read_b32 v23, a5                                     // 00000000020C: 
0xD2010000, 0x0405610C, // v_and_or_b32 v0, v12, 48, v1                                   // 000000000214: 
0x102020FF, 0x00000420, // v_mul_u32_u24_e32 v16, 0x420, v16                              // 00000000021C: 
0xD1FD0010, 0x04410300, // v_lshl_add_u32 v16, v0, 1, v16                                 // 000000000224: 
0xD3D94001, 0x18000117, // v_accvgpr_write_b32 a1, v23                                    // 00000000022C: 
0xD3D84017, 0x18000106, // v_accvgpr_read_b32 v23, a6                                     // 000000000234: 
0xD1FD0010, 0x00350310, // v_lshl_add_u32 v16, v16, 1, s13                                // 00000000023C: 
0xD3D94002, 0x18000117, // v_accvgpr_write_b32 a2, v23                                    // 000000000244: 
0xD3D84017, 0x18000107, // v_accvgpr_read_b32 v23, a7                                     // 00000000024C: 
0x682A20A0,             // v_add_u32_e32 v21, 32, v16                                     // 000000000254: 
0x682C20B0,             // v_add_u32_e32 v22, 48, v16                                     // 000000000258: 
0xD3D94003, 0x18000117, // v_accvgpr_write_b32 a3, v23                                    // 00000000025C: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0) <BB0_1>                                   // 000000000264: 
0xBF8A0000,             // s_barrier                                                      // 000000000268: 
0xD9FE0000, 0x18000011, // ds_read_b128 v[24:27], v17                                     // 00000000026C: 
0xD8700100, 0x24000010, // ds_read2st64_b32 v[36:37], v16 offset1:1                       // 000000000274: 
0xD86EC484, 0x26000010, // ds_read2_b32 v[38:39], v16 offset0:132 offset1:196             // 00000000027C: 
0xD8700504, 0x28000015, // ds_read2st64_b32 v[40:41], v21 offset0:4 offset1:5             // 000000000284: 
0xD8700706, 0x2A000016, // ds_read2st64_b32 v[42:43], v22 offset0:6 offset1:7             // 00000000028C: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000294: 
0xBF8A0000,             // s_barrier                                                      // 000000000298: 
0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                           // 00000000029C: 
0x7E400324,             // v_mov_b32_e32 v32, v36                                         // 0000000002A0: 
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 0000000002A4: 
0x7E420326,             // v_mov_b32_e32 v33, v38                                         // 0000000002A8: 
0xBF8C0F74,             // s_waitcnt vmcnt(4)                                             // 0000000002AC: 
0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                          // 0000000002B0: 
0xE0501000, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen                    // 0000000002B8: 
0xD3CD8004, 0x04124118, // v_mfma_f32_16x16x16f16 a[4:7], v[24:25], v[32:33], a[4:7]      // 0000000002C0: 
0xBF8C0F73,             // s_waitcnt vmcnt(3)                                             // 0000000002C8: 
0xD2A0001C, 0x00021106, // v_pack_b32_f16 v28, v6, v8                                     // 0000000002CC: 
0xD2A0181D, 0x00021106, // v_pack_b32_f16 v29, v6, v8 op_sel:[1,1,0]                      // 0000000002D4: 
0xD2A0001E, 0x00021307, // v_pack_b32_f16 v30, v7, v9                                     // 0000000002DC: 
0xD2A0181F, 0x00021307, // v_pack_b32_f16 v31, v7, v9 op_sel:[1,1,0]                      // 0000000002E4: 
0xBF8C0F71,             // s_waitcnt vmcnt(1)                                             // 0000000002EC: 
0xD2A00020, 0x00020902, // v_pack_b32_f16 v32, v2, v4                                     // 0000000002F0: 
0xD2A01821, 0x00020902, // v_pack_b32_f16 v33, v2, v4 op_sel:[1,1,0]                      // 0000000002F8: 
0x6804260C,             // v_add_u32_e32 v2, s12, v19                                     // 000000000300: 
0x680804FF, 0x00028000, // v_add_u32_e32 v4, 0x28000, v2                                  // 000000000304: 
0x810C030C,             // s_add_i32 s12, s12, s3                                         // 00000000030C: 
0xD2A00022, 0x00020B03, // v_pack_b32_f16 v34, v3, v5                                     // 000000000310: 
0xD2A01823, 0x00020B03, // v_pack_b32_f16 v35, v3, v5 op_sel:[1,1,0]                      // 000000000318: 
0x682E04FF, 0x00032000, // v_add_u32_e32 v23, 0x32000, v2                                 // 000000000320: 
0xE0541000, 0x80020604, // buffer_load_dwordx2 v[6:7], v4, s[8:11], 0 offen               // 000000000328: 
0xE0541A00, 0x80020804, // buffer_load_dwordx2 v[8:9], v4, s[8:11], 0 offen offset:2560   // 000000000330: 
0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen              // 000000000338: 
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 000000000340: 
0x7E0A032A,             // v_mov_b32_e32 v5, v42                                          // 000000000344: 
0x7E080328,             // v_mov_b32_e32 v4, v40                                          // 000000000348: 
0xBF07FF0C, 0x00C58000, // s_cmp_lg_u32 s12, 0xc58000                                     // 00000000034C: 
0x7E4C0325,             // v_mov_b32_e32 v38, v37                                         // 000000000354: 
0xD3CD8004, 0x0412091A, // v_mfma_f32_16x16x16f16 a[4:7], v[26:27], v[4:5], a[4:7]        // 000000000358: 
0xE0541A00, 0x80020417, // buffer_load_dwordx2 v[4:5], v23, s[8:11], 0 offen offset:2560  // 000000000360: 
0x7E540329,             // v_mov_b32_e32 v42, v41                                         // 000000000368: 
0x682828C0,             // v_add_u32_e32 v20, 64, v20                                     // 00000000036C: 
0xD9BE0000, 0x00001C0E, // ds_write_b128 v14, v[28:31]                                    // 000000000370: 
0xD9BE0000, 0x0000200F, // ds_write_b128 v15, v[32:35]                                    // 000000000378: 
0xD3CD8000, 0x04024D18, // v_mfma_f32_16x16x16f16 a[0:3], v[24:25], v[38:39], a[0:3]      // 000000000380: 
0xD3CD8000, 0x0402551A, // v_mfma_f32_16x16x16f16 a[0:3], v[26:27], v[42:43], a[0:3]      // 000000000388: 
0xBF85FFB4,             // s_cbranch_scc1 BB0_1                                           // 000000000390: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000394: 
0xBF8A0000,             // s_barrier                                                      // 000000000398: 
0xD9FE0000, 0x14000011, // ds_read_b128 v[20:23], v17                                     // 00000000039C: 
0xD8700100, 0x18000010, // ds_read2st64_b32 v[24:25], v16 offset1:1                       // 0000000003A4: 
0xD86EC484, 0x1C000010, // ds_read2_b32 v[28:29], v16 offset0:132 offset1:196             // 0000000003AC: 
0x682620A0,             // v_add_u32_e32 v19, 32, v16                                     // 0000000003B4: 
0x684420B0,             // v_add_u32_e32 v34, 48, v16                                     // 0000000003B8: 
0x24000082,             // v_lshlrev_b32_e32 v0, 2, v0                                    // 0000000003BC: 
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 0000000003C0: 
0x7E340318,             // v_mov_b32_e32 v26, v24                                         // 0000000003C4: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000003C8: 
0x7E36031C,             // v_mov_b32_e32 v27, v28                                         // 0000000003CC: 
0x7E380319,             // v_mov_b32_e32 v28, v25                                         // 0000000003D0: 
0xBF800000,             // s_nop 0                                                        // 0000000003D4: 
0xD3CD8004, 0x04123514, // v_mfma_f32_16x16x16f16 a[4:7], v[20:21], v[26:27], a[4:7]      // 0000000003D8: 
0xD8700504, 0x1A000013, // ds_read2st64_b32 v[26:27], v19 offset0:4 offset1:5             // 0000000003E0: 
0xD8700706, 0x1E000022, // ds_read2st64_b32 v[30:31], v34 offset0:6 offset1:7             // 0000000003E8: 
0xB0030500,             // s_movk_i32 s3, 0x500                                           // 0000000003F0: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000003F4: 
0xBF8A0000,             // s_barrier                                                      // 0000000003F8: 
0xBF8C0F74,             // s_waitcnt vmcnt(4)                                             // 0000000003FC: 
0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                          // 000000000400: 
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 000000000408: 
0x7E40031A,             // v_mov_b32_e32 v32, v26                                         // 00000000040C: 
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 000000000410: 
0x7E42031E,             // v_mov_b32_e32 v33, v30                                         // 000000000414: 
0xBF8C0F72,             // s_waitcnt vmcnt(2)                                             // 000000000418: 
0xD2A00018, 0x00021106, // v_pack_b32_f16 v24, v6, v8                                     // 00000000041C: 
0xD2A01819, 0x00021106, // v_pack_b32_f16 v25, v6, v8 op_sel:[1,1,0]                      // 000000000424: 
0x7E3C031B,             // v_mov_b32_e32 v30, v27                                         // 00000000042C: 
0xD2A0001A, 0x00021307, // v_pack_b32_f16 v26, v7, v9                                     // 000000000430: 
0xD2A0181B, 0x00021307, // v_pack_b32_f16 v27, v7, v9 op_sel:[1,1,0]                      // 000000000438: 
0xBF8C0F70,             // s_waitcnt vmcnt(0)                                             // 000000000440: 
0xD2A00006, 0x00020902, // v_pack_b32_f16 v6, v2, v4                                      // 000000000444: 
0xD2A01807, 0x00020902, // v_pack_b32_f16 v7, v2, v4 op_sel:[1,1,0]                       // 00000000044C: 
0xD2A00008, 0x00020B03, // v_pack_b32_f16 v8, v3, v5                                      // 000000000454: 
0xD2A01809, 0x00020B03, // v_pack_b32_f16 v9, v3, v5 op_sel:[1,1,0]                       // 00000000045C: 
0xD9BE0000, 0x0000180E, // ds_write_b128 v14, v[24:27]                                    // 000000000464: 
0xD9BE0000, 0x0000060F, // ds_write_b128 v15, v[6:9]                                      // 00000000046C: 
0xD3CD8004, 0x04124116, // v_mfma_f32_16x16x16f16 a[4:7], v[22:23], v[32:33], a[4:7]      // 000000000474: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 00000000047C: 
0xBF8A0000,             // s_barrier                                                      // 000000000480: 
0xD9FE0000, 0x02000011, // ds_read_b128 v[2:5], v17                                       // 000000000484: 
0xD8700100, 0x06000010, // ds_read2st64_b32 v[6:7], v16 offset1:1                         // 00000000048C: 
0xD86EC484, 0x08000010, // ds_read2_b32 v[8:9], v16 offset0:132 offset1:196               // 000000000494: 
0xD8700504, 0x10000013, // ds_read2st64_b32 v[16:17], v19 offset0:4 offset1:5             // 00000000049C: 
0xD8700706, 0x12000022, // ds_read2st64_b32 v[18:19], v34 offset0:6 offset1:7             // 0000000004A4: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000004AC: 
0xBF8A0000,             // s_barrier                                                      // 0000000004B0: 
0xD3CD8000, 0x04023914, // v_mfma_f32_16x16x16f16 a[0:3], v[20:21], v[28:29], a[0:3]      // 0000000004B4: 
0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                           // 0000000004BC: 
0x7E1C0306,             // v_mov_b32_e32 v14, v6                                          // 0000000004C0: 
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 0000000004C4: 
0x7E1E0308,             // v_mov_b32_e32 v15, v8                                          // 0000000004C8: 
0x7E100307,             // v_mov_b32_e32 v8, v7                                           // 0000000004CC: 
0x260C188C,             // v_and_b32_e32 v6, 12, v12                                      // 0000000004D0: 
0xD200000C, 0x04011106, // v_lshl_or_b32 v12, v6, 8, v0                                   // 0000000004D4: 
0x24000284,             // v_lshlrev_b32_e32 v0, 4, v1                                    // 0000000004DC: 
0xD3CD8004, 0x04121D02, // v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[14:15], a[4:7]        // 0000000004E0: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000004E8: 
0x7E1E0312,             // v_mov_b32_e32 v15, v18                                         // 0000000004EC: 
0x7E1C0310,             // v_mov_b32_e32 v14, v16                                         // 0000000004F0: 
0x7E240311,             // v_mov_b32_e32 v18, v17                                         // 0000000004F4: 
0xD3CD8000, 0x04023D16, // v_mfma_f32_16x16x16f16 a[0:3], v[22:23], v[30:31], a[0:3]      // 0000000004F8: 
0xD3CD8004, 0x04121D04, // v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[14:15], a[4:7]        // 000000000500: 
0xD3CD8000, 0x04021102, // v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[8:9], a[0:3]          // 000000000508: 
0xD2000002, 0x0401110B, // v_lshl_or_b32 v2, v11, 8, v0                                   // 000000000510: 
0xBF800007,             // s_nop 7                                                        // 000000000518: 
0xBF800000,             // s_nop 0                                                        // 00000000051C: 
0xDA1A0000, 0x0000040C, // ds_write_b32 v12, a4                                           // 000000000520: 
0xDA1A0100, 0x0000050C, // ds_write_b32 v12, a5 offset:256                                // 000000000528: 
0xDA1A0200, 0x0000060C, // ds_write_b32 v12, a6 offset:512                                // 000000000530: 
0xDA1A0300, 0x0000070C, // ds_write_b32 v12, a7 offset:768                                // 000000000538: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000540: 
0xBF8A0000,             // s_barrier                                                      // 000000000544: 
0xD9FE0000, 0x06000002, // ds_read_b128 v[6:9], v2                                        // 000000000548: 
0xD3CD8000, 0x04022504, // v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[18:19], a[0:3]        // 000000000550: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000558: 
0x7E001506,             // v_cvt_f16_f32_e32 v0, v6                                       // 00000000055C: 
0x7E061508,             // v_cvt_f16_f32_e32 v3, v8                                       // 000000000560: 
0x7E0C1509,             // v_cvt_f16_f32_e32 v6, v9                                       // 000000000564: 
0x7E0E1507,             // v_cvt_f16_f32_e32 v7, v7                                       // 000000000568: 
0x24100282,             // v_lshlrev_b32_e32 v8, 2, v1                                    // 00000000056C: 
0xD2A00001, 0x00020D03, // v_pack_b32_f16 v1, v3, v6                                      // 000000000570: 
0xD2850003, 0x0000070A, // v_mul_lo_u32 v3, v10, s3                                       // 000000000578: 
0x68060602,             // v_add_u32_e32 v3, s2, v3                                       // 000000000580: 
0xD2A00000, 0x00020F00, // v_pack_b32_f16 v0, v0, v7                                      // 000000000584: 
0xBE8300FF, 0x00020000, // s_mov_b32 s3, 0x20000                                          // 00000000058C: 
0xBE8200FF, 0x00014000, // s_mov_b32 s2, 0x14000                                          // 000000000594: 
0xD1FE0004, 0x02061103, // v_add_lshl_u32 v4, v3, v8, 1                                   // 00000000059C: 
0xE0741000, 0x80000004, // buffer_store_dwordx2 v[0:1], v4, s[0:3], 0 offen               // 0000000005A4: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005AC: 
0xBF8A0000,             // s_barrier                                                      // 0000000005B0: 
0xDA1A0000, 0x0000000C, // ds_write_b32 v12, a0                                           // 0000000005B4: 
0xDA1A0100, 0x0000010C, // ds_write_b32 v12, a1 offset:256                                // 0000000005BC: 
0xDA1A0200, 0x0000020C, // ds_write_b32 v12, a2 offset:512                                // 0000000005C4: 
0xDA1A0300, 0x0000030C, // ds_write_b32 v12, a3 offset:768                                // 0000000005CC: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005D4: 
0xBF8A0000,             // s_barrier                                                      // 0000000005D8: 
0xD9FE0000, 0x00000002, // ds_read_b128 v[0:3], v2                                        // 0000000005DC: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005E4: 
0x7E001500,             // v_cvt_f16_f32_e32 v0, v0                                       // 0000000005E8: 
0x7E0A1501,             // v_cvt_f16_f32_e32 v5, v1                                       // 0000000005EC: 
0x7E041502,             // v_cvt_f16_f32_e32 v2, v2                                       // 0000000005F0: 
0x7E061503,             // v_cvt_f16_f32_e32 v3, v3                                       // 0000000005F4: 
0xD2A00000, 0x00020B00, // v_pack_b32_f16 v0, v0, v5                                      // 0000000005F8: 
0xD2A00001, 0x00020702, // v_pack_b32_f16 v1, v2, v3                                      // 000000000600: 
0xE0741080, 0x80000004, // buffer_store_dwordx2 v[0:1], v4, s[0:3], 0 offen offset:128    // 000000000608: 
0xBF810000              // s_endpgm                                                       // 000000000610: 
#endif
};

// GEMM kernels using atomic instructions (buffer_atomic_pk_add_f16) not working for some reason.
//
// const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_1280_5120[] = {
// #if EMPTY_GEMM_KERNEL
// 0xBF810000,              // s_endpgm                                                         // 000000000660:
// #else
// // Original ISA:
// // s4-s5 : kernarg pointer
// // s6 : workgroup_id_x
// // s7 : workgroup_id_y
// // s8 : workgroup_id_z
// //0xC00A0302, 0x00000000, // s_load_dwordx4 s[12:15], s[4:5], 0x0                             // 000000000000:
// //0xC00A0002, 0x00000010, // s_load_dwordx4 s[0:3], s[4:5], 0x10                              // 000000000008:
// //0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000010:
// 
// // Hacked ISA
// // s0-s1: matrix C pointer
// // s2-s3: matrix A pointer
// // s4-s5: matrix B pointer
// // s6 : workgroup_id_x
// // s7 : workgroup_id_y
// // s8 : workgroup_id_z
// 0xBE8C0002,             //     s_mov_b32 s12, s2
// 0xBE8D0003,             //     s_mov_b32 s13, s3
// 0xBE8E0004,             //     s_mov_b32 s14, s4
// 0xBE8F0005,             //     s_mov_b32 s15, s5
// 0xBF800000,             // s_nop 0
// 
// // The rest of GEMM algorithm.
// 0x8E038407,             // s_lshl_b32 s3, s7, 4                                             // 000000000014: 
// 0x20140084,             // v_lshrrev_b32_e32 v10, 4, v0                                     // 000000000018: 
// 0x24020082,             // v_lshlrev_b32_e32 v1, 2, v0                                      // 00000000001C: 
// 0x261802FF, 0x0000007C, // v_and_b32_e32 v12, 0x7c, v1                                      // 000000000020: 
// 0x68021403,             // v_add_u32_e32 v1, s3, v10                                        // 000000000028: 
// 0xB0031400,             // s_movk_i32 s3, 0x1400                                            // 00000000002C: 
// 0xD2850004, 0x00000701, // v_mul_lo_u32 v4, v1, s3                                          // 000000000030: 
// 0x8E038508,             // s_lshl_b32 s3, s8, 5                                             // 000000000038: 
// 0x2606148E,             // v_and_b32_e32 v3, 14, v10                                        // 00000000003C: 
// 0x280A0603,             // v_or_b32_e32 v5, s3, v3                                          // 000000000040: 
// 0xB0100500,             // s_movk_i32 s16, 0x500                                            // 000000000044: 
// 0xD2850005, 0x00002105, // v_mul_lo_u32 v5, v5, s16                                         // 000000000048: 
// 0x8E028706,             // s_lshl_b32 s2, s6, 7                                             // 000000000050: 
// 0x28261905,             // v_or_b32_e32 v19, v5, v12                                        // 000000000054: 
// 0x2616008F,             // v_and_b32_e32 v11, 15, v0                                        // 000000000058: 
// 0xD1FE0015, 0x02040513, // v_add_lshl_u32 v21, v19, s2, 1                                   // 00000000005C: 
// 0x68080803,             // v_add_u32_e32 v4, s3, v4                                         // 000000000064: 
// 0x24041681,             // v_lshlrev_b32_e32 v2, 1, v11                                     // 000000000068: 
// 0xBE8700FF, 0x00020000, // s_mov_b32 s7, 0x20000                                            // 00000000006C: 
// 0x28080504,             // v_or_b32_e32 v4, v4, v2                                          // 000000000074: 
// 0xBE8A00FF, 0x01900000, // s_mov_b32 s10, 0x1900000                                         // 000000000078: 
// 0xBE88000E,             // s_mov_b32 s8, s14                                                // 000000000080: 
// 0xBE89000F,             // s_mov_b32 s9, s15                                                // 000000000084: 
// 0xBE8B0007,             // s_mov_b32 s11, s7                                                // 000000000088: 
// 0x681A2AFF, 0x0000A000, // v_add_u32_e32 v13, 0xa000, v21                                   // 00000000008C: 
// 0x24280881,             // v_lshlrev_b32_e32 v20, 1, v4                                     // 000000000094: 
// 0xE0541000, 0x80020415, // buffer_load_dwordx2 v[4:5], v21, s[8:11], 0 offen                // 000000000098: 
// 0xE0541A00, 0x80020615, // buffer_load_dwordx2 v[6:7], v21, s[8:11], 0 offen offset:2560    // 0000000000A0: 
// 0xE0541000, 0x8002080D, // buffer_load_dwordx2 v[8:9], v13, s[8:11], 0 offen                // 0000000000A8: 
// 0xE0541A00, 0x8002100D, // buffer_load_dwordx2 v[16:17], v13, s[8:11], 0 offen offset:2560  // 0000000000B0: 
// 0x241A0081,             // v_lshlrev_b32_e32 v13, 1, v0                                     // 0000000000B8: 
// 0xD2010002, 0x0409410D, // v_and_or_b32 v2, v13, 32, v2                                     // 0000000000BC: 
// 0xBE85000D,             // s_mov_b32 s5, s13                                                // 0000000000C4: 
// 0x201C0085,             // v_lshrrev_b32_e32 v14, 5, v0                                     // 0000000000C8: 
// 0xB00D0050,             // s_movk_i32 s13, 0x50                                             // 0000000000CC: 
// 0x101A1C0D,             // v_mul_u32_u24_e32 v13, s13, v14                                  // 0000000000D0: 
// 0x24040481,             // v_lshlrev_b32_e32 v2, 1, v2                                      // 0000000000D4: 
// 0xB0030108,             // s_movk_i32 s3, 0x108                                             // 0000000000D8: 
// 0xD1FD000D, 0x0409030D, // v_lshl_add_u32 v13, v13, 1, v2                                   // 0000000000DC: 
// 0x10041C03,             // v_mul_u32_u24_e32 v2, s3, v14                                    // 0000000000E4: 
// 0x24181881,             // v_lshlrev_b32_e32 v12, 1, v12                                    // 0000000000E8: 
// 0xD1FE0016, 0x0206050C, // v_add_lshl_u32 v22, v12, v2, 1                                   // 0000000000EC: 
// 0x28040690,             // v_or_b32_e32 v2, 16, v3                                          // 0000000000F4: 
// 0x20040481,             // v_lshrrev_b32_e32 v2, 1, v2                                      // 0000000000F8: 
// 0x10040403,             // v_mul_u32_u24_e32 v2, s3, v2                                     // 0000000000FC: 
// 0xBE84000C,             // s_mov_b32 s4, s12                                                // 000000000100: 
// 0xBE8600FF, 0x00050000, // s_mov_b32 s6, 0x50000                                            // 000000000104: 
// 0xD1FE000C, 0x02061902, // v_add_lshl_u32 v12, v2, v12, 1                                   // 00000000010C: 
// 0xE0501000, 0x80010214, // buffer_load_dword v2, v20, s[4:7], 0 offen                       // 000000000114: 
// 0xE0501200, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen offset:512           // 00000000011C: 
// 0xBE8300FF, 0x000A0000, // s_mov_b32 s3, 0xa0000                                            // 000000000124: 
// 0x682E2A03,             // v_add_u32_e32 v23, s3, v21                                       // 00000000012C: 
// 0x682A2AFF, 0x000AA000, // v_add_u32_e32 v21, 0xaa000, v21                                  // 000000000130: 
// 0x681E1810,             // v_add_u32_e32 v15, s16, v12                                      // 000000000138: 
// 0xD3D94004, 0x18000080, // v_accvgpr_write_b32 a4, 0                                        // 00000000013C: 
// 0x7E0AA504,             // v_accvgpr_mov_b32 a5, a4                                         // 000000000144: 
// 0x7E0CA504,             // v_accvgpr_mov_b32 a6, a4                                         // 000000000148: 
// 0x7E0EA504,             // v_accvgpr_mov_b32 a7, a4                                         // 00000000014C: 
// 0xD3D94000, 0x18000080, // v_accvgpr_write_b32 a0, 0                                        // 000000000150: 
// 0x24262681,             // v_lshlrev_b32_e32 v19, 1, v19                                    // 000000000158: 
// 0xBE8C0080,             // s_mov_b32 s12, 0                                                 // 00000000015C: 
// 0x681C2C10,             // v_add_u32_e32 v14, s16, v22                                      // 000000000160: 
// 0xD1FD0013, 0x044D0202, // v_lshl_add_u32 v19, s2, 1, v19                                   // 000000000164: 
// 0x682828FF, 0x00000400, // v_add_u32_e32 v20, 0x400, v20                                    // 00000000016C: 
// 0xBF8C0F71,             // s_waitcnt vmcnt(1)                                               // 000000000174: 
// 0xD81A0000, 0x0000020D, // ds_write_b32 v13, v2                                             // 000000000178: 
// 0xD2A00002, 0x00020D04, // v_pack_b32_f16 v2, v4, v6                                        // 000000000180: 
// 0xD2A01803, 0x00020D04, // v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                         // 000000000188: 
// 0xD2A00004, 0x00020F05, // v_pack_b32_f16 v4, v5, v7                                        // 000000000190: 
// 0xD2A01805, 0x00020F05, // v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                         // 000000000198: 
// 0xD2A00006, 0x00022108, // v_pack_b32_f16 v6, v8, v16                                       // 0000000001A0: 
// 0xD2A01807, 0x00022108, // v_pack_b32_f16 v7, v8, v16 op_sel:[1,1,0]                        // 0000000001A8: 
// 0xD2A00008, 0x00022309, // v_pack_b32_f16 v8, v9, v17                                       // 0000000001B0: 
// 0xD2A01809, 0x00022309, // v_pack_b32_f16 v9, v9, v17 op_sel:[1,1,0]                        // 0000000001B8: 
// 0xD9BE0500, 0x00000216, // ds_write_b128 v22, v[2:5] offset:1280                            // 0000000001C0: 
// 0xD9BE0500, 0x0000060C, // ds_write_b128 v12, v[6:9] offset:1280                            // 0000000001C8: 
// 0xE0541000, 0x80020617, // buffer_load_dwordx2 v[6:7], v23, s[8:11], 0 offen                // 0000000001D0: 
// 0xE0541A00, 0x80020817, // buffer_load_dwordx2 v[8:9], v23, s[8:11], 0 offen offset:2560    // 0000000001D8: 
// 0xE0541000, 0x80020215, // buffer_load_dwordx2 v[2:3], v21, s[8:11], 0 offen                // 0000000001E0: 
// 0xE0541A00, 0x80020415, // buffer_load_dwordx2 v[4:5], v21, s[8:11], 0 offen offset:2560    // 0000000001E8: 
// 0x24220085,             // v_lshlrev_b32_e32 v17, 5, v0                                     // 0000000001F0: 
// 0xD1C8000C, 0x020D0300, // v_bfe_u32 v12, v0, 1, 3                                          // 0000000001F4: 
// 0xD1C80010, 0x02090900, // v_bfe_u32 v16, v0, 4, 2                                          // 0000000001FC: 
// 0x262222A0,             // v_and_b32_e32 v17, 32, v17                                       // 000000000204: 
// 0xD2000011, 0x04450710, // v_lshl_or_b32 v17, v16, 3, v17                                   // 000000000208: 
// 0x1018180D,             // v_mul_u32_u24_e32 v12, s13, v12                                  // 000000000210: 
// 0xD1FE0011, 0x02061911, // v_add_lshl_u32 v17, v17, v12, 1                                  // 000000000214: 
// 0x20180082,             // v_lshrrev_b32_e32 v12, 2, v0                                     // 00000000021C: 
// 0xD3D84017, 0x18000105, // v_accvgpr_read_b32 v23, a5                                       // 000000000220: 
// 0xD2010000, 0x042D610C, // v_and_or_b32 v0, v12, 48, v11                                    // 000000000228: 
// 0x102020FF, 0x00000420, // v_mul_u32_u24_e32 v16, 0x420, v16                                // 000000000230: 
// 0xD1FD0010, 0x04410300, // v_lshl_add_u32 v16, v0, 1, v16                                   // 000000000238: 
// 0xD3D94001, 0x18000117, // v_accvgpr_write_b32 a1, v23                                      // 000000000240: 
// 0xD3D84017, 0x18000106, // v_accvgpr_read_b32 v23, a6                                       // 000000000248: 
// 0xD1FD0010, 0x00410310, // v_lshl_add_u32 v16, v16, 1, s16                                  // 000000000250: 
// 0xD3D94002, 0x18000117, // v_accvgpr_write_b32 a2, v23                                      // 000000000258: 
// 0xD3D84017, 0x18000107, // v_accvgpr_read_b32 v23, a7                                       // 000000000260: 
// 0x682A20A0,             // v_add_u32_e32 v21, 32, v16                                       // 000000000268: 
// 0x682C20B0,             // v_add_u32_e32 v22, 48, v16                                       // 00000000026C: 
// 0xD3D94003, 0x18000117, // v_accvgpr_write_b32 a3, v23                                      // 000000000270: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0) // 279 <BB0_1>                              // 000000000278: 
// 0xBF8A0000,             // s_barrier                                                        // 00000000027C: 
// 0xD9FE0000, 0x18000011, // ds_read_b128 v[24:27], v17                                       // 000000000280: 
// 0xD8700100, 0x24000010, // ds_read2st64_b32 v[36:37], v16 offset1:1                         // 000000000288: 
// 0xD86EC484, 0x26000010, // ds_read2_b32 v[38:39], v16 offset0:132 offset1:196               // 000000000290: 
// 0xD8700504, 0x28000015, // ds_read2st64_b32 v[40:41], v21 offset0:4 offset1:5               // 000000000298: 
// 0xD8700706, 0x2A000016, // ds_read2st64_b32 v[42:43], v22 offset0:6 offset1:7               // 0000000002A0: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000002A8: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000002AC: 
// 0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                             // 0000000002B0: 
// 0x7E400324,             // v_mov_b32_e32 v32, v36                                           // 0000000002B4: 
// 0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                             // 0000000002B8: 
// 0x7E420326,             // v_mov_b32_e32 v33, v38                                           // 0000000002BC: 
// 0xBF8C0F74,             // s_waitcnt vmcnt(4)                                               // 0000000002C0: 
// 0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                            // 0000000002C4: 
// 0xE0501000, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen                      // 0000000002CC: 
// 0xD3CD8004, 0x04124118, // v_mfma_f32_16x16x16f16 a[4:7], v[24:25], v[32:33], a[4:7]        // 0000000002D4: 
// 0xBF8C0F73,             // s_waitcnt vmcnt(3)                                               // 0000000002DC: 
// 0xD2A0001C, 0x00021106, // v_pack_b32_f16 v28, v6, v8                                       // 0000000002E0: 
// 0xD2A0181D, 0x00021106, // v_pack_b32_f16 v29, v6, v8 op_sel:[1,1,0]                        // 0000000002E8: 
// 0xD2A0001E, 0x00021307, // v_pack_b32_f16 v30, v7, v9                                       // 0000000002F0: 
// 0xD2A0181F, 0x00021307, // v_pack_b32_f16 v31, v7, v9 op_sel:[1,1,0]                        // 0000000002F8: 
// 0xBF8C0F71,             // s_waitcnt vmcnt(1)                                               // 000000000300: 
// 0xD2A00020, 0x00020902, // v_pack_b32_f16 v32, v2, v4                                       // 000000000304: 
// 0xD2A01821, 0x00020902, // v_pack_b32_f16 v33, v2, v4 op_sel:[1,1,0]                        // 00000000030C: 
// 0x6804260C,             // v_add_u32_e32 v2, s12, v19                                       // 000000000314: 
// 0x680804FF, 0x00140000, // v_add_u32_e32 v4, 0x140000, v2                                   // 000000000318: 
// 0x810C030C,             // s_add_i32 s12, s12, s3                                           // 000000000320: 
// 0xD2A00022, 0x00020B03, // v_pack_b32_f16 v34, v3, v5                                       // 000000000324: 
// 0xD2A01823, 0x00020B03, // v_pack_b32_f16 v35, v3, v5 op_sel:[1,1,0]                        // 00000000032C: 
// 0x682E04FF, 0x0014A000, // v_add_u32_e32 v23, 0x14a000, v2                                  // 000000000334: 
// 0xE0541000, 0x80020604, // buffer_load_dwordx2 v[6:7], v4, s[8:11], 0 offen                 // 00000000033C: 
// 0xE0541A00, 0x80020804, // buffer_load_dwordx2 v[8:9], v4, s[8:11], 0 offen offset:2560     // 000000000344: 
// 0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen                // 00000000034C: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 000000000354: 
// 0x7E0A032A,             // v_mov_b32_e32 v5, v42                                            // 000000000358: 
// 0x7E080328,             // v_mov_b32_e32 v4, v40                                            // 00000000035C: 
// 0xBF07FF0C, 0x00B40000, // s_cmp_lg_u32 s12, 0xb40000                                       // 000000000360: 
// 0x7E4C0325,             // v_mov_b32_e32 v38, v37                                           // 000000000368: 
// 0xD3CD8004, 0x0412091A, // v_mfma_f32_16x16x16f16 a[4:7], v[26:27], v[4:5], a[4:7]          // 00000000036C: 
// 0xE0541A00, 0x80020417, // buffer_load_dwordx2 v[4:5], v23, s[8:11], 0 offen offset:2560    // 000000000374: 
// 0x7E540329,             // v_mov_b32_e32 v42, v41                                           // 00000000037C: 
// 0x682828FF, 0x00000200, // v_add_u32_e32 v20, 0x200, v20                                    // 000000000380: 
// 0xD9BE0000, 0x00001C0E, // ds_write_b128 v14, v[28:31]                                      // 000000000388: 
// 0xD9BE0000, 0x0000200F, // ds_write_b128 v15, v[32:35]                                      // 000000000390: 
// 0xD3CD8000, 0x04024D18, // v_mfma_f32_16x16x16f16 a[0:3], v[24:25], v[38:39], a[0:3]        // 000000000398: 
// 0xD3CD8000, 0x0402551A, // v_mfma_f32_16x16x16f16 a[0:3], v[26:27], v[42:43], a[0:3]        // 0000000003A0: 
// 0xBF85FFB3,             // s_cbranch_scc1 BB0_1                                             // 0000000003A8: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000003AC: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000003B0: 
// 0xD9FE0000, 0x14000011, // ds_read_b128 v[20:23], v17                                       // 0000000003B4: 
// 0xD8700100, 0x18000010, // ds_read2st64_b32 v[24:25], v16 offset1:1                         // 0000000003BC: 
// 0xD86EC484, 0x1C000010, // ds_read2_b32 v[28:29], v16 offset0:132 offset1:196               // 0000000003C4: 
// 0x682620A0,             // v_add_u32_e32 v19, 32, v16                                       // 0000000003CC: 
// 0x684420B0,             // v_add_u32_e32 v34, 48, v16                                       // 0000000003D0: 
// 0x24000082,             // v_lshlrev_b32_e32 v0, 2, v0                                      // 0000000003D4: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 0000000003D8: 
// 0x7E340318,             // v_mov_b32_e32 v26, v24                                           // 0000000003DC: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000003E0: 
// 0x7E36031C,             // v_mov_b32_e32 v27, v28                                           // 0000000003E4: 
// 0x7E380319,             // v_mov_b32_e32 v28, v25                                           // 0000000003E8: 
// 0xBF800000,             // s_nop 0                                                          // 0000000003EC: 
// 0xD3CD8004, 0x04123514, // v_mfma_f32_16x16x16f16 a[4:7], v[20:21], v[26:27], a[4:7]        // 0000000003F0: 
// 0xD8700504, 0x1A000013, // ds_read2st64_b32 v[26:27], v19 offset0:4 offset1:5               // 0000000003F8: 
// 0xD8700706, 0x1E000022, // ds_read2st64_b32 v[30:31], v34 offset0:6 offset1:7               // 000000000400: 
// 0xB0030500,             // s_movk_i32 s3, 0x500                                             // 000000000408: 
// 0xD2850001, 0x00000701, // v_mul_lo_u32 v1, v1, s3                                          // 00000000040C: 
// 0x68020202,             // v_add_u32_e32 v1, s2, v1                                         // 000000000414: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 000000000418: 
// 0x7E40031A,             // v_mov_b32_e32 v32, v26                                           // 00000000041C: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000420: 
// 0x7E42031E,             // v_mov_b32_e32 v33, v30                                           // 000000000424: 
// 0xBE8300FF, 0x00020000, // s_mov_b32 s3, 0x20000                                            // 000000000428: 
// 0xBE8200FF, 0x00014000, // s_mov_b32 s2, 0x14000                                            // 000000000430: 
// 0xD3CD8004, 0x04124116, // v_mfma_f32_16x16x16f16 a[4:7], v[22:23], v[32:33], a[4:7]        // 000000000438: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000440: 
// 0xBF8A0000,             // s_barrier                                                        // 000000000444: 
// 0xBF8C0F74,             // s_waitcnt vmcnt(4)                                               // 000000000448: 
// 0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                            // 00000000044C: 
// 0xBF8C0F72,             // s_waitcnt vmcnt(2)                                               // 000000000454: 
// 0xD2A00018, 0x00021106, // v_pack_b32_f16 v24, v6, v8                                       // 000000000458: 
// 0xD2A01819, 0x00021106, // v_pack_b32_f16 v25, v6, v8 op_sel:[1,1,0]                        // 000000000460: 
// 0x7E3C031B,             // v_mov_b32_e32 v30, v27                                           // 000000000468: 
// 0xD2A0001A, 0x00021307, // v_pack_b32_f16 v26, v7, v9                                       // 00000000046C: 
// 0xD2A0181B, 0x00021307, // v_pack_b32_f16 v27, v7, v9 op_sel:[1,1,0]                        // 000000000474: 
// 0xBF8C0F70,             // s_waitcnt vmcnt(0)                                               // 00000000047C: 
// 0xD2A00006, 0x00020902, // v_pack_b32_f16 v6, v2, v4                                        // 000000000480: 
// 0xD2A01807, 0x00020902, // v_pack_b32_f16 v7, v2, v4 op_sel:[1,1,0]                         // 000000000488: 
// 0xD2A00008, 0x00020B03, // v_pack_b32_f16 v8, v3, v5                                        // 000000000490: 
// 0xD2A01809, 0x00020B03, // v_pack_b32_f16 v9, v3, v5 op_sel:[1,1,0]                         // 000000000498: 
// 0xD9BE0000, 0x0000180E, // ds_write_b128 v14, v[24:27]                                      // 0000000004A0: 
// 0xD9BE0000, 0x0000060F, // ds_write_b128 v15, v[6:9]                                        // 0000000004A8: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000004B0: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000004B4: 
// 0xD9FE0000, 0x02000011, // ds_read_b128 v[2:5], v17                                         // 0000000004B8: 
// 0xD8700100, 0x06000010, // ds_read2st64_b32 v[6:7], v16 offset1:1                           // 0000000004C0: 
// 0xD86EC484, 0x08000010, // ds_read2_b32 v[8:9], v16 offset0:132 offset1:196                 // 0000000004C8: 
// 0xD8700504, 0x10000013, // ds_read2st64_b32 v[16:17], v19 offset0:4 offset1:5               // 0000000004D0: 
// 0xD8700706, 0x12000022, // ds_read2st64_b32 v[18:19], v34 offset0:6 offset1:7               // 0000000004D8: 
// 0xD3CD8000, 0x04023914, // v_mfma_f32_16x16x16f16 a[0:3], v[20:21], v[28:29], a[0:3]        // 0000000004E0: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000004E8: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000004EC: 
// 0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                             // 0000000004F0: 
// 0x7E1C0306,             // v_mov_b32_e32 v14, v6                                            // 0000000004F4: 
// 0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                             // 0000000004F8: 
// 0x7E1E0308,             // v_mov_b32_e32 v15, v8                                            // 0000000004FC: 
// 0x7E100307,             // v_mov_b32_e32 v8, v7                                             // 000000000500: 
// 0x260C188C,             // v_and_b32_e32 v6, 12, v12                                        // 000000000504: 
// 0xD2000000, 0x04011106, // v_lshl_or_b32 v0, v6, 8, v0                                      // 000000000508: 
// 0x24181682,             // v_lshlrev_b32_e32 v12, 2, v11                                    // 000000000510: 
// 0xD3CD8004, 0x04121D02, // v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[14:15], a[4:7]          // 000000000514: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 00000000051C: 
// 0x7E1E0312,             // v_mov_b32_e32 v15, v18                                           // 000000000520: 
// 0x7E1C0310,             // v_mov_b32_e32 v14, v16                                           // 000000000524: 
// 0x7E240311,             // v_mov_b32_e32 v18, v17                                           // 000000000528: 
// 0xD3CD8000, 0x04023D16, // v_mfma_f32_16x16x16f16 a[0:3], v[22:23], v[30:31], a[0:3]        // 00000000052C: 
// 0xD3CD8004, 0x04121D04, // v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[14:15], a[4:7]          // 000000000534: 
// 0xD3CD8000, 0x04021102, // v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[8:9], a[0:3]            // 00000000053C: 
// 0x24041684,             // v_lshlrev_b32_e32 v2, 4, v11                                     // 000000000544: 
// 0xD2000002, 0x0409110A, // v_lshl_or_b32 v2, v10, 8, v2                                     // 000000000548: 
// 0xBF800007,             // s_nop 7                                                          // 000000000550: 
// 0xDA1A0000, 0x00000400, // ds_write_b32 v0, a4                                              // 000000000554: 
// 0xDA1A0100, 0x00000500, // ds_write_b32 v0, a5 offset:256                                   // 00000000055C: 
// 0xDA1A0200, 0x00000600, // ds_write_b32 v0, a6 offset:512                                   // 000000000564: 
// 0xDA1A0300, 0x00000700, // ds_write_b32 v0, a7 offset:768                                   // 00000000056C: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000574: 
// 0xBF8A0000,             // s_barrier                                                        // 000000000578: 
// 0xD9FE0000, 0x06000002, // ds_read_b128 v[6:9], v2                                          // 00000000057C: 
// 0xD3CD8000, 0x04022504, // v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[18:19], a[0:3]          // 000000000584: 
// 0xD1FE0004, 0x02061901, // v_add_lshl_u32 v4, v1, v12, 1                                    // 00000000058C: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000594: 
// 0x7E061506,             // v_cvt_f16_f32_e32 v3, v6                                         // 000000000598: 
// 0x7E0C1507,             // v_cvt_f16_f32_e32 v6, v7                                         // 00000000059C: 
// 0x7E0E1508,             // v_cvt_f16_f32_e32 v7, v8                                         // 0000000005A0: 
// 0x7E101509,             // v_cvt_f16_f32_e32 v8, v9                                         // 0000000005A4: 
// 0xD2A00001, 0x00020D03, // v_pack_b32_f16 v1, v3, v6                                        // 0000000005A8: 
// 0xE1381000, 0x80000104, // buffer_atomic_pk_add_f16 v1, v4, s[0:3], 0 offen                 // 0000000005B0: 
// 0xD2A00001, 0x00021107, // v_pack_b32_f16 v1, v7, v8                                        // 0000000005B8: 
// 0xE1381000, 0x84000104, // buffer_atomic_pk_add_f16 v1, v4, s[0:3], 4 offen                 // 0000000005C0: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000005C8: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000005CC: 
// 0xBF800000,             // s_nop 0                                                          // 0000000005D0: 
// 0xDA1A0000, 0x00000000, // ds_write_b32 v0, a0                                              // 0000000005D4: 
// 0xDA1A0100, 0x00000100, // ds_write_b32 v0, a1 offset:256                                   // 0000000005DC: 
// 0xDA1A0200, 0x00000200, // ds_write_b32 v0, a2 offset:512                                   // 0000000005E4: 
// 0xDA1A0300, 0x00000300, // ds_write_b32 v0, a3 offset:768                                   // 0000000005EC: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000005F4: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000005F8: 
// 0xD9FE0000, 0x00000002, // ds_read_b128 v[0:3], v2                                          // 0000000005FC: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000604: 
// 0x7E001500,             // v_cvt_f16_f32_e32 v0, v0                                         // 000000000608: 
// 0x7E021501,             // v_cvt_f16_f32_e32 v1, v1                                         // 00000000060C: 
// 0x7E041502,             // v_cvt_f16_f32_e32 v2, v2                                         // 000000000610: 
// 0x7E061503,             // v_cvt_f16_f32_e32 v3, v3                                         // 000000000614: 
// 0xD2A00000, 0x00020300, // v_pack_b32_f16 v0, v0, v1                                        // 000000000618: 
// 0xE1381080, 0x80000004, // buffer_atomic_pk_add_f16 v0, v4, s[0:3], 0 offen offset:128      // 000000000620: 
// 0xD2A00000, 0x00020702, // v_pack_b32_f16 v0, v2, v3                                        // 000000000628: 
// 0xE1381080, 0x84000004, // buffer_atomic_pk_add_f16 v0, v4, s[0:3], 4 offen offset:128      // 000000000630: 
// 0xBF810000              // s_endpgm                                                         // 000000000638: 
// #endif
// };

const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_5120_1280[] = {
#if EMPTY_GEMM_KERNEL
0xBF810000,              // s_endpgm                                                         // 000000000660:
#else
//0xC00A0302, 0x00000000, // s_load_dwordx4 s[12:15], s[4:5], 0x0                           // 000000000000: 
//0xC00A0002, 0x00000010, // s_load_dwordx4 s[0:3], s[4:5], 0x10                            // 000000000008: 
0x20060085,             // v_lshrrev_b32_e32 v3, 5, v0                                    // 000000000010: 
0x24080082,             // v_lshlrev_b32_e32 v4, 2, v0                                    // 000000000014: 
//0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000018: 

0xBE8C0002,             //     s_mov_b32 s12, s2
0xBE8D0003,             //     s_mov_b32 s13, s3
0xBE8E0004,             //     s_mov_b32 s14, s4
0xBE8F0005,             //     s_mov_b32 s15, s5
0xBF800000,             // s_nop 0

0x8E038407,             // s_lshl_b32 s3, s7, 4                                           // 00000000001C: 
0x20160084,             // v_lshrrev_b32_e32 v11, 4, v0                                   // 000000000020: 
0x261808FF, 0x0000007C, // v_and_b32_e32 v12, 0x7c, v4                                    // 000000000024: 
0x100A06FF, 0x00002800, // v_mul_u32_u24_e32 v5, 0x2800, v3                               // 00000000002C: 
0x8E028706,             // s_lshl_b32 s2, s6, 7                                           // 000000000034: 
0x28261905,             // v_or_b32_e32 v19, v5, v12                                      // 000000000038: 
0x2602008F,             // v_and_b32_e32 v1, 15, v0                                       // 00000000003C: 
0x68141603,             // v_add_u32_e32 v10, s3, v11                                     // 000000000040: 
0xB0100500,             // s_movk_i32 s16, 0x500                                          // 000000000044: 
0xD1FE0012, 0x02040513, // v_add_lshl_u32 v18, v19, s2, 1                                 // 000000000048: 
0xBE8700FF, 0x00020000, // s_mov_b32 s7, 0x20000                                          // 000000000050: 
0x24040281,             // v_lshlrev_b32_e32 v2, 1, v1                                    // 000000000058: 
0xD2850004, 0x0000210A, // v_mul_lo_u32 v4, v10, s16                                      // 00000000005C: 
0x28080504,             // v_or_b32_e32 v4, v4, v2                                        // 000000000064: 
0xBE8A00FF, 0x01900000, // s_mov_b32 s10, 0x1900000                                       // 000000000068: 
0xBE88000E,             // s_mov_b32 s8, s14                                              // 000000000070: 
0xBE89000F,             // s_mov_b32 s9, s15                                              // 000000000074: 
0xBE8B0007,             // s_mov_b32 s11, s7                                              // 000000000078: 
0x681A24FF, 0x00002000, // v_add_u32_e32 v13, 0x2000, v18                                 // 00000000007C: 
0x24280881,             // v_lshlrev_b32_e32 v20, 1, v4                                   // 000000000084: 
0x681C24FF, 0x00028000, // v_add_u32_e32 v14, 0x28000, v18                                // 000000000088: 
0x681E24FF, 0x0002A000, // v_add_u32_e32 v15, 0x2a000, v18                                // 000000000090: 
0xE0541000, 0x80020412, // buffer_load_dwordx2 v[4:5], v18, s[8:11], 0 offen              // 000000000098: 
0xE0541800, 0x8002060D, // buffer_load_dwordx2 v[6:7], v13, s[8:11], 0 offen offset:2048  // 0000000000A0: 
0xE0541000, 0x8002080E, // buffer_load_dwordx2 v[8:9], v14, s[8:11], 0 offen              // 0000000000A8: 
0xE0541800, 0x8002100F, // buffer_load_dwordx2 v[16:17], v15, s[8:11], 0 offen offset:2048// 0000000000B0: 
0x241A0081,             // v_lshlrev_b32_e32 v13, 1, v0                                   // 0000000000B8: 
0xD2010002, 0x0409410D, // v_and_or_b32 v2, v13, 32, v2                                   // 0000000000BC: 
0xBE85000D,             // s_mov_b32 s5, s13                                              // 0000000000C4: 
0xB00D0050,             // s_movk_i32 s13, 0x50                                           // 0000000000C8: 
0x101A060D,             // v_mul_u32_u24_e32 v13, s13, v3                                 // 0000000000CC: 
0x24040481,             // v_lshlrev_b32_e32 v2, 1, v2                                    // 0000000000D0: 
0xB0030108,             // s_movk_i32 s3, 0x108                                           // 0000000000D4: 
0xD1FD000D, 0x0409030D, // v_lshl_add_u32 v13, v13, 1, v2                                 // 0000000000D8: 
0x10040603,             // v_mul_u32_u24_e32 v2, s3, v3                                   // 0000000000E0: 
0x24181881,             // v_lshlrev_b32_e32 v12, 1, v12                                  // 0000000000E4: 
0xD1FE0015, 0x0206050C, // v_add_lshl_u32 v21, v12, v2, 1                                 // 0000000000E8: 
0x28040688,             // v_or_b32_e32 v2, 8, v3                                         // 0000000000F0: 
0x10040403,             // v_mul_u32_u24_e32 v2, s3, v2                                   // 0000000000F4: 
0xBE84000C,             // s_mov_b32 s4, s12                                              // 0000000000F8: 
0xBE8300FF, 0x00050000, // s_mov_b32 s3, 0x50000                                          // 0000000000FC: 
0xBE8600FF, 0x00014000, // s_mov_b32 s6, 0x14000                                          // 000000000104: 
0xD1FE000C, 0x02061902, // v_add_lshl_u32 v12, v2, v12, 1                                 // 00000000010C: 
0x682C2403,             // v_add_u32_e32 v22, s3, v18                                     // 000000000114: 
0x682E24FF, 0x00052000, // v_add_u32_e32 v23, 0x52000, v18                                // 000000000118: 
0x683024FF, 0x00078000, // v_add_u32_e32 v24, 0x78000, v18                                // 000000000120: 
0x683224FF, 0x0007A000, // v_add_u32_e32 v25, 0x7a000, v18                                // 000000000128: 
0xE0501000, 0x80010214, // buffer_load_dword v2, v20, s[4:7], 0 offen                     // 000000000130: 
0xE0501040, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen offset:64          // 000000000138: 
0x681E1810,             // v_add_u32_e32 v15, s16, v12                                    // 000000000140: 
0xD3D94004, 0x18000080, // v_accvgpr_write_b32 a4, 0                                      // 000000000144: 
0x7E0AA504,             // v_accvgpr_mov_b32 a5, a4                                       // 00000000014C: 
0x7E0CA504,             // v_accvgpr_mov_b32 a6, a4                                       // 000000000150: 
0x7E0EA504,             // v_accvgpr_mov_b32 a7, a4                                       // 000000000154: 
0xD3D94000, 0x18000080, // v_accvgpr_write_b32 a0, 0                                      // 000000000158: 
0x24262681,             // v_lshlrev_b32_e32 v19, 1, v19                                  // 000000000160: 
0xBE8C0080,             // s_mov_b32 s12, 0                                               // 000000000164: 
0x681C2A10,             // v_add_u32_e32 v14, s16, v21                                    // 000000000168: 
0xD1FD0013, 0x044D0202, // v_lshl_add_u32 v19, s2, 1, v19                                 // 00000000016C: 
0x282828FF, 0x00000080, // v_or_b32_e32 v20, 0x80, v20                                    // 000000000174: 
0xBF8C0F71,             // s_waitcnt vmcnt(1)                                             // 00000000017C: 
0xD81A0000, 0x0000020D, // ds_write_b32 v13, v2                                           // 000000000180: 
0xD2A00002, 0x00020D04, // v_pack_b32_f16 v2, v4, v6                                      // 000000000188: 
0xD2A01803, 0x00020D04, // v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                       // 000000000190: 
0xD2A00004, 0x00020F05, // v_pack_b32_f16 v4, v5, v7                                      // 000000000198: 
0xD2A01805, 0x00020F05, // v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                       // 0000000001A0: 
0xD2A00006, 0x00022108, // v_pack_b32_f16 v6, v8, v16                                     // 0000000001A8: 
0xD2A01807, 0x00022108, // v_pack_b32_f16 v7, v8, v16 op_sel:[1,1,0]                      // 0000000001B0: 
0xD2A00008, 0x00022309, // v_pack_b32_f16 v8, v9, v17                                     // 0000000001B8: 
0xD2A01809, 0x00022309, // v_pack_b32_f16 v9, v9, v17 op_sel:[1,1,0]                      // 0000000001C0: 
0xD9BE0500, 0x00000215, // ds_write_b128 v21, v[2:5] offset:1280                          // 0000000001C8: 
0xD9BE0500, 0x0000060C, // ds_write_b128 v12, v[6:9] offset:1280                          // 0000000001D0: 
0xE0541000, 0x80020616, // buffer_load_dwordx2 v[6:7], v22, s[8:11], 0 offen              // 0000000001D8: 
0xE0541800, 0x80020817, // buffer_load_dwordx2 v[8:9], v23, s[8:11], 0 offen offset:2048  // 0000000001E0: 
0xE0541000, 0x80020218, // buffer_load_dwordx2 v[2:3], v24, s[8:11], 0 offen              // 0000000001E8: 
0xE0541800, 0x80020419, // buffer_load_dwordx2 v[4:5], v25, s[8:11], 0 offen offset:2048  // 0000000001F0: 
0x24220085,             // v_lshlrev_b32_e32 v17, 5, v0                                   // 0000000001F8: 
0xD1C8000C, 0x020D0300, // v_bfe_u32 v12, v0, 1, 3                                        // 0000000001FC: 
0xD1C80010, 0x02090900, // v_bfe_u32 v16, v0, 4, 2                                        // 000000000204: 
0x262222A0,             // v_and_b32_e32 v17, 32, v17                                     // 00000000020C: 
0xD2000011, 0x04450710, // v_lshl_or_b32 v17, v16, 3, v17                                 // 000000000210: 
0x1018180D,             // v_mul_u32_u24_e32 v12, s13, v12                                // 000000000218: 
0xD1FE0011, 0x02061911, // v_add_lshl_u32 v17, v17, v12, 1                                // 00000000021C: 
0x20180082,             // v_lshrrev_b32_e32 v12, 2, v0                                   // 000000000224: 
0xD3D84017, 0x18000105, // v_accvgpr_read_b32 v23, a5                                     // 000000000228: 
0xD2010000, 0x0405610C, // v_and_or_b32 v0, v12, 48, v1                                   // 000000000230: 
0x102020FF, 0x00000420, // v_mul_u32_u24_e32 v16, 0x420, v16                              // 000000000238: 
0xD1FD0010, 0x04410300, // v_lshl_add_u32 v16, v0, 1, v16                                 // 000000000240: 
0xD3D94001, 0x18000117, // v_accvgpr_write_b32 a1, v23                                    // 000000000248: 
0xD3D84017, 0x18000106, // v_accvgpr_read_b32 v23, a6                                     // 000000000250: 
0xD1FD0010, 0x00410310, // v_lshl_add_u32 v16, v16, 1, s16                                // 000000000258: 
0xD3D94002, 0x18000117, // v_accvgpr_write_b32 a2, v23                                    // 000000000260: 
0xD3D84017, 0x18000107, // v_accvgpr_read_b32 v23, a7                                     // 000000000268: 
0x682A20A0,             // v_add_u32_e32 v21, 32, v16                                     // 000000000270: 
0x682C20B0,             // v_add_u32_e32 v22, 48, v16                                     // 000000000274: 
0xD3D94003, 0x18000117, // v_accvgpr_write_b32 a3, v23                                    // 000000000278: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0) <BB0_1>                                   // 000000000280: 
0xBF8A0000,             // s_barrier                                                      // 000000000284: 
0xD9FE0000, 0x18000011, // ds_read_b128 v[24:27], v17                                     // 000000000288: 
0xD8700100, 0x24000010, // ds_read2st64_b32 v[36:37], v16 offset1:1                       // 000000000290: 
0xD86EC484, 0x26000010, // ds_read2_b32 v[38:39], v16 offset0:132 offset1:196             // 000000000298: 
0xD8700504, 0x28000015, // ds_read2st64_b32 v[40:41], v21 offset0:4 offset1:5             // 0000000002A0: 
0xD8700706, 0x2A000016, // ds_read2st64_b32 v[42:43], v22 offset0:6 offset1:7             // 0000000002A8: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000002B0: 
0xBF8A0000,             // s_barrier                                                      // 0000000002B4: 
0xBF8C0F74,             // s_waitcnt vmcnt(4)                                             // 0000000002B8: 
0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                          // 0000000002BC: 
0xE0501000, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen                    // 0000000002C4: 
0xBF8C0F73,             // s_waitcnt vmcnt(3)                                             // 0000000002CC: 
0xD2A0001C, 0x00021106, // v_pack_b32_f16 v28, v6, v8                                     // 0000000002D0: 
0xD2A0181D, 0x00021106, // v_pack_b32_f16 v29, v6, v8 op_sel:[1,1,0]                      // 0000000002D8: 
0xD2A0001E, 0x00021307, // v_pack_b32_f16 v30, v7, v9                                     // 0000000002E0: 
0xD2A0181F, 0x00021307, // v_pack_b32_f16 v31, v7, v9 op_sel:[1,1,0]                      // 0000000002E8: 
0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                           // 0000000002F0: 
0x7E0E0326,             // v_mov_b32_e32 v7, v38                                          // 0000000002F4: 
0x7E0C0324,             // v_mov_b32_e32 v6, v36                                          // 0000000002F8: 
0xBF8C0F71,             // s_waitcnt vmcnt(1)                                             // 0000000002FC: 
0xD2A00020, 0x00020902, // v_pack_b32_f16 v32, v2, v4                                     // 000000000300: 
0xD2A01821, 0x00020902, // v_pack_b32_f16 v33, v2, v4 op_sel:[1,1,0]                      // 000000000308: 
0x6804260C,             // v_add_u32_e32 v2, s12, v19                                     // 000000000310: 
0xD2A00022, 0x00020B03, // v_pack_b32_f16 v34, v3, v5                                     // 000000000314: 
0xD2A01823, 0x00020B03, // v_pack_b32_f16 v35, v3, v5 op_sel:[1,1,0]                      // 00000000031C: 
0x7E4C0325,             // v_mov_b32_e32 v38, v37                                         // 000000000324: 
0xD3CD8004, 0x04120D18, // v_mfma_f32_16x16x16f16 a[4:7], v[24:25], v[6:7], a[4:7]        // 000000000328: 
0x810C030C,             // s_add_i32 s12, s12, s3                                         // 000000000330: 
0x680804FF, 0x000A0000, // v_add_u32_e32 v4, 0xa0000, v2                                  // 000000000334: 
0x680A04FF, 0x000A2000, // v_add_u32_e32 v5, 0xa2000, v2                                  // 00000000033C: 
0x682E04FF, 0x000C8000, // v_add_u32_e32 v23, 0xc8000, v2                                 // 000000000344: 
0xBF07FF0C, 0x00BE0000, // s_cmp_lg_u32 s12, 0xbe0000                                     // 00000000034C: 
0x682828C0,             // v_add_u32_e32 v20, 64, v20                                     // 000000000354: 
0xD3CD8000, 0x04024D18, // v_mfma_f32_16x16x16f16 a[0:3], v[24:25], v[38:39], a[0:3]      // 000000000358: 
0x683004FF, 0x000CA000, // v_add_u32_e32 v24, 0xca000, v2                                 // 000000000360: 
0xE0541000, 0x80020604, // buffer_load_dwordx2 v[6:7], v4, s[8:11], 0 offen               // 000000000368: 
0xE0541800, 0x80020805, // buffer_load_dwordx2 v[8:9], v5, s[8:11], 0 offen offset:2048   // 000000000370: 
0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen              // 000000000378: 
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 000000000380: 
0x7E0A032A,             // v_mov_b32_e32 v5, v42                                          // 000000000384: 
0x7E080328,             // v_mov_b32_e32 v4, v40                                          // 000000000388: 
0x7E540329,             // v_mov_b32_e32 v42, v41                                         // 00000000038C: 
0xD9BE0000, 0x00001C0E, // ds_write_b128 v14, v[28:31]                                    // 000000000390: 
0xD3CD8004, 0x0412091A, // v_mfma_f32_16x16x16f16 a[4:7], v[26:27], v[4:5], a[4:7]        // 000000000398: 
0xE0541800, 0x80020418, // buffer_load_dwordx2 v[4:5], v24, s[8:11], 0 offen offset:2048  // 0000000003A0: 
0xD9BE0000, 0x0000200F, // ds_write_b128 v15, v[32:35]                                    // 0000000003A8: 
0xD3CD8000, 0x0402551A, // v_mfma_f32_16x16x16f16 a[0:3], v[26:27], v[42:43], a[0:3]      // 0000000003B0: 
0xBF85FFB1,             // s_cbranch_scc1 BB0_1                                           // 0000000003B8: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000003BC: 
0xBF8A0000,             // s_barrier                                                      // 0000000003C0: 
0xD9FE0000, 0x14000011, // ds_read_b128 v[20:23], v17                                     // 0000000003C4: 
0xD8700100, 0x18000010, // ds_read2st64_b32 v[24:25], v16 offset1:1                       // 0000000003CC: 
0xD86EC484, 0x1C000010, // ds_read2_b32 v[28:29], v16 offset0:132 offset1:196             // 0000000003D4: 
0x682620A0,             // v_add_u32_e32 v19, 32, v16                                     // 0000000003DC: 
0x684420B0,             // v_add_u32_e32 v34, 48, v16                                     // 0000000003E0: 
0x24000082,             // v_lshlrev_b32_e32 v0, 2, v0                                    // 0000000003E4: 
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 0000000003E8: 
0x7E340318,             // v_mov_b32_e32 v26, v24                                         // 0000000003EC: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000003F0: 
0x7E36031C,             // v_mov_b32_e32 v27, v28                                         // 0000000003F4: 
0x7E380319,             // v_mov_b32_e32 v28, v25                                         // 0000000003F8: 
0xBF800000,             // s_nop 0                                                        // 0000000003FC: 
0xD3CD8004, 0x04123514, // v_mfma_f32_16x16x16f16 a[4:7], v[20:21], v[26:27], a[4:7]      // 000000000400: 
0xD8700504, 0x1A000013, // ds_read2st64_b32 v[26:27], v19 offset0:4 offset1:5             // 000000000408: 
0xD8700706, 0x1E000022, // ds_read2st64_b32 v[30:31], v34 offset0:6 offset1:7             // 000000000410: 
0xB0031400,             // s_movk_i32 s3, 0x1400                                          // 000000000418: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 00000000041C: 
0xBF8A0000,             // s_barrier                                                      // 000000000420: 
0xBF8C0F74,             // s_waitcnt vmcnt(4)                                             // 000000000424: 
0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                          // 000000000428: 
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 000000000430: 
0x7E40031A,             // v_mov_b32_e32 v32, v26                                         // 000000000434: 
0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                           // 000000000438: 
0x7E42031E,             // v_mov_b32_e32 v33, v30                                         // 00000000043C: 
0xBF8C0F72,             // s_waitcnt vmcnt(2)                                             // 000000000440: 
0xD2A00018, 0x00021106, // v_pack_b32_f16 v24, v6, v8                                     // 000000000444: 
0xD2A01819, 0x00021106, // v_pack_b32_f16 v25, v6, v8 op_sel:[1,1,0]                      // 00000000044C: 
0x7E3C031B,             // v_mov_b32_e32 v30, v27                                         // 000000000454: 
0xD2A0001A, 0x00021307, // v_pack_b32_f16 v26, v7, v9                                     // 000000000458: 
0xD2A0181B, 0x00021307, // v_pack_b32_f16 v27, v7, v9 op_sel:[1,1,0]                      // 000000000460: 
0xBF8C0F70,             // s_waitcnt vmcnt(0)                                             // 000000000468: 
0xD2A00006, 0x00020902, // v_pack_b32_f16 v6, v2, v4                                      // 00000000046C: 
0xD2A01807, 0x00020902, // v_pack_b32_f16 v7, v2, v4 op_sel:[1,1,0]                       // 000000000474: 
0xD2A00008, 0x00020B03, // v_pack_b32_f16 v8, v3, v5                                      // 00000000047C: 
0xD2A01809, 0x00020B03, // v_pack_b32_f16 v9, v3, v5 op_sel:[1,1,0]                       // 000000000484: 
0xD9BE0000, 0x0000180E, // ds_write_b128 v14, v[24:27]                                    // 00000000048C: 
0xD9BE0000, 0x0000060F, // ds_write_b128 v15, v[6:9]                                      // 000000000494: 
0xD3CD8004, 0x04124116, // v_mfma_f32_16x16x16f16 a[4:7], v[22:23], v[32:33], a[4:7]      // 00000000049C: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000004A4: 
0xBF8A0000,             // s_barrier                                                      // 0000000004A8: 
0xD9FE0000, 0x02000011, // ds_read_b128 v[2:5], v17                                       // 0000000004AC: 
0xD8700100, 0x06000010, // ds_read2st64_b32 v[6:7], v16 offset1:1                         // 0000000004B4: 
0xD86EC484, 0x08000010, // ds_read2_b32 v[8:9], v16 offset0:132 offset1:196               // 0000000004BC: 
0xD8700504, 0x10000013, // ds_read2st64_b32 v[16:17], v19 offset0:4 offset1:5             // 0000000004C4: 
0xD8700706, 0x12000022, // ds_read2st64_b32 v[18:19], v34 offset0:6 offset1:7             // 0000000004CC: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000004D4: 
0xBF8A0000,             // s_barrier                                                      // 0000000004D8: 
0xD3CD8000, 0x04023914, // v_mfma_f32_16x16x16f16 a[0:3], v[20:21], v[28:29], a[0:3]      // 0000000004DC: 
0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                           // 0000000004E4: 
0x7E1C0306,             // v_mov_b32_e32 v14, v6                                          // 0000000004E8: 
0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                           // 0000000004EC: 
0x7E1E0308,             // v_mov_b32_e32 v15, v8                                          // 0000000004F0: 
0x7E100307,             // v_mov_b32_e32 v8, v7                                           // 0000000004F4: 
0x260C188C,             // v_and_b32_e32 v6, 12, v12                                      // 0000000004F8: 
0xD200000C, 0x04011106, // v_lshl_or_b32 v12, v6, 8, v0                                   // 0000000004FC: 
0x24000284,             // v_lshlrev_b32_e32 v0, 4, v1                                    // 000000000504: 
0xD3CD8004, 0x04121D02, // v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[14:15], a[4:7]        // 000000000508: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000510: 
0x7E1E0312,             // v_mov_b32_e32 v15, v18                                         // 000000000514: 
0x7E1C0310,             // v_mov_b32_e32 v14, v16                                         // 000000000518: 
0x7E240311,             // v_mov_b32_e32 v18, v17                                         // 00000000051C: 
0xD3CD8000, 0x04023D16, // v_mfma_f32_16x16x16f16 a[0:3], v[22:23], v[30:31], a[0:3]      // 000000000520: 
0xD3CD8004, 0x04121D04, // v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[14:15], a[4:7]        // 000000000528: 
0xD3CD8000, 0x04021102, // v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[8:9], a[0:3]          // 000000000530: 
0xD2000002, 0x0401110B, // v_lshl_or_b32 v2, v11, 8, v0                                   // 000000000538: 
0xBF800007,             // s_nop 7                                                        // 000000000540: 
0xBF800000,             // s_nop 0                                                        // 000000000544: 
0xDA1A0000, 0x0000040C, // ds_write_b32 v12, a4                                           // 000000000548: 
0xDA1A0100, 0x0000050C, // ds_write_b32 v12, a5 offset:256                                // 000000000550: 
0xDA1A0200, 0x0000060C, // ds_write_b32 v12, a6 offset:512                                // 000000000558: 
0xDA1A0300, 0x0000070C, // ds_write_b32 v12, a7 offset:768                                // 000000000560: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000568: 
0xBF8A0000,             // s_barrier                                                      // 00000000056C: 
0xD9FE0000, 0x06000002, // ds_read_b128 v[6:9], v2                                        // 000000000570: 
0xD3CD8000, 0x04022504, // v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[18:19], a[0:3]        // 000000000578: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 000000000580: 
0x7E001506,             // v_cvt_f16_f32_e32 v0, v6                                       // 000000000584: 
0x7E061508,             // v_cvt_f16_f32_e32 v3, v8                                       // 000000000588: 
0x7E0C1509,             // v_cvt_f16_f32_e32 v6, v9                                       // 00000000058C: 
0x7E0E1507,             // v_cvt_f16_f32_e32 v7, v7                                       // 000000000590: 
0x24100282,             // v_lshlrev_b32_e32 v8, 2, v1                                    // 000000000594: 
0xD2A00001, 0x00020D03, // v_pack_b32_f16 v1, v3, v6                                      // 000000000598: 
0xD2850003, 0x0000070A, // v_mul_lo_u32 v3, v10, s3                                       // 0000000005A0: 
0x68060602,             // v_add_u32_e32 v3, s2, v3                                       // 0000000005A8: 
0xD2A00000, 0x00020F00, // v_pack_b32_f16 v0, v0, v7                                      // 0000000005AC: 
0xBE8300FF, 0x00020000, // s_mov_b32 s3, 0x20000                                          // 0000000005B4: 
0xBE8200FF, 0x00050000, // s_mov_b32 s2, 0x50000                                          // 0000000005BC: 
0xD1FE0004, 0x02061103, // v_add_lshl_u32 v4, v3, v8, 1                                   // 0000000005C4: 
0xE0741000, 0x80000004, // buffer_store_dwordx2 v[0:1], v4, s[0:3], 0 offen               // 0000000005CC: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005D4: 
0xBF8A0000,             // s_barrier                                                      // 0000000005D8: 
0xDA1A0000, 0x0000000C, // ds_write_b32 v12, a0                                           // 0000000005DC: 
0xDA1A0100, 0x0000010C, // ds_write_b32 v12, a1 offset:256                                // 0000000005E4: 
0xDA1A0200, 0x0000020C, // ds_write_b32 v12, a2 offset:512                                // 0000000005EC: 
0xDA1A0300, 0x0000030C, // ds_write_b32 v12, a3 offset:768                                // 0000000005F4: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 0000000005FC: 
0xBF8A0000,             // s_barrier                                                      // 000000000600: 
0xD9FE0000, 0x00000002, // ds_read_b128 v[0:3], v2                                        // 000000000604: 
0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                           // 00000000060C: 
0x7E001500,             // v_cvt_f16_f32_e32 v0, v0                                       // 000000000610: 
0x7E0A1501,             // v_cvt_f16_f32_e32 v5, v1                                       // 000000000614: 
0x7E041502,             // v_cvt_f16_f32_e32 v2, v2                                       // 000000000618: 
0x7E061503,             // v_cvt_f16_f32_e32 v3, v3                                       // 00000000061C: 
0xD2A00000, 0x00020B00, // v_pack_b32_f16 v0, v0, v5                                      // 000000000620: 
0xD2A00001, 0x00020702, // v_pack_b32_f16 v1, v2, v3                                      // 000000000628: 
0xE0741080, 0x80000004, // buffer_store_dwordx2 v[0:1], v4, s[0:3], 0 offen offset:128    // 000000000630: 
0xBF810000              // s_endpgm                                                       // 000000000638: 
#endif
};

// GEMM kernels using atomic instructions (buffer_atomic_pk_add_f16) not working for some reason.
//
// const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_5120_1280[] = {
// #if EMPTY_GEMM_KERNEL
// 0xBF810000,              // s_endpgm                                                         // 000000000660:
// #else
// // Original ISA:
// // s4-s5 : kernarg pointer
// // s6 : workgroup_id_x
// // s7 : workgroup_id_y
// // s8 : workgroup_id_z
// //0xC00A0302, 0x00000000, // s_load_dwordx4 s[12:15], s[4:5], 0x0                             // 000000000000:
// //0xC00A0002, 0x00000010, // s_load_dwordx4 s[0:3], s[4:5], 0x10                              // 000000000008:
// //0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000010:
// 
// // Hacked ISA
// // s0-s1: matrix C pointer
// // s2-s3: matrix A pointer
// // s4-s5: matrix B pointer
// // s6 : workgroup_id_x
// // s7 : workgroup_id_y
// // s8 : workgroup_id_z
// 0xBE8C0002,             //     s_mov_b32 s12, s2
// 0xBE8D0003,             //     s_mov_b32 s13, s3
// 0xBE8E0004,             //     s_mov_b32 s14, s4
// 0xBE8F0005,             //     s_mov_b32_s15, s5
// 0xBF800000,             // s_nop 0
// 
// // The rest of GEMM algorithm.
// 0x8E038407,             // s_lshl_b32 s3, s7, 4                                             // 000000000014: 
// 0x20140084,             // v_lshrrev_b32_e32 v10, 4, v0                                     // 000000000018: 
// 0x24020082,             // v_lshlrev_b32_e32 v1, 2, v0                                      // 00000000001C: 
// 0x261802FF, 0x0000007C, // v_and_b32_e32 v12, 0x7c, v1                                      // 000000000020: 
// 0x68021403,             // v_add_u32_e32 v1, s3, v10                                        // 000000000028: 
// 0xB0100500,             // s_movk_i32 s16, 0x500                                            // 00000000002C: 
// 0x8E038508,             // s_lshl_b32 s3, s8, 5                                             // 000000000030: 
// 0xD2850004, 0x00002101, // v_mul_lo_u32 v4, v1, s16                                         // 000000000034: 
// 0x2606148E,             // v_and_b32_e32 v3, 14, v10                                        // 00000000003C: 
// 0x68080803,             // v_add_u32_e32 v4, s3, v4                                         // 000000000040: 
// 0x280A0603,             // v_or_b32_e32 v5, s3, v3                                          // 000000000044: 
// 0xB0031400,             // s_movk_i32 s3, 0x1400                                            // 000000000048: 
// 0xD2850005, 0x00000705, // v_mul_lo_u32 v5, v5, s3                                          // 00000000004C: 
// 0x8E028706,             // s_lshl_b32 s2, s6, 7                                             // 000000000054: 
// 0x28261905,             // v_or_b32_e32 v19, v5, v12                                        // 000000000058: 
// 0x2616008F,             // v_and_b32_e32 v11, 15, v0                                        // 00000000005C: 
// 0x24041681,             // v_lshlrev_b32_e32 v2, 1, v11                                     // 000000000060: 
// 0xBE8700FF, 0x00020000, // s_mov_b32 s7, 0x20000                                            // 000000000064: 
// 0xBE85000D,             // s_mov_b32 s5, s13                                                // 00000000006C: 
// 0xD1FE0015, 0x02040513, // v_add_lshl_u32 v21, v19, s2, 1                                   // 000000000070: 
// 0xB00D2000,             // s_movk_i32 s13, 0x2000                                           // 000000000078: 
// 0x28080504,             // v_or_b32_e32 v4, v4, v2                                          // 00000000007C: 
// 0xBE88000E,             // s_mov_b32 s8, s14                                                // 000000000080: 
// 0xBE89000F,             // s_mov_b32 s9, s15                                                // 000000000084: 
// 0xBE8E00FF, 0x00028000, // s_mov_b32 s14, 0x28000                                           // 000000000088: 
// 0xBE8F00FF, 0x0002A000, // s_mov_b32 s15, 0x2a000                                           // 000000000090: 
// 0xBE8A00FF, 0x01900000, // s_mov_b32 s10, 0x1900000                                         // 000000000098: 
// 0xBE8B0007,             // s_mov_b32 s11, s7                                                // 0000000000A0: 
// 0x681A2A0D,             // v_add_u32_e32 v13, s13, v21                                      // 0000000000A4: 
// 0x24280881,             // v_lshlrev_b32_e32 v20, 1, v4                                     // 0000000000A8: 
// 0x681C2A0E,             // v_add_u32_e32 v14, s14, v21                                      // 0000000000AC: 
// 0x681E2A0F,             // v_add_u32_e32 v15, s15, v21                                      // 0000000000B0: 
// 0xE0541000, 0x80020415, // buffer_load_dwordx2 v[4:5], v21, s[8:11], 0 offen                // 0000000000B4: 
// 0xE0541800, 0x8002060D, // buffer_load_dwordx2 v[6:7], v13, s[8:11], 0 offen offset:2048    // 0000000000BC: 
// 0xE0541000, 0x8002080E, // buffer_load_dwordx2 v[8:9], v14, s[8:11], 0 offen                // 0000000000C4: 
// 0xE0541800, 0x8002100F, // buffer_load_dwordx2 v[16:17], v15, s[8:11], 0 offen offset:2048  // 0000000000CC: 
// 0x241A0081,             // v_lshlrev_b32_e32 v13, 1, v0                                     // 0000000000D4: 
// 0xD2010002, 0x0409410D, // v_and_or_b32 v2, v13, 32, v2                                     // 0000000000D8: 
// 0x201C0085,             // v_lshrrev_b32_e32 v14, 5, v0                                     // 0000000000E0: 
// 0xB0110050,             // s_movk_i32 s17, 0x50                                             // 0000000000E4: 
// 0x101A1C11,             // v_mul_u32_u24_e32 v13, s17, v14                                  // 0000000000E8: 
// 0x24040481,             // v_lshlrev_b32_e32 v2, 1, v2                                      // 0000000000EC: 
// 0xB0030108,             // s_movk_i32 s3, 0x108                                             // 0000000000F0: 
// 0xD1FD000D, 0x0409030D, // v_lshl_add_u32 v13, v13, 1, v2                                   // 0000000000F4: 
// 0x10041C03,             // v_mul_u32_u24_e32 v2, s3, v14                                    // 0000000000FC: 
// 0x24181881,             // v_lshlrev_b32_e32 v12, 1, v12                                    // 000000000100: 
// 0xD1FE0016, 0x0206050C, // v_add_lshl_u32 v22, v12, v2, 1                                   // 000000000104: 
// 0x28040690,             // v_or_b32_e32 v2, 16, v3                                          // 00000000010C: 
// 0x20040481,             // v_lshrrev_b32_e32 v2, 1, v2                                      // 000000000110: 
// 0x10040403,             // v_mul_u32_u24_e32 v2, s3, v2                                     // 000000000114: 
// 0xBE84000C,             // s_mov_b32 s4, s12                                                // 000000000118: 
// 0xBE8600FF, 0x00014000, // s_mov_b32 s6, 0x14000                                            // 00000000011C: 
// 0xD1FE000C, 0x02061902, // v_add_lshl_u32 v12, v2, v12, 1                                   // 000000000124: 
// 0xE0501000, 0x80010214, // buffer_load_dword v2, v20, s[4:7], 0 offen                       // 00000000012C: 
// 0xE0501200, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen offset:512           // 000000000134: 
// 0xBE8300FF, 0x00280000, // s_mov_b32 s3, 0x280000                                           // 00000000013C: 
// 0x682A2A03,             // v_add_u32_e32 v21, s3, v21                                       // 000000000144: 
// 0x682E2A0D,             // v_add_u32_e32 v23, s13, v21                                      // 000000000148: 
// 0x68302A0E,             // v_add_u32_e32 v24, s14, v21                                      // 00000000014C: 
// 0x68322A0F,             // v_add_u32_e32 v25, s15, v21                                      // 000000000150: 
// 0x681E1810,             // v_add_u32_e32 v15, s16, v12                                      // 000000000154: 
// 0xD3D94004, 0x18000080, // v_accvgpr_write_b32 a4, 0                                        // 000000000158: 
// 0x7E0AA504,             // v_accvgpr_mov_b32 a5, a4                                         // 000000000160: 
// 0x7E0CA504,             // v_accvgpr_mov_b32 a6, a4                                         // 000000000164: 
// 0x7E0EA504,             // v_accvgpr_mov_b32 a7, a4                                         // 000000000168: 
// 0xD3D94000, 0x18000080, // v_accvgpr_write_b32 a0, 0                                        // 00000000016C: 
// 0x24262681,             // v_lshlrev_b32_e32 v19, 1, v19                                    // 000000000174: 
// 0xBE8C0080,             // s_mov_b32 s12, 0                                                 // 000000000178: 
// 0x681C2C10,             // v_add_u32_e32 v14, s16, v22                                      // 00000000017C: 
// 0xD1FD0013, 0x044D0202, // v_lshl_add_u32 v19, s2, 1, v19                                   // 000000000180: 
// 0x682828FF, 0x00000400, // v_add_u32_e32 v20, 0x400, v20                                    // 000000000188: 
// 0xBF8C0F71,             // s_waitcnt vmcnt(1)                                               // 000000000190: 
// 0xD81A0000, 0x0000020D, // ds_write_b32 v13, v2                                             // 000000000194: 
// 0xD2A00002, 0x00020D04, // v_pack_b32_f16 v2, v4, v6                                        // 00000000019C: 
// 0xD2A01803, 0x00020D04, // v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                         // 0000000001A4: 
// 0xD2A00004, 0x00020F05, // v_pack_b32_f16 v4, v5, v7                                        // 0000000001AC: 
// 0xD2A01805, 0x00020F05, // v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                         // 0000000001B4: 
// 0xD2A00006, 0x00022108, // v_pack_b32_f16 v6, v8, v16                                       // 0000000001BC: 
// 0xD2A01807, 0x00022108, // v_pack_b32_f16 v7, v8, v16 op_sel:[1,1,0]                        // 0000000001C4: 
// 0xD2A00008, 0x00022309, // v_pack_b32_f16 v8, v9, v17                                       // 0000000001CC: 
// 0xD2A01809, 0x00022309, // v_pack_b32_f16 v9, v9, v17 op_sel:[1,1,0]                        // 0000000001D4: 
// 0xD9BE0500, 0x00000216, // ds_write_b128 v22, v[2:5] offset:1280                            // 0000000001DC: 
// 0xD9BE0500, 0x0000060C, // ds_write_b128 v12, v[6:9] offset:1280                            // 0000000001E4: 
// 0xE0541000, 0x80020615, // buffer_load_dwordx2 v[6:7], v21, s[8:11], 0 offen                // 0000000001EC: 
// 0xE0541800, 0x80020817, // buffer_load_dwordx2 v[8:9], v23, s[8:11], 0 offen offset:2048    // 0000000001F4: 
// 0xE0541000, 0x80020218, // buffer_load_dwordx2 v[2:3], v24, s[8:11], 0 offen                // 0000000001FC: 
// 0xE0541800, 0x80020419, // buffer_load_dwordx2 v[4:5], v25, s[8:11], 0 offen offset:2048    // 000000000204: 
// 0x24220085,             // v_lshlrev_b32_e32 v17, 5, v0                                     // 00000000020C: 
// 0xD1C8000C, 0x020D0300, // v_bfe_u32 v12, v0, 1, 3                                          // 000000000210: 
// 0xD1C80010, 0x02090900, // v_bfe_u32 v16, v0, 4, 2                                          // 000000000218: 
// 0x262222A0,             // v_and_b32_e32 v17, 32, v17                                       // 000000000220: 
// 0xD2000011, 0x04450710, // v_lshl_or_b32 v17, v16, 3, v17                                   // 000000000224: 
// 0x10181811,             // v_mul_u32_u24_e32 v12, s17, v12                                  // 00000000022C: 
// 0xD1FE0011, 0x02061911, // v_add_lshl_u32 v17, v17, v12, 1                                  // 000000000230: 
// 0x20180082,             // v_lshrrev_b32_e32 v12, 2, v0                                     // 000000000238: 
// 0xD3D84017, 0x18000105, // v_accvgpr_read_b32 v23, a5                                       // 00000000023C: 
// 0xD2010000, 0x042D610C, // v_and_or_b32 v0, v12, 48, v11                                    // 000000000244: 
// 0x102020FF, 0x00000420, // v_mul_u32_u24_e32 v16, 0x420, v16                                // 00000000024C: 
// 0xD1FD0010, 0x04410300, // v_lshl_add_u32 v16, v0, 1, v16                                   // 000000000254: 
// 0xD3D94001, 0x18000117, // v_accvgpr_write_b32 a1, v23                                      // 00000000025C: 
// 0xD3D84017, 0x18000106, // v_accvgpr_read_b32 v23, a6                                       // 000000000264: 
// 0xD1FD0010, 0x00410310, // v_lshl_add_u32 v16, v16, 1, s16                                  // 00000000026C: 
// 0xD3D94002, 0x18000117, // v_accvgpr_write_b32 a2, v23                                      // 000000000274: 
// 0xD3D84017, 0x18000107, // v_accvgpr_read_b32 v23, a7                                       // 00000000027C: 
// 0x682A20A0,             // v_add_u32_e32 v21, 32, v16                                       // 000000000284: 
// 0x682C20B0,             // v_add_u32_e32 v22, 48, v16                                       // 000000000288: 
// 0xD3D94003, 0x18000117, // v_accvgpr_write_b32 a3, v23                                      // 00000000028C: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0) // 294 <BB0_1>                              // 000000000294: 
// 0xBF8A0000,             // s_barrier                                                        // 000000000298: 
// 0xD9FE0000, 0x18000011, // ds_read_b128 v[24:27], v17                                       // 00000000029C: 
// 0xD8700100, 0x24000010, // ds_read2st64_b32 v[36:37], v16 offset1:1                         // 0000000002A4: 
// 0xD86EC484, 0x26000010, // ds_read2_b32 v[38:39], v16 offset0:132 offset1:196               // 0000000002AC: 
// 0xD8700504, 0x28000015, // ds_read2st64_b32 v[40:41], v21 offset0:4 offset1:5               // 0000000002B4: 
// 0xD8700706, 0x2A000016, // ds_read2st64_b32 v[42:43], v22 offset0:6 offset1:7               // 0000000002BC: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000002C4: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000002C8: 
// 0xBF8C0F74,             // s_waitcnt vmcnt(4)                                               // 0000000002CC: 
// 0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                            // 0000000002D0: 
// 0xE0501000, 0x80011214, // buffer_load_dword v18, v20, s[4:7], 0 offen                      // 0000000002D8: 
// 0xBF8C0F73,             // s_waitcnt vmcnt(3)                                               // 0000000002E0: 
// 0xD2A0001C, 0x00021106, // v_pack_b32_f16 v28, v6, v8                                       // 0000000002E4: 
// 0xD2A0181D, 0x00021106, // v_pack_b32_f16 v29, v6, v8 op_sel:[1,1,0]                        // 0000000002EC: 
// 0xD2A0001E, 0x00021307, // v_pack_b32_f16 v30, v7, v9                                       // 0000000002F4: 
// 0xD2A0181F, 0x00021307, // v_pack_b32_f16 v31, v7, v9 op_sel:[1,1,0]                        // 0000000002FC: 
// 0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                             // 000000000304: 
// 0x7E0E0326,             // v_mov_b32_e32 v7, v38                                            // 000000000308: 
// 0x7E0C0324,             // v_mov_b32_e32 v6, v36                                            // 00000000030C: 
// 0xBF8C0F71,             // s_waitcnt vmcnt(1)                                               // 000000000310: 
// 0xD2A00020, 0x00020902, // v_pack_b32_f16 v32, v2, v4                                       // 000000000314: 
// 0xD2A01821, 0x00020902, // v_pack_b32_f16 v33, v2, v4 op_sel:[1,1,0]                        // 00000000031C: 
// 0x6804260C,             // v_add_u32_e32 v2, s12, v19                                       // 000000000324: 
// 0xD2A00022, 0x00020B03, // v_pack_b32_f16 v34, v3, v5                                       // 000000000328: 
// 0xD2A01823, 0x00020B03, // v_pack_b32_f16 v35, v3, v5 op_sel:[1,1,0]                        // 000000000330: 
// 0x7E4C0325,             // v_mov_b32_e32 v38, v37                                           // 000000000338: 
// 0xD3CD8004, 0x04120D18, // v_mfma_f32_16x16x16f16 a[4:7], v[24:25], v[6:7], a[4:7]          // 00000000033C: 
// 0x810C030C,             // s_add_i32 s12, s12, s3                                           // 000000000344: 
// 0x680804FF, 0x00500000, // v_add_u32_e32 v4, 0x500000, v2                                   // 000000000348: 
// 0x680A04FF, 0x00502000, // v_add_u32_e32 v5, 0x502000, v2                                   // 000000000350: 
// 0x682E04FF, 0x00528000, // v_add_u32_e32 v23, 0x528000, v2                                  // 000000000358: 
// 0xBF07FF0C, 0x00780000, // s_cmp_lg_u32 s12, 0x780000                                       // 000000000360: 
// 0x682828FF, 0x00000200, // v_add_u32_e32 v20, 0x200, v20                                    // 000000000368: 
// 0xD3CD8000, 0x04024D18, // v_mfma_f32_16x16x16f16 a[0:3], v[24:25], v[38:39], a[0:3]        // 000000000370: 
// 0x683004FF, 0x0052A000, // v_add_u32_e32 v24, 0x52a000, v2                                  // 000000000378: 
// 0xE0541000, 0x80020604, // buffer_load_dwordx2 v[6:7], v4, s[8:11], 0 offen                 // 000000000380: 
// 0xE0541800, 0x80020805, // buffer_load_dwordx2 v[8:9], v5, s[8:11], 0 offen offset:2048     // 000000000388: 
// 0xE0541000, 0x80020217, // buffer_load_dwordx2 v[2:3], v23, s[8:11], 0 offen                // 000000000390: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 000000000398: 
// 0x7E0A032A,             // v_mov_b32_e32 v5, v42                                            // 00000000039C: 
// 0x7E080328,             // v_mov_b32_e32 v4, v40                                            // 0000000003A0: 
// 0x7E540329,             // v_mov_b32_e32 v42, v41                                           // 0000000003A4: 
// 0xD9BE0000, 0x00001C0E, // ds_write_b128 v14, v[28:31]                                      // 0000000003A8: 
// 0xD3CD8004, 0x0412091A, // v_mfma_f32_16x16x16f16 a[4:7], v[26:27], v[4:5], a[4:7]          // 0000000003B0: 
// 0xE0541800, 0x80020418, // buffer_load_dwordx2 v[4:5], v24, s[8:11], 0 offen offset:2048    // 0000000003B8: 
// 0xD9BE0000, 0x0000200F, // ds_write_b128 v15, v[32:35]                                      // 0000000003C0: 
// 0xD3CD8000, 0x0402551A, // v_mfma_f32_16x16x16f16 a[0:3], v[26:27], v[42:43], a[0:3]        // 0000000003C8: 
// 0xBF85FFB0,             // s_cbranch_scc1 BB0_1                                             // 0000000003D0: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000003D4: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000003D8: 
// 0xD9FE0000, 0x14000011, // ds_read_b128 v[20:23], v17                                       // 0000000003DC: 
// 0xD8700100, 0x18000010, // ds_read2st64_b32 v[24:25], v16 offset1:1                         // 0000000003E4: 
// 0xD86EC484, 0x1C000010, // ds_read2_b32 v[28:29], v16 offset0:132 offset1:196               // 0000000003EC: 
// 0x682620A0,             // v_add_u32_e32 v19, 32, v16                                       // 0000000003F4: 
// 0x684420B0,             // v_add_u32_e32 v34, 48, v16                                       // 0000000003F8: 
// 0x24000082,             // v_lshlrev_b32_e32 v0, 2, v0                                      // 0000000003FC: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 000000000400: 
// 0x7E340318,             // v_mov_b32_e32 v26, v24                                           // 000000000404: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000408: 
// 0x7E36031C,             // v_mov_b32_e32 v27, v28                                           // 00000000040C: 
// 0x7E380319,             // v_mov_b32_e32 v28, v25                                           // 000000000410: 
// 0xBF800000,             // s_nop 0                                                          // 000000000414: 
// 0xD3CD8004, 0x04123514, // v_mfma_f32_16x16x16f16 a[4:7], v[20:21], v[26:27], a[4:7]        // 000000000418: 
// 0xD8700504, 0x1A000013, // ds_read2st64_b32 v[26:27], v19 offset0:4 offset1:5               // 000000000420: 
// 0xD8700706, 0x1E000022, // ds_read2st64_b32 v[30:31], v34 offset0:6 offset1:7               // 000000000428: 
// 0xB0031400,             // s_movk_i32 s3, 0x1400                                            // 000000000430: 
// 0xD2850001, 0x00000701, // v_mul_lo_u32 v1, v1, s3                                          // 000000000434: 
// 0x68020202,             // v_add_u32_e32 v1, s2, v1                                         // 00000000043C: 
// 0xBF8CC17F,             // s_waitcnt lgkmcnt(1)                                             // 000000000440: 
// 0x7E40031A,             // v_mov_b32_e32 v32, v26                                           // 000000000444: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000448: 
// 0x7E42031E,             // v_mov_b32_e32 v33, v30                                           // 00000000044C: 
// 0xBE8300FF, 0x00020000, // s_mov_b32 s3, 0x20000                                            // 000000000450: 
// 0xBE8200FF, 0x00050000, // s_mov_b32 s2, 0x50000                                            // 000000000458: 
// 0xD3CD8004, 0x04124116, // v_mfma_f32_16x16x16f16 a[4:7], v[22:23], v[32:33], a[4:7]        // 000000000460: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000468: 
// 0xBF8A0000,             // s_barrier                                                        // 00000000046C: 
// 0xBF8C0F74,             // s_waitcnt vmcnt(4)                                               // 000000000470: 
// 0xD81A0000, 0x0000120D, // ds_write_b32 v13, v18                                            // 000000000474: 
// 0xBF8C0F72,             // s_waitcnt vmcnt(2)                                               // 00000000047C: 
// 0xD2A00018, 0x00021106, // v_pack_b32_f16 v24, v6, v8                                       // 000000000480: 
// 0xD2A01819, 0x00021106, // v_pack_b32_f16 v25, v6, v8 op_sel:[1,1,0]                        // 000000000488: 
// 0x7E3C031B,             // v_mov_b32_e32 v30, v27                                           // 000000000490: 
// 0xD2A0001A, 0x00021307, // v_pack_b32_f16 v26, v7, v9                                       // 000000000494: 
// 0xD2A0181B, 0x00021307, // v_pack_b32_f16 v27, v7, v9 op_sel:[1,1,0]                        // 00000000049C: 
// 0xBF8C0F70,             // s_waitcnt vmcnt(0)                                               // 0000000004A4: 
// 0xD2A00006, 0x00020902, // v_pack_b32_f16 v6, v2, v4                                        // 0000000004A8: 
// 0xD2A01807, 0x00020902, // v_pack_b32_f16 v7, v2, v4 op_sel:[1,1,0]                         // 0000000004B0: 
// 0xD2A00008, 0x00020B03, // v_pack_b32_f16 v8, v3, v5                                        // 0000000004B8: 
// 0xD2A01809, 0x00020B03, // v_pack_b32_f16 v9, v3, v5 op_sel:[1,1,0]                         // 0000000004C0: 
// 0xD9BE0000, 0x0000180E, // ds_write_b128 v14, v[24:27]                                      // 0000000004C8: 
// 0xD9BE0000, 0x0000060F, // ds_write_b128 v15, v[6:9]                                        // 0000000004D0: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000004D8: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000004DC: 
// 0xD9FE0000, 0x02000011, // ds_read_b128 v[2:5], v17                                         // 0000000004E0: 
// 0xD8700100, 0x06000010, // ds_read2st64_b32 v[6:7], v16 offset1:1                           // 0000000004E8: 
// 0xD86EC484, 0x08000010, // ds_read2_b32 v[8:9], v16 offset0:132 offset1:196                 // 0000000004F0: 
// 0xD8700504, 0x10000013, // ds_read2st64_b32 v[16:17], v19 offset0:4 offset1:5               // 0000000004F8: 
// 0xD8700706, 0x12000022, // ds_read2st64_b32 v[18:19], v34 offset0:6 offset1:7               // 000000000500: 
// 0xD3CD8000, 0x04023914, // v_mfma_f32_16x16x16f16 a[0:3], v[20:21], v[28:29], a[0:3]        // 000000000508: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000510: 
// 0xBF8A0000,             // s_barrier                                                        // 000000000514: 
// 0xBF8CC37F,             // s_waitcnt lgkmcnt(3)                                             // 000000000518: 
// 0x7E1C0306,             // v_mov_b32_e32 v14, v6                                            // 00000000051C: 
// 0xBF8CC27F,             // s_waitcnt lgkmcnt(2)                                             // 000000000520: 
// 0x7E1E0308,             // v_mov_b32_e32 v15, v8                                            // 000000000524: 
// 0x7E100307,             // v_mov_b32_e32 v8, v7                                             // 000000000528: 
// 0x260C188C,             // v_and_b32_e32 v6, 12, v12                                        // 00000000052C: 
// 0xD2000000, 0x04011106, // v_lshl_or_b32 v0, v6, 8, v0                                      // 000000000530: 
// 0x24181682,             // v_lshlrev_b32_e32 v12, 2, v11                                    // 000000000538: 
// 0xD3CD8004, 0x04121D02, // v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[14:15], a[4:7]          // 00000000053C: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 000000000544: 
// 0x7E1E0312,             // v_mov_b32_e32 v15, v18                                           // 000000000548: 
// 0x7E1C0310,             // v_mov_b32_e32 v14, v16                                           // 00000000054C: 
// 0x7E240311,             // v_mov_b32_e32 v18, v17                                           // 000000000550: 
// 0xD3CD8000, 0x04023D16, // v_mfma_f32_16x16x16f16 a[0:3], v[22:23], v[30:31], a[0:3]        // 000000000554: 
// 0xD3CD8004, 0x04121D04, // v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[14:15], a[4:7]          // 00000000055C: 
// 0xD3CD8000, 0x04021102, // v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[8:9], a[0:3]            // 000000000564: 
// 0x24041684,             // v_lshlrev_b32_e32 v2, 4, v11                                     // 00000000056C: 
// 0xD2000002, 0x0409110A, // v_lshl_or_b32 v2, v10, 8, v2                                     // 000000000570: 
// 0xBF800007,             // s_nop 7                                                          // 000000000578: 
// 0xDA1A0000, 0x00000400, // ds_write_b32 v0, a4                                              // 00000000057C: 
// 0xDA1A0100, 0x00000500, // ds_write_b32 v0, a5 offset:256                                   // 000000000584: 
// 0xDA1A0200, 0x00000600, // ds_write_b32 v0, a6 offset:512                                   // 00000000058C: 
// 0xDA1A0300, 0x00000700, // ds_write_b32 v0, a7 offset:768                                   // 000000000594: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 00000000059C: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000005A0: 
// 0xD9FE0000, 0x06000002, // ds_read_b128 v[6:9], v2                                          // 0000000005A4: 
// 0xD3CD8000, 0x04022504, // v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[18:19], a[0:3]          // 0000000005AC: 
// 0xD1FE0004, 0x02061901, // v_add_lshl_u32 v4, v1, v12, 1                                    // 0000000005B4: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000005BC: 
// 0x7E061506,             // v_cvt_f16_f32_e32 v3, v6                                         // 0000000005C0: 
// 0x7E0C1507,             // v_cvt_f16_f32_e32 v6, v7                                         // 0000000005C4: 
// 0x7E0E1508,             // v_cvt_f16_f32_e32 v7, v8                                         // 0000000005C8: 
// 0x7E101509,             // v_cvt_f16_f32_e32 v8, v9                                         // 0000000005CC: 
// 0xD2A00001, 0x00020D03, // v_pack_b32_f16 v1, v3, v6                                        // 0000000005D0: 
// 0xE1381000, 0x80000104, // buffer_atomic_pk_add_f16 v1, v4, s[0:3], 0 offen                 // 0000000005D8: 
// 0xD2A00001, 0x00021107, // v_pack_b32_f16 v1, v7, v8                                        // 0000000005E0: 
// 0xE1381000, 0x84000104, // buffer_atomic_pk_add_f16 v1, v4, s[0:3], 4 offen                 // 0000000005E8: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 0000000005F0: 
// 0xBF8A0000,             // s_barrier                                                        // 0000000005F4: 
// 0xBF800000,             // s_nop 0                                                          // 0000000005F8: 
// 0xDA1A0000, 0x00000000, // ds_write_b32 v0, a0                                              // 0000000005FC: 
// 0xDA1A0100, 0x00000100, // ds_write_b32 v0, a1 offset:256                                   // 000000000604: 
// 0xDA1A0200, 0x00000200, // ds_write_b32 v0, a2 offset:512                                   // 00000000060C: 
// 0xDA1A0300, 0x00000300, // ds_write_b32 v0, a3 offset:768                                   // 000000000614: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 00000000061C: 
// 0xBF8A0000,             // s_barrier                                                        // 000000000620: 
// 0xD9FE0000, 0x00000002, // ds_read_b128 v[0:3], v2                                          // 000000000624: 
// 0xBF8CC07F,             // s_waitcnt lgkmcnt(0)                                             // 00000000062C: 
// 0x7E001500,             // v_cvt_f16_f32_e32 v0, v0                                         // 000000000630: 
// 0x7E021501,             // v_cvt_f16_f32_e32 v1, v1                                         // 000000000634: 
// 0x7E041502,             // v_cvt_f16_f32_e32 v2, v2                                         // 000000000638: 
// 0x7E061503,             // v_cvt_f16_f32_e32 v3, v3                                         // 00000000063C: 
// 0xD2A00000, 0x00020300, // v_pack_b32_f16 v0, v0, v1                                        // 000000000640: 
// 0xE1381080, 0x80000004, // buffer_atomic_pk_add_f16 v0, v4, s[0:3], 0 offen offset:128      // 000000000648: 
// 0xD2A00000, 0x00020702, // v_pack_b32_f16 v0, v2, v3                                        // 000000000650: 
// 0xE1381080, 0x84000004, // buffer_atomic_pk_add_f16 v0, v4, s[0:3], 4 offen offset:128      // 000000000658: 
// 0xBF810000              // s_endpgm                                                         // 000000000660: 
// #endif
// };

void IsaGenerator_Aldbrn::GetGEMMIsa_16_1152_5120(HsaMemoryBuffer& rBuf) {
    std::copy(GEMM_ISA_16_1152_5120, GEMM_ISA_16_1152_5120+ARRAY_SIZE(GEMM_ISA_16_1152_5120), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetGEMMIsa_16_5120_384(HsaMemoryBuffer& rBuf) {
    std::copy(GEMM_ISA_16_5120_384, GEMM_ISA_16_5120_384+ARRAY_SIZE(GEMM_ISA_16_5120_384), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetGEMMIsa_16_1280_5120(HsaMemoryBuffer& rBuf) {
    std::copy(GEMM_ISA_16_1280_5120, GEMM_ISA_16_1280_5120+ARRAY_SIZE(GEMM_ISA_16_1280_5120), rBuf.As<uint32_t*>());
}

void IsaGenerator_Aldbrn::GetGEMMIsa_16_5120_1280(HsaMemoryBuffer& rBuf) {
    std::copy(GEMM_ISA_16_5120_1280, GEMM_ISA_16_5120_1280+ARRAY_SIZE(GEMM_ISA_16_5120_1280), rBuf.As<uint32_t*>());
}

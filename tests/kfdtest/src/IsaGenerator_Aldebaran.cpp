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

const std::string& IsaGenerator_Aldbrn::GetAsicName() {
    return ASIC_NAME;
}

const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_1152_5120[] = {
    0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_5120_384[] = {
 // Original ISA:
 // s4-s5 : kernarg pointer
 // s6 : workgroup_id_x
 // s7 : workgroup_id_y
 // s8 : workgroup_id_z
 // 0xC00A0402, 0x00000000, //	s_load_dwordx4 s[16:19], s[4:5], 0x0                       
 // 0xC00A0002, 0x00000010, //	s_load_dwordx4 s[0:3], s[4:5], 0x10                        
 // 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       

 // Hacked ISA
 // s0-s1: matrix C pointer
 // s2-s3: matrix A pointer
 // s4-s5: matrix B pointer
 // s6 : workgroup_id_x
 // s7 : workgroup_id_y
 // s8 : workgroup_id_z
 0xBE900002,             //     s_mov_b32 s16, s2
 0xBE910003,             //     s_mov_b32 s17, s3
 0xBE920004,		 //     s_mov_b32 s18, s4
 0xBE930005,		 //     s_mov_b32_s19, s5

 // The actual GEMM algorithm from  Microsoft based on CK.
 0x8E028407,             //	s_lshl_b32 s2, s7, 4                                       
 0x20300084,             //	v_lshrrev_b32_e32 v24, 4, v0                               
 0x68343002,             //	v_add_u32_e32 v26, s2, v24                                 
 0xB0020180,             //	s_movk_i32 s2, 0x180                                       
 0xD2850004, 0x0000051A, //	v_mul_lo_u32 v4, v26, s2                                   
 0x8E028508,             //	s_lshl_b32 s2, s8, 5                                       
 0x2604308E,             //	v_and_b32_e32 v2, 14, v24                                  
 0x24060082,             //	v_lshlrev_b32_e32 v3, 2, v0                                
 0x280A0402,             //	v_or_b32_e32 v5, s2, v2                                    
 0xB00D1400,             //	s_movk_i32 s13, 0x1400                                     
 0x260606FF, 0x0000007C, //	v_and_b32_e32 v3, 0x7c, v3                                 
 0xD2850005, 0x00001B05, //	v_mul_lo_u32 v5, v5, s13                                   
 0x8E0C8706,             //	s_lshl_b32 s12, s6, 7                                      
 0x2632008F,             //	v_and_b32_e32 v25, 15, v0                                  
 0x280A0705,             //	v_or_b32_e32 v5, v5, v3                                    
 0xD1FE0015, 0x02041905, //	v_add_lshl_u32 v21, v5, s12, 1                             
 0x68080802,             //	v_add_u32_e32 v4, s2, v4                                   
 0x24023281,             //	v_lshlrev_b32_e32 v1, 1, v25                               
 0xBE8700FF, 0x00020000, //	s_mov_b32 s7, 0x20000                                      
 0x28080304,             //	v_or_b32_e32 v4, v4, v1                                    
 0xBE8A00FF, 0x00780000, //	s_mov_b32 s10, 0x780000                                    
 0xBE880012,             //	s_mov_b32 s8, s18                                          
 0xBE890013,             //	s_mov_b32 s9, s19                                          
 0xBE8B0007,             //	s_mov_b32 s11, s7                                          
 0x68182AFF, 0x00002000, //	v_add_u32_e32 v12, 0x2000, v21                             
 0x24280881,             //	v_lshlrev_b32_e32 v20, 1, v4                               
 0x681A2AFF, 0x00028000, //	v_add_u32_e32 v13, 0x28000, v21                            
 0x681C2AFF, 0x0002A000, //	v_add_u32_e32 v14, 0x2a000, v21                            
 0xE0541000, 0x80020415, //	buffer_load_dwordx2 v[4:5], v21, s[8:11], 0 offen          
 0xE0541800, 0x8002060C, //	buffer_load_dwordx2 v[6:7], v12, s[8:11], 0 offen offset:2048
 0xE0541000, 0x8002080D, //	buffer_load_dwordx2 v[8:9], v13, s[8:11], 0 offen          
 0xE0541800, 0x80020A0E, //	buffer_load_dwordx2 v[10:11], v14, s[8:11], 0 offen offset:2048
 0x24180081,             //	v_lshlrev_b32_e32 v12, 1, v0                               
 0xD2010001, 0x0405410C, //	v_and_or_b32 v1, v12, 32, v1                               
 0x201A0085,             //	v_lshrrev_b32_e32 v13, 5, v0                               
 0xB0030050,             //	s_movk_i32 s3, 0x50                                        
 0x24240681,             //	v_lshlrev_b32_e32 v18, 1, v3                               
 0xD1C80003, 0x020D0300, //	v_bfe_u32 v3, v0, 1, 3                                     
 0x10181A03,             //	v_mul_u32_u24_e32 v12, s3, v13                             
 0x24020281,             //	v_lshlrev_b32_e32 v1, 1, v1                                
 0xB0020108,             //	s_movk_i32 s2, 0x108                                       
 0xD1FD001B, 0x0405030C, //	v_lshl_add_u32 v27, v12, 1, v1                             
 0x10021A02,             //	v_mul_u32_u24_e32 v1, s2, v13                              
 0x10260603,             //	v_mul_u32_u24_e32 v19, s3, v3                              
 0x24060085,             //	v_lshlrev_b32_e32 v3, 5, v0                                
 0xD1FE001C, 0x02060312, //	v_add_lshl_u32 v28, v18, v1, 1                             
 0x28020490,             //	v_or_b32_e32 v1, 16, v2                                    
 0xD1C80002, 0x02090900, //	v_bfe_u32 v2, v0, 4, 2                                     
 0x260606A0,             //	v_and_b32_e32 v3, 32, v3                                   
 0xB0066000,             //	s_movk_i32 s6, 0x6000                                      
 0xBE840010,             //	s_mov_b32 s4, s16                                          
 0xBE850011,             //	s_mov_b32 s5, s17                                          
 0xD2000016, 0x040D0702, //	v_lshl_or_b32 v22, v2, 3, v3                               
 0x103C04FF, 0x00000420, //	v_mul_u32_u24_e32 v30, 0x420, v2                           
 0xE0501000, 0x80010214, //	buffer_load_dword v2, v20, s[4:7], 0 offen                 
 0xE0501100, 0x80011F14, //	buffer_load_dword v31, v20, s[4:7], 0 offen offset:256     
 0x20060082,             //	v_lshrrev_b32_e32 v3, 2, v0                                
 0x68402AFF, 0x00140000, //	v_add_u32_e32 v32, 0x140000, v21                           
 0xD201001D, 0x04656103, //	v_and_or_b32 v29, v3, 48, v25                              
 0x68422AFF, 0x00142000, //	v_add_u32_e32 v33, 0x142000, v21                           
 0x68442AFF, 0x00168000, //	v_add_u32_e32 v34, 0x168000, v21                           
 0x68462AFF, 0x0016A000, //	v_add_u32_e32 v35, 0x16a000, v21                           
 0x20020281,             //	v_lshrrev_b32_e32 v1, 1, v1                                
 0x242E3A81,             //	v_lshlrev_b32_e32 v23, 1, v29                              
 0xD1FE001E, 0x02063D17, //	v_add_lshl_u32 v30, v23, v30, 1                            
 0x10020202,             //	v_mul_u32_u24_e32 v1, s2, v1                               
 0x68483CB0,             //	v_add_u32_e32 v36, 48, v30                                 
 0x684A2AFF, 0x00282000, //	v_add_u32_e32 v37, 0x282000, v21                           
 0x684C2AFF, 0x002A8000, //	v_add_u32_e32 v38, 0x2a8000, v21                           
 0x684E2AFF, 0x002AA000, //	v_add_u32_e32 v39, 0x2aa000, v21                           
 0xBF8C0F71,             //	s_waitcnt vmcnt(1)                                         
 0xD81A0000, 0x0000021B, //	ds_write_b32 v27, v2                                       
 0xD2A00002, 0x00020D04, //	v_pack_b32_f16 v2, v4, v6                                  
 0xD2A01803, 0x00020D04, //	v_pack_b32_f16 v3, v4, v6 op_sel:[1,1,0]                   
 0xD2A00004, 0x00020F05, //	v_pack_b32_f16 v4, v5, v7                                  
 0xD2A01805, 0x00020F05, //	v_pack_b32_f16 v5, v5, v7 op_sel:[1,1,0]                   
 0xD2A00006, 0x00021508, //	v_pack_b32_f16 v6, v8, v10                                 
 0xD2A01807, 0x00021508, //	v_pack_b32_f16 v7, v8, v10 op_sel:[1,1,0]                  
 0xD2A00008, 0x00021709, //	v_pack_b32_f16 v8, v9, v11                                 
 0xD2A01809, 0x00021709, //	v_pack_b32_f16 v9, v9, v11 op_sel:[1,1,0]                  
 0xE0541000, 0x80020A20, //	buffer_load_dwordx2 v[10:11], v32, s[8:11], 0 offen        
 0xE0541800, 0x80020C21, //	buffer_load_dwordx2 v[12:13], v33, s[8:11], 0 offen offset:2048
 0xE0541000, 0x80020E22, //	buffer_load_dwordx2 v[14:15], v34, s[8:11], 0 offen        
 0xE0541800, 0x80021023, //	buffer_load_dwordx2 v[16:17], v35, s[8:11], 0 offen offset:2048
 0xD1FE0020, 0x02062716, //	v_add_lshl_u32 v32, v22, v19, 1                            
 0xD1FE0022, 0x02062501, //	v_add_lshl_u32 v34, v1, v18, 1                             
 0xD9BE0500, 0x0000021C, //	ds_write_b128 v28, v[2:5] offset:1280                      
 0xD9BE0500, 0x00000622, //	ds_write_b128 v34, v[6:9] offset:1280                      
 0x68423C90,             //	v_add_u32_e32 v33, 16, v30                                 
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xD9FE0000, 0x02000020, //	ds_read_b128 v[2:5], v32                                   
 0xD8700605, 0x0600001E, //	ds_read2st64_b32 v[6:7], v30 offset0:5 offset1:6           
 0xD8700807, 0x08000021, //	ds_read2st64_b32 v[8:9], v33 offset0:7 offset1:8           
 0x68463CA0,             //	v_add_u32_e32 v35, 32, v30                                 
 0x68022AFF, 0x00280000, //	v_add_u32_e32 v1, 0x280000, v21                            
 0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
 0x7E240306,             //	v_mov_b32_e32 v18, v6                                      
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0x7E260308,             //	v_mov_b32_e32 v19, v8                                      
 0x7E100307,             //	v_mov_b32_e32 v8, v7                                       
 0xBF800000,             //	s_nop 0                                                    
 0xD3CD8000, 0x02022502, //	v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[18:19], 0         
 0xBE8200FF, 0x00050000, //	s_mov_b32 s2, 0x50000                                      
 0xBE830007,             //	s_mov_b32 s3, s7                                           
 0xD3CD8004, 0x02021102, //	v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[8:9], 0           
 0xD8700A09, 0x02000023, //	ds_read2st64_b32 v[2:3], v35 offset0:9 offset1:10          
 0xD8700C0B, 0x12000024, //	ds_read2st64_b32 v[18:19], v36 offset0:11 offset1:12       
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xBF8C0F74,             //	s_waitcnt vmcnt(4)                                         
 0xD81A0000, 0x00001F1B, //	ds_write_b32 v27, v31                                      
 0xE0501200, 0x80011F14, //	buffer_load_dword v31, v20, s[4:7], 0 offen offset:512     
 0xBF8CC27F,             //	s_waitcnt lgkmcnt(2)                                       
 0x7E0C0302,             //	v_mov_b32_e32 v6, v2                                       
 0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
 0x7E0E0312,             //	v_mov_b32_e32 v7, v18                                      
 0xB0040C00,             //	s_movk_i32 s4, 0xc00                                       
 0x7E240303,             //	v_mov_b32_e32 v18, v3                                      
 0xD3CD8000, 0x04020D04, //	v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[6:7], a[0:3]      
 0xBF800000,             //	s_nop 0                                                    
 0xD3CD8004, 0x04122504, //	v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[18:19], a[4:7]    
 0xBF8C0F73,             //	s_waitcnt vmcnt(3)                                         
 0xD2A00006, 0x0002190A, //	v_pack_b32_f16 v6, v10, v12                                
 0xD2A01807, 0x0002190A, //	v_pack_b32_f16 v7, v10, v12 op_sel:[1,1,0]                 
 0xD2A00008, 0x00021B0B, //	v_pack_b32_f16 v8, v11, v13                                
 0xD2A01809, 0x00021B0B, //	v_pack_b32_f16 v9, v11, v13 op_sel:[1,1,0]                 
 0xBF8C0F71,             //	s_waitcnt vmcnt(1)                                         
 0xD2A0000A, 0x0002210E, //	v_pack_b32_f16 v10, v14, v16                               
 0xD2A0180B, 0x0002210E, //	v_pack_b32_f16 v11, v14, v16 op_sel:[1,1,0]                
 0xD2A0000C, 0x0002230F, //	v_pack_b32_f16 v12, v15, v17                               
 0xD2A0180D, 0x0002230F, //	v_pack_b32_f16 v13, v15, v17 op_sel:[1,1,0]                
 0xE0541000, 0x80020E01, //	buffer_load_dwordx2 v[14:15], v1, s[8:11], 0 offen         
 0xE0541800, 0x80021025, //	buffer_load_dwordx2 v[16:17], v37, s[8:11], 0 offen offset:2048
 0xE0541000, 0x80021426, //	buffer_load_dwordx2 v[20:21], v38, s[8:11], 0 offen        
 0xE0541800, 0x80021627, //	buffer_load_dwordx2 v[22:23], v39, s[8:11], 0 offen offset:2048
 0xD9BE0500, 0x0000061C, //	ds_write_b128 v28, v[6:9] offset:1280                      
 0xD9BE0500, 0x00000A22, //	ds_write_b128 v34, v[10:13] offset:1280                    
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xD9FE0000, 0x02000020, //	ds_read_b128 v[2:5], v32                                   
 0xD8700605, 0x0600001E, //	ds_read2st64_b32 v[6:7], v30 offset0:5 offset1:6           
 0xD8700807, 0x08000021, //	ds_read2st64_b32 v[8:9], v33 offset0:7 offset1:8           
 0x24183284,             //	v_lshlrev_b32_e32 v12, 4, v25                              
 0x241A3A82,             //	v_lshlrev_b32_e32 v13, 2, v29                              
 0xD200000C, 0x04311118, //	v_lshl_or_b32 v12, v24, 8, v12                             
 0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
 0x7E140306,             //	v_mov_b32_e32 v10, v6                                      
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0x7E160308,             //	v_mov_b32_e32 v11, v8                                      
 0x7E100307,             //	v_mov_b32_e32 v8, v7                                       
 0xBF800000,             //	s_nop 0                                                    
 0xD3CD8000, 0x04021502, //	v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[10:11], a[0:3]    
 0x24140086,             //	v_lshlrev_b32_e32 v10, 6, v0                               
 0x24163282,             //	v_lshlrev_b32_e32 v11, 2, v25                              
 0xD201000A, 0x0434090A, //	v_and_or_b32 v10, v10, s4, v13                             
 0xD3CD8004, 0x04121102, //	v_mfma_f32_16x16x16f16 a[4:7], v[2:3], v[8:9], a[4:7]      
 0xD8700A09, 0x00000023, //	ds_read2st64_b32 v[0:1], v35 offset0:9 offset1:10          
 0xD8700C0B, 0x02000024, //	ds_read2st64_b32 v[2:3], v36 offset0:11 offset1:12         
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xBF8C0F74,             //	s_waitcnt vmcnt(4)                                         
 0xD81A0000, 0x00001F1B, //	ds_write_b32 v27, v31                                      
 0xBF8CC27F,             //	s_waitcnt lgkmcnt(2)                                       
 0x7E0C0300,             //	v_mov_b32_e32 v6, v0                                       
 0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
 0x7E0E0302,             //	v_mov_b32_e32 v7, v2                                       
 0x7E040301,             //	v_mov_b32_e32 v2, v1                                       
 0xBF800000,             //	s_nop 0                                                    
 0xD3CD8000, 0x04020D04, //	v_mfma_f32_16x16x16f16 a[0:3], v[4:5], v[6:7], a[0:3]      
 0xD3CD8004, 0x04120504, //	v_mfma_f32_16x16x16f16 a[4:7], v[4:5], v[2:3], a[4:7]      
 0xBF8C0F72,             //	s_waitcnt vmcnt(2)                                         
 0xD2A00000, 0x0002210E, //	v_pack_b32_f16 v0, v14, v16                                
 0xD2A01801, 0x0002210E, //	v_pack_b32_f16 v1, v14, v16 op_sel:[1,1,0]                 
 0xD2A00002, 0x0002230F, //	v_pack_b32_f16 v2, v15, v17                                
 0xD2A01803, 0x0002230F, //	v_pack_b32_f16 v3, v15, v17 op_sel:[1,1,0]                 
 0xBF8C0F70,             //	s_waitcnt vmcnt(0)                                         
 0xD2A00004, 0x00022D14, //	v_pack_b32_f16 v4, v20, v22                                
 0xD2A01805, 0x00022D14, //	v_pack_b32_f16 v5, v20, v22 op_sel:[1,1,0]                 
 0xD2A00006, 0x00022F15, //	v_pack_b32_f16 v6, v21, v23                                
 0xD2A01807, 0x00022F15, //	v_pack_b32_f16 v7, v21, v23 op_sel:[1,1,0]                 
 0xD9BE0500, 0x0000001C, //	ds_write_b128 v28, v[0:3] offset:1280                      
 0xD9BE0500, 0x00000422, //	ds_write_b128 v34, v[4:7] offset:1280                      
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xD9FE0000, 0x00000020, //	ds_read_b128 v[0:3], v32                                   
 0xD8700605, 0x0400001E, //	ds_read2st64_b32 v[4:5], v30 offset0:5 offset1:6           
 0xD8700807, 0x06000021, //	ds_read2st64_b32 v[6:7], v33 offset0:7 offset1:8           
 0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
 0x7E100304,             //	v_mov_b32_e32 v8, v4                                       
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0x7E120306,             //	v_mov_b32_e32 v9, v6                                       
 0x7E0C0305,             //	v_mov_b32_e32 v6, v5                                       
 0xD2850004, 0x00001B1A, //	v_mul_lo_u32 v4, v26, s13                                  
 0xD3CD8000, 0x04021100, //	v_mfma_f32_16x16x16f16 a[0:3], v[0:1], v[8:9], a[0:3]      
 0xD8700A09, 0x08000023, //	ds_read2st64_b32 v[8:9], v35 offset0:9 offset1:10          
 0x6808080C,             //	v_add_u32_e32 v4, s12, v4                                  
 0xD1FE000B, 0x02061704, //	v_add_lshl_u32 v11, v4, v11, 1                             
 0xD3CD8004, 0x04120D00, //	v_mfma_f32_16x16x16f16 a[4:7], v[0:1], v[6:7], a[4:7]      
 0xD8700C0B, 0x00000024, //	ds_read2st64_b32 v[0:1], v36 offset0:11 offset1:12         
 0xBF8CC17F,             //	s_waitcnt lgkmcnt(1)                                       
 0x7E080308,             //	v_mov_b32_e32 v4, v8                                       
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0x7E0A0300,             //	v_mov_b32_e32 v5, v0                                       
 0x7E000309,             //	v_mov_b32_e32 v0, v9                                       
 0xBF800000,             //	s_nop 0                                                    
 0xD3CD8000, 0x04020902, //	v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[4:5], a[0:3]      
 0xBF800007,             //	s_nop 7                                                    
 0xBF800002,             //	s_nop 2                                                    
 0xDA1A0000, 0x0000000A, //	ds_write_b32 v10, a0                                       
 0xDA1A0100, 0x0000010A, //	ds_write_b32 v10, a1 offset:256                            
 0xDA1A0200, 0x0000020A, //	ds_write_b32 v10, a2 offset:512                            
 0xDA1A0300, 0x0000030A, //	ds_write_b32 v10, a3 offset:768                            
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xD9FE0000, 0x0400000C, //	ds_read_b128 v[4:7], v12                                   
 0xD3CD8000, 0x04120102, //	v_mfma_f32_16x16x16f16 a[0:3], v[2:3], v[0:1], a[4:7]      
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0x7E081504,             //	v_cvt_f16_f32_e32 v4, v4                                   
 0x7E0A1505,             //	v_cvt_f16_f32_e32 v5, v5                                   
 0x7E0C1506,             //	v_cvt_f16_f32_e32 v6, v6                                   
 0x7E0E1507,             //	v_cvt_f16_f32_e32 v7, v7                                   
 0xD2A00000, 0x00020B04, //	v_pack_b32_f16 v0, v4, v5                                  
 0xD2A00001, 0x00020F06, //	v_pack_b32_f16 v1, v6, v7                                  
 0xE1381000, 0x8000000B, //	buffer_atomic_pk_add_f16 v0, v11, s[0:3], 0 offen          
 0xE1381000, 0x8400010B, //	buffer_atomic_pk_add_f16 v1, v11, s[0:3], 4 offen          
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xBF800001,             //	s_nop 1                                                    
 0xDA1A0000, 0x0000000A, //	ds_write_b32 v10, a0                                       
 0xDA1A0100, 0x0000010A, //	ds_write_b32 v10, a1 offset:256                            
 0xDA1A0200, 0x0000020A, //	ds_write_b32 v10, a2 offset:512                            
 0xDA1A0300, 0x0000030A, //	ds_write_b32 v10, a3 offset:768                            
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0xBF8A0000,             //	s_barrier                                                  
 0xD9FE0000, 0x0000000C, //	ds_read_b128 v[0:3], v12                                   
 0xBF8CC07F,             //	s_waitcnt lgkmcnt(0)                                       
 0x7E001500,             //	v_cvt_f16_f32_e32 v0, v0                                   
 0x7E021501,             //	v_cvt_f16_f32_e32 v1, v1                                   
 0x7E041502,             //	v_cvt_f16_f32_e32 v2, v2                                   
 0x7E061503,             //	v_cvt_f16_f32_e32 v3, v3                                   
 0xD2A00000, 0x00020300, //	v_pack_b32_f16 v0, v0, v1                                  
 0xE1381080, 0x8000000B, //	buffer_atomic_pk_add_f16 v0, v11, s[0:3], 0 offen offset:128
 0xD2A00000, 0x00020702, //	v_pack_b32_f16 v0, v2, v3                                  
 0xE1381080, 0x8400000B, //	buffer_atomic_pk_add_f16 v0, v11, s[0:3], 4 offen offset:128
 0xBF810000              //	s_endpgm                                                   
};

const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_1280_5120[] = {
    0xbf810000                // s_endpgm
};

const uint32_t IsaGenerator_Aldbrn::GEMM_ISA_16_5120_1280[] = {
    0xbf810000                // s_endpgm
};

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

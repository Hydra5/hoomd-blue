/*
Highly Optimized Object-Oriented Molecular Dynamics (HOOMD) Open
Source Software License
Copyright (c) 2008 Ames Laboratory Iowa State University
All rights reserved.

Redistribution and use of HOOMD, in source and binary forms, with or
without modification, are permitted, provided that the following
conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names HOOMD's
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

Disclaimer

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND
CONTRIBUTORS ``AS IS''  AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 

IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS  BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

// $Id$
// $URL$

/*! \file NVTUpdaterGPU.cuh
	\brief Declares GPU kernel code for NVT integration on the GPU. Used by NVTUpdaterGPU.
*/

#include "ParticleData.cuh"

#ifndef __NVTUPDATER_CUH__
#define __NVTUPDATER_CUH__

//! Stores intermediate values for NVT integration
/*! NVT integration (NVTUpdaterGPU) requires summing up the kinetic energy of the system.
	gpu_nvt_data stores the needed auxiliary data structure needed to do the standard reduction
	sum.
	
	\ingroup gpu_data_structs
*/
struct gpu_nvt_data
	{
	float *partial_Ksum; //!< NBlocks elements, each is a partial sum of m*v^2
	float *Ksum;	//!< fully reduced Ksum on one GPU
	int NBlocks;	//!< Number of blocks in the computation
	int block_size;	//!< Block size of the kernel to be run on the device (must be a power of 2)
	};

//! Kernel driver for the first part of the NVT update called by NVTUpdaterGPU
cudaError_t gpu_nvt_pre_step(const gpu_pdata_arrays &pdata, const gpu_boxsize &box, const gpu_nvt_data &d_nvt_data, float Xi, float deltaT);

//! Kernel driver for the Ksum reduction final pass called by NVTUpdaterGPU
cudaError_t gpu_nvt_reduce_ksum(const gpu_nvt_data &d_nvt_data);

//! Kernel driver for the second part of the NVT update called by NVTUpdaterGPU
cudaError_t gpu_nvt_step(const gpu_pdata_arrays &pdata, const gpu_nvt_data &d_nvt_data, float4 **force_data_ptrs, int num_forces, float Xi, float deltaT);

#endif

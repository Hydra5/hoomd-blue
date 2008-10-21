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

// $Id: YukawaForceCompute.h 1325 2008-10-06 13:42:07Z joaander $
// $URL: http://svn2.assembla.com/svn/hoomd/trunk/src/computes/YukawaForceCompute.h $

#include <boost/shared_ptr.hpp>

#include "ForceCompute.h"
#include "NeighborList.h"

/*! \file YukawaForceCompute.h
	\brief Declares a class for computing lennard jones forces
*/

#ifndef __YUKAWAFORCECOMPUTE_H__
#define __YUKAWAFORCECOMPUTE_H__

//! Computes Lennard-Jones forces on each particle
/*! The total pair force is summed for each particle when compute() is called. Forces are only summed between
	neighboring particles with a separation distance less than \c r_cut. A NeighborList must be provided
	to identify these neighbors. Calling compute() in this class will in turn result in a call to the 
	NeighborList's compute() to make sure that the neighbor list is up to date.
	
	Usage: Construct a YukawaForceCompute, providing it an already constructed ParticleData and NeighborList.
	Then set parameters for all possible pairs of types by calling setParams. 
	
	Forces can be computed directly by calling compute() and then retrieved with a call to acquire(), but 
	a more typical usage will be to add the force compute to NVEUpdator or NVTUpdator. 
	
	This base class defines the interface for performing Lennard-Jones force computations. It does provide
	a functional, single threaded method for computing the forces. A faster multi-threaded version
	is YukawaForceComputeThreaded which will perform its computations in parallel across all processor cores in a
	system. If compiled with CUDA support, there is YukawaForceComputeGPU which is roughly 60x faster than 
	this class at computing forces.
	\ingroup computes
*/
class YukawaForceCompute : public ForceCompute
	{
	public:
		//! Constructs the compute
		YukawaForceCompute(boost::shared_ptr<ParticleData> pdata, boost::shared_ptr<NeighborList> nlist, Scalar r_cut, Scalar kappa);
		
		//! Destructor
		virtual ~YukawaForceCompute();
		
		//! Set the parameters for a single type pair
		virtual void setParams(unsigned int typ1, unsigned int typ2, Scalar epsilon);
		
		// Would it be worthwhile to be able to change kappa?
		
		//! Returns a list of log quantities this compute calculates
		virtual std::vector< std::string > getProvidedLogQuantities(); 
		
		//! Calculates the requested log value and returns it
		virtual Scalar getLogValue(const std::string& quantity);
		
	protected:
		boost::shared_ptr<NeighborList> m_nlist;	//!< The neighborlist to use for the computation
		Scalar m_r_cut;	//!< Cuttoff radius beyond which the force is set to 0
		Scalar m_kappa; //!< Kappa
		unsigned int m_ntypes;	//!< Store the width and height of lj1 and lj2 here
		
		// This is a low level force summing class, it ONLY sums forces, and doesn't do high
		// level concepts like mixing. That is for the caller to handle. So, I only store 
		// lj1 and lj2 here
		Scalar * __restrict__ m_epsilon;	//!< Parameter for computing forces (m_ntypes by m_ntypes array)
		
		//! Actually compute the forces
		virtual void computeForces(unsigned int timestep);
	};
	
//! Exports the YukawaForceCompute class to python
void export_YukawaForceCompute();

#endif

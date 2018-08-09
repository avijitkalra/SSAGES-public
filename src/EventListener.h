/**
 * This file is part of
 * SSAGES - Suite for Advanced Generalized Ensemble Simulations
 *
 * Copyright 2016 Hythem Sidky <hsidky@nd.edu>
 *
 * SSAGES is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SSAGES is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SSAGES.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once 

#include <mxx/comm.hpp>
#include "types.h"

namespace SSAGES
{
	// Forward declare. 
	class Snapshot;

	//! Base abstract class for listening in to events fired by "Hook".
	/*!
	 * \ingroup Core
	 */
	class EventListener
	{
	private:
		uint frequency_; //!< Frequency for listening.
	
	public:
		//! Constructor
		/*!
		 * \param frequency Frequency for listening.
		 */
		EventListener(uint frequency) : 
		frequency_(frequency)
		{
		}

		//! Get frequency of event listener.
		/*!
		 * \return Frequency of event listener.
		 */
		uint GetFrequency() const { return frequency_; }

		//! Method call prior to simulation initiation.
		/*!
		 * \param snapshot Pointer to the simulation snapshot.
		 * \param cvmanager Collective variable manager.
		 *
		 * This function will be called before the simulation is started.
		 */
		virtual void PreSimulation(Snapshot* snapshot, const class CVManager& cvmanager) = 0;

		//! Method call post integration.
		/*!
		 * \param snapshot Pointer to the simulation snapshot.
		 * \param cvmanager Collective variable manager.
		 *
		 * This function will be called at the end of each integration step.
		 */
		virtual void PostIntegration(Snapshot* snapshot, const class CVManager& cvmanager) = 0;

		//! Method call post simulation.
		/*!
		 * \param snapshot Pointer to the simulation snapshot.
		 * \param cvmanager Collective variable manager.
		 *
		 * This function will be called after the simulation has finished.
		 */
		virtual void PostSimulation(Snapshot* snapshot, const class CVManager& cvmanager) = 0;

		//! Return walker ID number.
		static unsigned int GetWalkerID(const MPI_Comm& world, const MPI_Comm& comm)
		{
			return mxx::comm(world).rank()/mxx::comm(comm).size();
		}

		//! Return total number of walkers.
		static unsigned int GetNumWalkers(const MPI_Comm& world, const MPI_Comm& comm)
		{
			return mxx::comm(world).size()/mxx::comm(comm).size();
		}

		//! Check if current processor is master.
		static bool IsMasterRank(const MPI_Comm& comm)
		{
			return mxx::comm(comm).rank() == 0;
		}

		//! Destructor
		virtual ~EventListener() {}
	};
}
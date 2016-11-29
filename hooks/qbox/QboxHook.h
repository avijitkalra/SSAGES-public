#pragma once 

#include "Hook.h"
#include <boost/property_tree/ptree.hpp>

namespace SSAGES
{
	class QboxHook : public Hook
	{
	private: 
		//! Property tree used for XML parsing.
		boost::property_tree::ptree pt_;

		//! Vector of unique species. 
		std::vector<std::string> species_; 

		//! Vector of species' masses.
		std::vector<double> speciesmass_;

		//! Previous forces for velocity verlet integration.
		std::vector<Vector3> prevforces_;

		//! Timestep of integration.
		double timestep_;

		//! Get species index from species vector.
		int GetSpeciesIndex(const std::string& species)
		{
			auto it = std::find(species_.begin(), species_.end(), species);
			if(it == species_.end())
				return -1; 

			return it - species_.begin();
		}

		void BuildSpeciesInfo();

	protected:
		// Implementation of the SyncToEngine interface.
		void SyncToEngine() override;

		// Implementation of the SyncToSnapshot interface. 
		void SyncToSnapshot() override;

	public: 
		QboxHook() : species_(), speciesmass_(), prevforces_(0)
		{}

		void XMLToSSAGES(const std::string& xmlfile);
		void SSAGESToCommands(const std::string& commandfile);

		//! Set integration timestep.
		void SetTimestep(double dt)
		{
			timestep_ = dt;
		}
		
		~QboxHook()
		{}
	};
}
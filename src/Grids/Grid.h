#pragma once

#include "../JSON/Serializable.h"
#include <vector>
#include <iostream>
#include <stdexcept>

// Forward declare.
namespace Json {
	class Value;
}

namespace SSAGES
{
	//! Calculate array index from n-Dimensional Grid indices.
	/*!
	 * \param indices Vector of indices.
	 * \param num_points Number of grid points in each dimension.
	 *
	 * \return Index of 1d array storing data.
	 */
	inline int FlattenIndices(std::vector<int> indices, std::vector<int> num_points)
	{

		int loci = 0;
		for(size_t i = 0; i < indices.size(); i++)
		{
			if(indices[i]>=num_points[i])
				throw std::out_of_range("Grid indices out of range");
			
			int locj = indices[i];
			for(size_t j = i+1;j<indices.size();j++)
				locj *= num_points[j];
			loci += locj;
		}
		
		return loci;
	}

	//! Generic Grid.
	/*!
	 * \ingroup Core
	 */
	class Grid: public Serializable
	{

	protected:

		std::vector<double> _lower; //!< Lower edge of the grid.
		std::vector<double> _upper; //!< Upper edge of the grid.
		std::vector<bool> _periodic; //!< Is the grid periodic in the corresponding dimension?
		std::vector<int> _num_points; //!< Number of grid points.
		std::vector<double> _spacing; //!< Grid spacing.
		int _NDim; //!< Grid dimension.

		//! Array storing grid data.
		std::vector<std::pair<std::vector<double>, std::vector<double>>> _flatvector;
	
	public:
		//! Iterator for traversing the Grid.
		using const_iterator = std::vector<std::pair<std::vector<double>, std::vector<double>>>::const_iterator;

		//! Destructor.
		virtual ~Grid(){}

		//! Return the nearest indices for given values.
		/*!
		 * \param val N-dimensional value.
		 * \return N-dimensional vector with nearest grid indices.
		 *
		 * Returns the grid point which is the nearest to a given value. The
		 * value must be given as a N-dimensional vector and the grid point is
		 * returned as an N-dimensional vector. Here, N is the dimension of the
		 * Grid.
		 */
		std::vector<int> GetIndices(const std::vector<double> &val)
		{
			std::vector<int> vertices;

			for(size_t i = 0; i < val.size(); i++)
			{
				int vertex = 0;
				if(val[i]<_lower[i])
					vertex = int((val[i] - _lower[i])/_spacing[i] - 0.5);
				else
					vertex = int((val[i] - _lower[i])/_spacing[i] + 0.5);

				if(_periodic[i])
				{
					vertex = vertex % _num_points[i];
					if(vertex<0)
						vertex += _num_points[i];
				}

				if(vertex < 0) // out of bounds
					vertex = 0;
				else if(vertex >= _num_points[i]) // out of bounds
					vertex = _num_points[i] - 1;

				vertices.push_back(vertex);
			}

			return vertices;
		}

		//! Get the location at the current indices.
		/*!
		 * \param indices Indices specifying grid point.
		 * \return vector of positions at the specified grid point.
		 */
		std::vector<double> GetLocation(const std::vector<int> &indices) const
		{
			return _flatvector[FlattenIndices(indices,_num_points)].second;
		}

		//! Get the value at the current indices.
		/*!
		 * \param indices Indices specifying grid point.
		 * \return Value at the specified grid point.
		 */
		double GetValue(const std::vector<int>& indices) const
		{
			return _flatvector[FlattenIndices(indices,_num_points)].first[0];
		}

		//! Set the value at the current incices.
		/*!
		 * \param indices Indices specifying the grid point.
		 * \param value New value for the grid point.
		 */
		void SetValue(const std::vector<int>& indices, double value)
		{
			_flatvector[FlattenIndices(indices,_num_points)].first[0] = value;
		}

		//! Get the extra at the current indices.
		/*!
		 * \param indices Indices specifying grid point.
		 * \return Value at the specified grid point.
		 */
		std::vector<double> GetExtra(const std::vector<int>& indices) const
		{
			const std::vector<double>& temp = _flatvector[FlattenIndices(indices,_num_points)].first;
			std::vector<double> temp2(&temp[1], &temp[temp.size()]);
			return temp2;
		}

		//! Set the extra at the current incices.
		/*!
		 * \param indices Indices specifying the grid point.
		 * \param value New value for the grid point.
		 */
		void SetExtra(const std::vector<int>& indices, std::vector<double> value)
		{
			int flatten = FlattenIndices(indices, _num_points);
			if(value.size() != _flatvector[flatten].first.size()-1)
				throw std::out_of_range("Vector field in grid is not the same size as value");
			for(size_t i = 0; i < value.size(); i++)
				_flatvector[flatten].first[i+1] = value[i];
		}

		//! Write Grid to console
		/*!
		 * Currently only for debugging
		 */
		void PrintGrid() const
		{
			for(size_t i = 0; i<_flatvector.size(); i++)
			{
				for(size_t j = 0; j<_flatvector[i].first.size(); j++)
					std::cout << _flatvector[i].first[j]<<" ";
				for(size_t j = 0; j<_flatvector[i].second.size(); j++)
					std::cout<<_flatvector[i].second[j]<< " ";
				std::cout<<std::endl;
			}
		}

		//! Return lower edges of the Grid.
		/*!
		 * \return Vector containing the lower edges of the grid in each dimension.
		 */
		std::vector<double> GetLower()
		{
			return _lower;
		}

		//! Return upper edges of the Grid.
		/*!
		 * \return Vector containing the upper edges of the grid in each dimension.
		 */
		std::vector<double> GetUpper()
		{
			return _upper;
		}

		//! Get periodicity of the Grid.
		/*!
		 * \return Vector with boolean values.
		 *
		 * The vector contains \c True if the Grid is periodic in the
		 * corresponding dimension.
		 */
		std::vector<bool> GetPeriodic()
		{
			return _periodic;
		}

		//! Get number of Grid points.
		/*!
		 * \return Vector containing number of grid points in each dimension.
		 */
		std::vector<int> GetNumPoints()
		{
			return _num_points;
		}

		//! Get grid spacing.
		/*!
		 * \return Vector containing grid spacing in each dimension.
		 */
		std::vector<double> GetSpacing()
		{
			return _spacing;
		}

		//! Get dimensionality of the Grid.
		/*!
		 * \return Grid dimension.
		 */
		int GetDimension()
		{
			return _NDim;
		}

		//! Set up the Grid.
		/*!
		 * \param json JSON input value.
		 * \return Pointer to the Grid. \c nullptr if unknown error occured.
		 *
		 * This function builds a Grid from a JSON node. If an unknown error
		 * occured, the return value is \c nullptr, but generally, the function
		 * will throw a BuildException on failure.
		 * \note Object lifetime is the caller's responsibility.
		 */
		static Grid* BuildGrid(const Json::Value& json);

		//! Set up Grid.
		/*!
		 * \param json JSON input value.
		 * \param path Path for JSON path specification
		 * \return Pointer the the Grid built.
		 *
		 * This function overloads Grid::BuildGrid(const Json::Value&) to allow
		 * JSON path specification.
		 */
		static Grid* BuildGrid(const Json::Value& json, 
							   const std::string& path);

		virtual std::vector<std::vector<int>> GetVoxel(const std::vector<double> &val) const = 0;

		virtual double InterpolateValue(const std::vector<double> &val) const = 0;

		virtual double InterpolateDeriv(const std::vector<double> &val, int dim) const = 0;

		//! \copydoc Serializable::Serialize()
		virtual void Serialize(Json::Value& json) const override
		{

		}

		//! Get iterator to first element of the Grid.
		/*!
		 * \return Iterator to first element in the Grid.
		 */
		const_iterator begin() const { return _flatvector.begin();}

		//! Get iterator one beyond last element of the Grid.
		/*!
		 * \return Iterator pointing to after the last element in the Grid.
		 */
		const_iterator end() const { return _flatvector.end(); }
	};
	
}

// +-------------------------------------------------------------------------
// | EGraphCache.h
// | 
// | Author: Gilbert Bernstein
// +-------------------------------------------------------------------------
// | COPYRIGHT:
// |    Copyright Gilbert Bernstein 2013
// |    See the included COPYRIGHT file for further details.
// |    
// |    This file is part of the Cork library.
// |
// |    Cork is free software: you can redistribute it and/or modify
// |    it under the terms of the GNU Lesser General Public License as
// |    published by the Free Software Foundation, either version 3 of
// |    the License, or (at your option) any later version.
// |
// |    Cork is distributed in the hope that it will be useful,
// |    but WITHOUT ANY WARRANTY; without even the implied warranty of
// |    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// |    GNU Lesser General Public License for more details.
// |
// |    You should have received a copy 
// |    of the GNU Lesser General Public License
// |    along with Cork.  If not, see <http://www.gnu.org/licenses/>.
// +-------------------------------------------------------------------------

#pragma once

#include <vector>

#include <boost\container\static_vector.hpp>

#include "..\Util\SparseVector.h"



namespace Cork
{


	//	The TIDs vector length never seems to go beyond 4 but I will double that just to be sure
	//		as static vectors will return junk if they pass beyond their limit.

	#define	EGRAPH_ENTRY_TIDS_VEC_LENGTH		8

	typedef boost::container::static_vector<IndexType, EGRAPH_ENTRY_TIDS_VEC_LENGTH>		EGraphEntryTIDVector;


	class EGraphEntry : public SEFUtility::SparseVectorEntry
	{
	public :

		EGraphEntry( IndexType		index )
			: SEFUtility::SparseVectorEntry( index ),
			  m_vid( index )
		{}



		IndexType						vid() const
		{
			return( m_vid );
		}

		const EGraphEntryTIDVector&		tids() const
		{
			return( m_tids );
		}

		EGraphEntryTIDVector&			tids()
		{
			return( m_tids );
		}

		bool							isIsct() const
		{
			return( m_isIsct );
		}

		void							setIsIsct( bool		newValue )
		{
			m_isIsct = newValue;
		}


	private :

		IndexType					m_vid;
		EGraphEntryTIDVector		m_tids;
		bool						m_isIsct;			
	};



	class EGraphSkeletonColumn : public SEFUtility::SparseVector<EGraphEntry,10>
	{
	public :

		EGraphSkeletonColumn()
		{}
	};


	

	class EGraphCache
	{

	public :

		typedef std::vector<EGraphSkeletonColumn>		SkeletonColumnVector;



		EGraphCache()
		{}

		~EGraphCache()
		{}



		void							resize( size_t		newSize )
		{
			m_skeleton.resize( newSize );
		}


		const SkeletonColumnVector&		columns() const
		{
			return( m_skeleton );
		}

		SkeletonColumnVector&			columns()
		{
			return( m_skeleton );
		}

		EGraphSkeletonColumn&			operator[]( IndexType		index )
		{
			return( m_skeleton[index] );
		}


	private :

		SkeletonColumnVector		m_skeleton;
	};

}	//	namespace Cork



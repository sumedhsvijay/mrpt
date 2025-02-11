/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */
#pragma once

#include <mrpt/core/exceptions.h>
#include <mrpt/graphs/TNodeID.h>
#include <mrpt/serialization/CSerializable.h>
#include <mrpt/serialization/stl_serialization.h>

#include <cstdint>
#include <iostream>
#include <string>

namespace mrpt::graphs::detail
{
/**\brief  Abstract class from which NodeAnnotations related classes can be
 * implemented
 *
 * \ingroup mrpt_graphs_grp
 */
struct TNodeAnnotations
{
	using self_t = TNodeAnnotations;

	/**\brief Constructor */
	TNodeAnnotations() = default;
	/**\brief Destructor */
	virtual ~TNodeAnnotations() = default;

	virtual void getAnnotsAsString(std::string* s) const { ASSERT_(s); }
	std::string retAnnotsAsString() const
	{
		std::string s;
		this->getAnnotsAsString(&s);
		return s;
	}

	virtual bool operator==(const TNodeAnnotations& other) const
	{
		return true;
	}
	virtual bool operator!=(const TNodeAnnotations& other) const
	{
		return (!(*this == other));
	}

	/**\brief Create and return a copy of the TNodeAnnotations object at hand.
	 *
	 * \warning Caller is responsible of afterwards deleting the object which is
	 * allocaed in heap
	 */
	TNodeAnnotations* getCopyOfAnnots() const { return new TNodeAnnotations(); }
	/**\brief Set the properties of the current TNodeAnnotations object
	 *
	 * \return True if setting the annotations part is successful.
	 */
	bool setAnnots(const self_t& other) { return true; }
	/**\brief Indicates if this is a dummy TNodeAnnotations struct or if it does
	 * contain meaningful data
	 */
	bool is_node_annots_empty{false};
};

/////////////////////////////////////////////////////////////////////////////

/**\brief Struct to be used as the NODE_ANNOTATIONS template argument in
 * CNetworkOfPoses class instances for use in multiple-robot SLAM applications
 *
 * \ingroup mrpt_graphs_grp
 */
struct TNodeAnnotationsEmpty : public TNodeAnnotations
{
	TNodeAnnotationsEmpty() { this->is_node_annots_empty = true; }
	DECLARE_TTYPENAME_CLASSNAME(mrpt::graphs::detail::TNodeAnnotationsEmpty)
};
}  // namespace mrpt::graphs::detail

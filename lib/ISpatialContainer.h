#ifndef NC_DS_ISPATIALCONTAINER_H_
#define NC_DS_ISPATIALCONTAINER_H_

#include "IContainer.h"
#include "AABB.h"

namespace NC::DataStructures {

	template <typename T>
	class ISpatialContainer : public IContainer<T> {

	public:

		using Predicate = IContainer<T>::Predicate;
		using QueryCallback = std::function<void(const T&)>;

		virtual void Query(const AABB& range, QueryCallback callback, int nodeIndex = 0) const = 0;
		virtual void Query(const AABB& range, std::vector<T>& elements, int nodeIndex = 0) const = 0;
		virtual std::vector<T> Query(const AABB& range) const = 0;
		virtual int RemoveIf(const AABB& range, Predicate pred) = 0;
	};

}

namespace NCDS = NC::DataStructures;

#endif // NC_DS_ISPATIALCONTAINER_H_

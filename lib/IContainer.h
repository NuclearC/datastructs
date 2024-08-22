#ifndef NC_DS_ICONTAINER_H_
#define NC_DS_ICONTAINER_H_

#include <functional>

namespace NC::DataStructures {

	template <typename T>
	class IContainer {

	public:

		using Predicate = std::function<bool(const T& )>;

		virtual int Insert(const T& item) = 0;
		virtual bool Contains(const T& item) const = 0;
		virtual bool Contains(Predicate pred) const = 0;

		virtual int Count(const T& item) const  = 0;
		virtual int CountIf(Predicate pred) const  = 0;
		virtual bool Remove(const T& item) = 0;
		virtual int RemoveIf(Predicate pred) = 0;

		virtual bool Empty() const  = 0;
	};

}

namespace NCDS = NC::DataStructures;

#endif // NC_DS_ICONTAINER_H_

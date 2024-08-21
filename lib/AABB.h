#ifndef NC_DS_AABB_H_
#define NC_DS_AABB_H_

#include <utility>

namespace NC::DataStructures {

	using Scalar = float;

	struct AABB {
		Scalar left, top, right, bottom;

		Scalar GetWidth() const { return right - left; }
		Scalar GetHeight() const { return bottom - top; }

		std::pair<Scalar, Scalar> GetCenter() const {
			return std::make_pair((left + right) / Scalar{2}, (top + bottom) / Scalar{2});
		}

		bool Contains(const AABB& other) const {
			return other.left >= left && other.right <= right &&
				other.top >= top && other.bottom <= bottom;
		}

		bool Contains(Scalar x, Scalar y) const {
			return x >= left && x <= right && y >= top && y <= bottom;
		}

		bool Intersects(const AABB& other) const {
			return top <= other.bottom && other.top <= bottom &&
				left <= other.right && other.left <= right;
		}
	};
}

#endif // NC_DS_AABB_H_

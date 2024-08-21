#ifndef NC_DS_QUADTREE_H_
#define NC_DS_QUADTREE_H_

#include "ISpatialContainer.h"
#include "AABB.h"

#include <vector>
#include <concepts>
#include <algorithm>

namespace NC::DataStructures {

	using Point = std::pair<Scalar, Scalar>;

	template <typename T>
	concept HasPoint = requires(T a) {
		{ a.GetPoint() } -> std::convertible_to<const Point&>;
	};

	template <typename T>
	concept HasAABB = requires(T a) {
		{ a.GetAABB() } -> std::convertible_to<const AABB&>;
	};

	template <typename T, int MaxDepth = 5, int NodeCapacity = 3> requires HasAABB<T>
	class QuadTree : public ISpatialContainer<T> {

	public:

		static constexpr int NodeCount() {
			int res = 0, j = 1;
			for (int i = 1; i <= MaxDepth; i++)
			{
				res += j;
				j *= 4;
			}
			return res;
		}

		struct QuadTreeNode {
			AABB range;
			std::vector<T> items;
		};

	protected:
		std::vector<QuadTreeNode> nodes;

		void ResolveRanges(int index = 0) {

			nodes[index].items.reserve(NodeCapacity);

			int curIdx = 4 * index + 1;
			if (curIdx >= NodeCount()) return;

			auto const& curRange = nodes[index].range;
			auto curCenter = curRange.GetCenter();

			// TOP LEFT
			nodes[curIdx].range = {
				curRange.left,
				curRange.top,
				curCenter.first,
				curCenter.second,
			};
			ResolveRanges(curIdx);
			curIdx++;

			// TOP RIGHT
			nodes[curIdx].range = {
				curCenter.first,
				curRange.top,
				curRange.right,
				curCenter.second,
			};
			ResolveRanges(curIdx);
			curIdx++;

			// BOTTOM LEFT
			nodes[curIdx].range = {
				curRange.left,
				curCenter.second,
				curCenter.first,
				curRange.bottom,
			};
			ResolveRanges(curIdx);
			curIdx++;

			// BOTTOM RIGHT
			nodes[curIdx].range = {
				curCenter.first,
				curCenter.second,
				curRange.right,
				curRange.bottom,
			};
			ResolveRanges(curIdx);
		}

		int ResolveQuadrant(AABB const& parent, AABB const& resolvee) {
			auto pCenter = parent.GetCenter();
			if (resolvee.bottom <= pCenter.second) { // top
				if (resolvee.right <= pCenter.first) { // left
					return 1;
				}
				else if (resolvee.left >= pCenter.first) { // right
					return 2; 
				}
			}
			else if (resolvee.top >= pCenter.second) { // bottom
				if (resolvee.right <= pCenter.first) { // left
					return 3;
				}
				else if (resolvee.left >= pCenter.first) { // right
					return 4;
				}
			}

			return -1;
		}

		int FindNode(const AABB& range) {
			int nodeIndex = 0;

			while (nodes[nodeIndex].range.Contains(range)) {
				auto& curNode = nodes[nodeIndex];
				int childIdx = ResolveQuadrant(curNode.range, range);

				if (childIdx == -1)
					break;

				int childNodeIndex = nodeIndex * 4 + childIdx;
				if (childNodeIndex >= NodeCount())
					break;
				nodeIndex = childNodeIndex;
			}

			return nodeIndex;
		}

	public:
		using Predicate = ISpatialContainer<T>::Predicate;
		using QueryCallback = ISpatialContainer<T>::QueryCallback;

		QuadTree() {
			nodes.resize(NodeCount());
		}

		explicit QuadTree(const AABB& bounds) {
			nodes.resize(NodeCount());

			nodes[0] = QuadTreeNode{
				.range = bounds,
				.items = {}
			};

			ResolveRanges();
		}

		virtual int Insert(const T& item) override {
			const AABB& itemAabb = item.GetAABB();
			auto nodeIndex = FindNode(itemAabb);
			nodes[nodeIndex].items.push_back(item);
			return nodeIndex;
		}

		virtual bool Empty() override {
			return std::any_of(std::begin(nodes), std::end(nodes), [] (auto const& node) {
				return !node.items.empty();
							   });
		}

		/**
		 * @brief Returns a pointer to the child nodes of the specified node
		 * @param nodeIndex the index of the specified node (0 is the root)
		 * @return a pointer to 4 contiguous child nodes or nullptr
		 */
		QuadTreeNode* GetChildren(int nodeIndex = 0) {
			nodeIndex = nodeIndex * 4 + 1;
			if (nodeIndex >= NodeCount())
				return nullptr;
			return &nodes[nodeIndex];
		}

		/**
		 * @brief returns true if the specified node or any of the childs are not empty
		 * @param nodeIndex the index of the specified node
		 * @return true or false
		 */
		bool Empty(int nodeIndex) {
			bool isEmpty = nodes[nodeIndex].items.empty();
			for (int i = 4 * nodeIndex + 1; i <= std::min(NodeCount() - 1, 4 * nodeIndex + 4); i++) {
				isEmpty = isEmpty && Empty(i);
			}

			return isEmpty;
		}

		/**
		 * @brief Query a range in the QuadTree.
		 * @param range the range
		 * @param callback the callback
		 * @param nodeIndex the index of the root node
		 */
		virtual void Query(const AABB& range, QueryCallback callback, int nodeIndex = 0) override {

			if (nodes[nodeIndex].range.Intersects(range)) {
				for (auto& item : nodes[nodeIndex].items) {
					if (item.GetAABB().Intersects(range))
						callback(item);
				}

				for (int i = 4 * nodeIndex + 1; i <= std::min(NodeCount() - 1, 4 * nodeIndex + 4); i++) {
					if (nodes[i].range.Intersects(range))
						Query(range, callback, i);
				}
			}

		}

		/**
		 * @brief Query a range in the QuadTree.
		 * @param range the range
		 * @param elements output elements
		 * @param nodeIndex the index of the root node
		 */
		virtual void Query(const AABB& range, std::vector<T>& elements, int nodeIndex = 0) override {

			if (nodes[nodeIndex].range.Intersects(range)) {
				for (auto& item : nodes[nodeIndex].items) {
					if (item.GetAABB().Intersects(range))
						elements.push_back(item);
				}

				for (int i = 4 * nodeIndex + 1; i <= std::min(NodeCount() - 1, 4 * nodeIndex + 4); i++) {
					if (nodes[i].range.Intersects(range))
						Query(range, elements, i);
				}
			}

		}

		/**
		 * @brief Query a range in the QuadTree.
		 * @param range the range
		 * @return a vector of elements in the range
		 */
		virtual std::vector<T> Query(const AABB& range) override {
			std::vector<T> elements;
			Query(range, elements, 0);
			return elements;
		}

		virtual bool Contains(const T& item) override { return false; }
		virtual bool Contains(Predicate pred) override { return false; }

		virtual int Count(const T& item) override { return 0; }

		virtual int CountIf(Predicate pred) override { return 0; }

		virtual bool Remove(const T& item) override {
			const AABB& itemAabb = item.GetAABB();
			auto nodeIndex = FindNode(itemAabb);
			auto& nodeItems = nodes[nodeIndex].items;

			auto removedItems = std::erase(nodeItems, item);

			if (removedItems == 0)
				__debugbreak();

			return removedItems > 0;
		}

		virtual int RemoveIf(Predicate pred) override {
			return 0;
		}

		virtual int RemoveIf(const AABB& range, Predicate pred) override {
			auto nodeIndex = FindNode(range);
			auto& nodeItems = nodes[nodeIndex].items;

			auto removedItems = std::erase_if(nodeItems, pred);

			return static_cast<int>(removedItems);
		}

		constexpr int GetDepth() { return MaxDepth; }

		auto&& GetNodes(this auto&& self) {
			return self.nodes;
		}

	};

}

#endif // NC_DS_QUADTREE_H_

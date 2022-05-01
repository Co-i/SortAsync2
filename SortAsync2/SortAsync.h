#pragma once
#include"ap.h"
#include<exception>
#include<future>
#include<algorithm>
#include<iostream>
#include<memory>

template<typename T>
struct TreeNode {
	T Value;
	std::shared_ptr<TreeNode> Left;
	std::shared_ptr<TreeNode> Right;
	TreeNode() :Left(nullptr), Right(nullptr) {}
};

template<typename T>
void Sort(T* first, T* last, size_t threshold, std::shared_ptr< TreeNode<std::future<void>> > this_node);
template<typename T>
T* Partition(T* first, T* last);
template<typename T>
void Manage(T* begin, T* end, size_t threshold);

template<typename T>
std::future<void> SortAsync(T* begin, T* end, size_t threshold = 10000) {
	if (end < begin)
		throw new std::exception("invaid pointers to SortAsync");
	if (threshold == 0)
		threshold = 1;

	return std::async(Manage<T>, begin, end, threshold);
}


template<typename T>
void Sort(T* first, T* last, size_t threshold, std::shared_ptr< TreeNode<std::future<void>> > this_node) {
	if (last < first + threshold) {
		std::sort(first, last + 1);
		return;
	}
	auto pivot = Partition<T>(first, last);
	if (first < pivot) {
		auto left_node = std::make_shared< TreeNode<std::future<void>> >();
		left_node->Value = std::async(/*std::launch::async,*/ Sort<T>, first, pivot - 1, threshold, left_node);
		this_node->Left = left_node;
	}
	if (pivot < last) {
		auto right_node = std::make_shared< TreeNode<std::future<void>> >();
		right_node->Value = std::async(/*std::launch::async,*/ Sort<T>, pivot + 1, last, threshold, right_node);
		this_node->Right = right_node;
	}
	return;
}

template<typename T>
T* Partition(T* first, T* last) {
	using std::swap;
	swap(*(first + randomNum(0, last - first)), *last);
	T* larger = first;
	for (T* smaller = first; smaller < last; ++smaller) {
		if (*smaller < *last) {
			if (smaller != larger)
				swap(*larger, *smaller);
			++larger;
		}
	}
	swap(*larger, *last);
	return larger;
}

template<typename T>
void Manage(T* begin, T* end, size_t threshold) {
	std::vector<std::shared_ptr<TreeNode<std::future<void>>>> Tree;
	Tree.reserve(end - begin);
	auto node = std::make_shared< TreeNode<std::future<void>> >();
	node->Value = std::async(Sort<T>, begin, end - 1, threshold, node);
	Tree.emplace_back(node);
	for (auto curr = Tree.begin(); curr != Tree.end(); ++curr) {
		node = *curr;
		node->Value.get();
		if (node->Left != nullptr)
			Tree.emplace_back(node->Left);
		if (node->Right != nullptr)
			Tree.emplace_back(node->Right);
	}
}

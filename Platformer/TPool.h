#pragma once

#include <bitset>
#include <stack>
#include <cassert>

namespace BD
{
	typedef unsigned long unit32;
	typedef unsigned long long uint64;
	typedef uint64 Handle;
}

template<class T, size_t MAX_SIZE = 1024>
class TPool
{
public:
	BD::Handle AddObject(T * obj)
	{
		if (mFreeIndices.empty())
		{
			throw std::runtime_error("Pool is full!");
		}

		BD::uint32 index = mFreeIndices.top();
		mFreeIndices.pop();

		mStorage[index] = obj;
		mIsOccupied.set(index);
		mVersions[index]++; // Increment version to ensure stale handles are invalid

		return PackHandle(index, mVersions[index]);
	}

	T * Get(BD::Handle handle)
	{
		BD::uint32 index = ExtractIndex(handle);
		BD::uint32 version = ExtractVersion(handle);

		if (index >= MAX_SIZE || !mIsOccupied.test(index) || mVersions[index] != version)
		{
			return nullptr; // Invalid handle or object no longer exists
		}

		return mStorage[index];
	}

	void Remove(BD::Handle handle)
	{
		BD::uint32 index = ExtractIndex(handle);
		BD::uint32 version = ExtractVersion(handle);

		if (index >= MAX_SIZE || !mIsOccupied.test(index) || mVersions[index] != version)
		{
			return; // Prevent double-free or invalid removal
		}

		delete mStorage[index];
		mStorage[index] = nullptr;
		mIsOccupied.reset(index);
		mFreeIndices.push(index);
	}

private:

	inline Handle PackHandle(uint32 index, uint32 version)
	{
		return (static_cast<Handle>(version) << 32) | index;
	}

	inline uint32 ExtractIndex(Handle handle)
	{
		return static_cast<uint32>(handle & 0xFFFFFFFF);
	}

	inline uint32 ExtractVersion(Handle handle)
	{
		return static_cast<uint32>((handle >> 32) & 0xFFFFFFFF);
	}

	T * mStorage[MAX_SIZE] = { nullptr };
	std::bitset<MAX_SIZE> mIsOccupied; // Fast lookup of allocated objects
	BD::uint32 mVersions[MAX_SIZE] = { 0 }; // Tracks version numbers
	std::stack<BD::uint32> mFreeIndices; // Stores available indices
};
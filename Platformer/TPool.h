#pragma once

#include <bitset>
#include <stack>
#include <cassert>

namespace BD
{
	typedef unsigned long uint32;
	typedef unsigned long long uint64;
	typedef uint64 Handle;
}

template<class T, size_t MAX_SIZE = 1024>
class TPool
{
public:
	TPool()
	{
		for (BD::uint32 ii = 0; ii < MAX_SIZE; ++ii)
		{
			mFreeIndices.push(ii);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------

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
		++mVersions[index];

		return PackHandle(index, mVersions[index]);
	}

	//------------------------------------------------------------------------------------------------------------------------

	T * Get(BD::Handle handle)
	{
		BD::uint32 index = ExtractIndex(handle);
		BD::uint32 version = ExtractVersion(handle);

		if (index >= MAX_SIZE || !mIsOccupied.test(index) || mVersions[index] != version || !mStorage[index])
		{
			return nullptr;
		}

		return mStorage[index];
	}

	//------------------------------------------------------------------------------------------------------------------------

	void Remove(BD::Handle handle)
	{
		BD::uint32 index = ExtractIndex(handle);
		BD::uint32 version = ExtractVersion(handle);

		if (index >= MAX_SIZE || !mIsOccupied.test(index) || mVersions[index] != version)
		{
			return;
		}

		delete mStorage[index];
		mStorage[index] = nullptr;
		mIsOccupied.reset(index);
		++mVersions[index];
		mFreeIndices.push(index);
	}

	//------------------------------------------------------------------------------------------------------------------------

private:

	inline BD::Handle PackHandle(uint32 index, uint32 version)
	{
		return (static_cast<BD::Handle>(version) << 32) | index;
	}

	//------------------------------------------------------------------------------------------------------------------------

	inline uint32 ExtractIndex(BD::Handle handle)
	{
		return static_cast<uint32>(handle & 0xFFFFFFFF);
	}

	//------------------------------------------------------------------------------------------------------------------------

	inline uint32 ExtractVersion(BD::Handle handle)
	{
		return static_cast<uint32>((handle >> 32) & 0xFFFFFFFF);
	}

	//------------------------------------------------------------------------------------------------------------------------

	T * mStorage[MAX_SIZE] = { nullptr }; // Stores pointers to objects
	std::bitset<MAX_SIZE> mIsOccupied; // Tracks which slots are in use
	BD::uint32 mVersions[MAX_SIZE] = { 0 }; // Stores version numbers for handles
	std::stack<BD::uint32> mFreeIndices; // Stores available indices for allocation
};
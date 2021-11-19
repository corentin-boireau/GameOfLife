#pragma once

#include <memory>

template<typename T, size_t N>
class heap_array final
{
public:
	heap_array() : m_data(std::make_unique<T[]>(N)) {}
	heap_array(std::unique_ptr<T[]>&& data) : m_data(std::move(data)) {}
	
	inline size_t size() const                   { return N; }
	inline T& operator[](size_t pos)             { return m_data[pos]; }
	inline T const& operator[](size_t pos) const { return m_data[pos]; }
	inline T* get() const                        { return m_data.get(); }
	inline void fill(T const& elem)              { std::fill(m_data.get(), m_data.get() + N, elem); }
private:
	std::unique_ptr<T[]> m_data;
};
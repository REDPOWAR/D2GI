#pragma once

#include <cassert>
#include <new>
#include <string>

template<typename CharT>
class BasicDelimStringReader
{
public:
	using value_type = CharT;

	BasicDelimStringReader(size_t size)
		: m_buffer(new(std::nothrow) value_type[size]), m_size(size)
	{
	}

	~BasicDelimStringReader()
	{
		delete[] m_buffer;
	}

	value_type* PutBuffer()
	{
		m_cursor = m_buffer;
		return m_buffer;
	}

	size_t GetSize() const
	{
		return m_size;
	}

	const value_type* GetString(size_t* length = nullptr)
	{
		assert(m_cursor != nullptr);
		if (*m_cursor == '\0')
		{
			if (length != nullptr) *length = 0;
			return nullptr;
		}
		const value_type* curString = m_cursor;
		const size_t len = std::char_traits<value_type>::length(m_cursor);
		m_cursor += len + 1;

		if (length != nullptr) *length = len;
		return curString;
	}

private:
	value_type* const m_buffer;
	const value_type* m_cursor = nullptr;
	const size_t m_size;
};

using DelimStringReader = BasicDelimStringReader<char> ;
using WideDelimStringReader = BasicDelimStringReader<wchar_t>;

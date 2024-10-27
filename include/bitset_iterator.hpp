#ifndef __BIT_BITSET_ITERATOR__
#define __BIT_BITSET_ITERATOR__ 1

#include <cinttypes>
#include <iterator>

namespace bit
{

  namespace bitset_iterator
  {

    class iterator
    {
    public:
      using size_type         = std::size_t;
      using reference         = std::uint8_t&;
      using pointer           = std::uint8_t*;
      using iterator_category = std::forward_iterator_tag;
    private:
      struct proxy_iterator
      {
      public:
        using bit_state = bool;

        enum class BMASK : size_type
        {
          BIT = 0b10000000
        };
      public:
        pointer   m_byte;
        size_type m_bit;

        proxy_iterator(pointer ptr, size_type bit_pos)
          : m_byte (ptr)
          , m_bit  (bit_pos)
        {}

        proxy_iterator(const proxy_iterator& other)
          : m_byte (other.m_byte)
          , m_bit  (other.m_bit)
        {}

        proxy_iterator(proxy_iterator&& other)
          : m_byte (other.m_byte)
          , m_bit  (other.m_bit)
        {}
        
        proxy_iterator& operator=(bit_state value)
        {
          if (value)
            m_byte[m_bit >> 3] |= static_cast<size_type>(BMASK::BIT) >> (m_bit & 0b00000111);
          else
            m_byte[m_bit >> 3] &= ~(static_cast<size_type>(BMASK::BIT) >> (m_bit & 0b00000111));
          
          return *this;
        }

        bool operator==(const proxy_iterator& other) const noexcept
        {
          if (m_byte == other.m_byte && m_bit == other.m_bit)
            return true;
          else
            return false;
        }

        bool operator!=(const proxy_iterator& other) const noexcept
        {
          return !(*this == other);
        }

        operator bool() const noexcept
        {
          return m_byte[m_bit >> 3] & static_cast<size_type>(BMASK::BIT) >> (m_bit & 0b00000111);
        }

        explicit operator char() const noexcept
        {
          return bool(*this) + '0';
        }
      } m_bit_iterator;
    public:
      iterator(pointer ptr, size_type bit_pos)
        : m_bit_iterator (ptr, bit_pos)
      {}

      iterator(const iterator& other)
        : m_bit_iterator (other.m_bit_iterator)
      {}

      iterator(iterator&& other)
        : m_bit_iterator (std::move(other.m_bit_iterator))
      {}

      proxy_iterator& operator*() noexcept { return m_bit_iterator; }

      iterator& operator++() noexcept
      {
        ++m_bit_iterator.m_bit;
        return *this;
      }

      iterator operator++(std::int32_t) noexcept
      {
        iterator tmp (*this);
        ++m_bit_iterator.m_bit;
        return tmp;
      }

      bool operator==(const iterator& other) const noexcept
      {
        return m_bit_iterator == other.m_bit_iterator;
      }

      bool operator!=(const iterator& other) const noexcept
      {
        return !(*this == other);
      }

    };

  }
}

#endif
#ifndef __BITS_BITSET__
#define __BITS_BITSET__ 1

#define calculate_capacity(bits) (((bits) >> 3) + ((bits) & 0b00000111 ? 1 : 0))
#define byte_division(bits) ((bits) >> 3)
#define byte_module(bits) ((bits) & 0b00000111)

#define BIT_SET   (true)
#define BIT_UNSET (false)

#include <cinttypes>
#include <cstring>
#include <stdexcept>
#include <string>

namespace bit
{

  template<std::size_t num_bits>
  class bitset
  {
  public:
    using bit_state = bool;
    using pointer   = std::uint8_t*;
    using size_type = std::size_t;
    using byte      = std::uint8_t;

    enum class BMASK : size_type
    {
      BIT   = 0b10000000,
      SET   = 0b11111111,
      RESET = 0b00000000
    };
  private:
    byte m_storage[calculate_capacity(num_bits)];
  public:
    constexpr bitset() noexcept
    {
      std::memset(m_storage,
                  static_cast<size_type>(BMASK::RESET),
                  calculate_capacity(num_bits)
           );
    }

    constexpr bitset(size_type value) noexcept : bitset()
    {
      std::memcpy(m_storage,
                  &value,
                  calculate_capacity(num_bits) >= sizeof(value) ? sizeof(value) : calculate_capacity(num_bits));
      
    }

    constexpr bitset(const bitset& other) noexcept
    {
      std::memcpy(m_storage,
                  other.m_storage,
                  calculate_capacity(num_bits)
           );
    }

    constexpr bitset(bitset&& other) noexcept
    {
      std::memcpy(m_storage,
                  other.m_storage,
                  calculate_capacity(num_bits)
           );
      std::memset(other.m_storage,
                  static_cast<size_type>(BMASK::RESET),
                  calculate_capacity(num_bits)
           );
    }

    ~bitset() = default;

    [[nodiscard]] constexpr size_type size() const noexcept { return num_bits; }

    [[nodiscard]] constexpr size_type count() noexcept
    {
      pointer end         {m_storage + calculate_capacity(num_bits) - 1};
      size_type bit_count {};

      for (pointer begin {m_storage}; begin != end; ++begin)
      {
        size_type byte {((*begin >> 1) & static_cast<size_type>(0b01010101)) +
                         (*begin & static_cast<size_type>(0b01010101))};
        byte = ((byte >> 2) & static_cast<size_type>(0b00110011)) +
                (byte & static_cast<size_type>(0b00110011));
        byte = ((byte >> 4) & static_cast<size_type>(0b00001111)) +
                (byte & static_cast<size_type>(0b00001111));
        bit_count += byte;
      }

      constexpr size_type remaining_bits {byte_module(num_bits) ? byte_module(num_bits) : 8};

      for (size_type current_bit {}; current_bit != remaining_bits; ++current_bit)
        if (*end & static_cast<size_type>(BMASK::BIT) >> current_bit) ++bit_count;

      return bit_count;
    }
    
    [[nodiscard]] constexpr bool all() const noexcept
    {
      pointer end {m_storage + calculate_capacity(num_bits)};

      for (pointer begin {m_storage}; begin != end; ++begin)
        if (!(*begin)) return false;

      return true;
    }

    [[nodiscard]] constexpr bool any() const noexcept
    {
      pointer end {m_storage + calculate_capacity(num_bits)};

      for (pointer begin {m_storage}; begin != end; ++begin)
        if (*begin) return true;
      return false;
    }

    [[nodiscard]] constexpr bool none() const noexcept
    {
      pointer end {m_storage + calculate_capacity(num_bits)};

      for (pointer begin {m_storage}; begin != end; ++begin)
        if (*begin) return false;

      return true;
    }
    
    [[nodiscard]] constexpr bit_state operator[](size_type index) const noexcept
    {
      return m_storage[index >> 3] & static_cast<size_type>(BMASK::BIT) >> index & 0b00000111;
    }
    
    [[nodiscard]] constexpr bit_state test(size_type index) const
    {
      if (index >= num_bits)
        std::out_of_range("bitset:test(size_type) -> index is out of range");

      return m_storage[index >> 3] & static_cast<size_type>(BMASK::BIT) >> index & 0b00000111;
    }

    constexpr bitset& set() noexcept
    {
      std::memset(m_storage,
                  static_cast<size_type>(BMASK::SET),
                  calculate_capacity(num_bits)
           );

      return *this;
    }

    constexpr bitset& set(size_type index, bit_state value = BIT_SET)
    {
      if (index >= num_bits)
        std::out_of_range("bitset:set(size_type, bit_state) -> index is out of range");

      if (value == BIT_SET)
        m_storage[index >> 3] |= static_cast<size_type>(BMASK::BIT) >> (index & 0b00000111);
      else
        m_storage[index >> 3] &= ~(static_cast<size_type>(BMASK::BIT) >> (index & 0b00000111));
      
      return *this;
    }

    constexpr bitset& reset() noexcept
    {
      std::memset(m_storage,
                  static_cast<size_type>(BMASK::RESET),
                  calculate_capacity(num_bits)
           );
           
      return *this;
    }

    constexpr bitset& reset(size_type index)
    {
      if (index >= num_bits)
        std::out_of_range("bitset:reset(size_type) -> index is out of range");

      m_storage[index >> 3] &= ~(static_cast<size_type>(BMASK::BIT) >> (index & 0b00000111));
      return *this;
    }

    constexpr bitset& flip() noexcept
    {
      pointer end {m_storage + calculate_capacity(num_bits)};

      for (pointer begin {m_storage}; begin != end; ++begin)
        *begin ^= static_cast<size_type>(BMASK::SET);

      return *this;
    }

    constexpr bitset& flip(size_type index)
    {
      if (index >= num_bits)
        std::out_of_range("bitset:flip(size_type) -> index is out of range");

      m_storage[index >> 3] ^= static_cast<size_type>(BMASK::BIT) >> (index & 0b00000111);
      return *this;
    }

    std::string to_string() const
    {
      std::string bin_string;

      try { bin_string.reserve(num_bits); }
      catch (std::exception& error) { throw; }

      for (size_type bit {}; bit != num_bits; ++bit)
        bin_string.push_back(
        static_cast<bool>(m_storage[bit >> 3] & static_cast<size_type>(BMASK::BIT) >> (bit & 0b00000111)
        ) + '0'
        );

      return bin_string;
    }

    constexpr bitset& operator&=(const bitset& other)
    {
      pointer end         {m_storage + calculate_capacity(num_bits)};
      pointer begin_other {other.m_storage};
      
      for (pointer begin {m_storage}; begin != end; ++begin, ++begin_other)
        *begin &= *begin_other;

      return *this;
    }

    constexpr bitset& operator|=(const bitset& other)
    {
      pointer end         {m_storage + calculate_capacity(num_bits)};
      pointer begin_other {other.m_storage};
      
      for (pointer begin {m_storage}; begin != end; ++begin, ++begin_other)
        *begin |= *begin_other;

      return *this;
    }

    constexpr bitset& operator^=(const bitset& other)
    {
      pointer end         {m_storage + calculate_capacity(num_bits)};
      pointer begin_other {other.m_storage};
      
      for (pointer begin {m_storage}; begin != end; ++begin, ++begin_other)
        *begin ^= *begin_other;

      return *this;
    }

    constexpr bitset& operator=(const bitset& other)
    {
      std::memcpy(m_storage,
                  other.m_storage,
                  calculate_capacity(num_bits)
           );
      
      return *this;
    }

    constexpr bitset& operator=(bitset&& other)
    {
      std::memcpy(m_storage,
                  other.m_storage,
                  calculate_capacity(num_bits)
           );
      std::memset(other.m_storage,
                  static_cast<size_type>(BMASK::RESET),
                  calculate_capacity(num_bits)
           );
      
      return *this;
    }

    [[nodiscard]] bool operator==(const bitset& other)
    {
      return std::memcmp(m_storage,
                         other.m_storage,
                         calculate_capacity(num_bits)
                  ) == 0;
    }

    [[nodiscard]] bool operator!=(const bitset& other)
    {
      return !(*this == other);
    }

  };

}

template<std::size_t num_bits>
[[nodiscard]] bit::bitset<num_bits> operator&(const bit::bitset<num_bits>& lhs,
                                              const bit::bitset<num_bits>& rhs)
{
  bit::bitset<num_bits> tmp (lhs);
  return tmp &= rhs;
}

template<std::size_t num_bits>
[[nodiscard]] bit::bitset<num_bits> operator|(const bit::bitset<num_bits>& lhs,
                                                const bit::bitset<num_bits>& rhs)
{
  bit::bitset<num_bits> tmp (lhs);
  return tmp |= rhs;
}

template<std::size_t num_bits>
[[nodiscard]] bit::bitset<num_bits> operator^(const bit::bitset<num_bits>& lhs,
                                              const bit::bitset<num_bits>& rhs)
{
  bit::bitset<num_bits> tmp (lhs);
  return lhs ^= rhs;
}

#undef BIT_SET
#undef BIT_UNSET
#undef calculate_capacity

#endif
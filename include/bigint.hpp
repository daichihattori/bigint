#include <sstream>
#include <iomanip>
#include <array>
#include <cstddef>
#include <gmp.h>
#include <iostream>
#include <vector>
#include <cctype>

namespace bigint {

// GMP's Limb type (usually unsigned long)
using limb_t = mp_limb_t;

// Number of bits in a limb (defined by GMP)
constexpr size_t limb_bit_count = GMP_NUMB_BITS;

// BigInt class template parameterized by number of bits
template <size_t Bits>
class BigInt {
private:
    static constexpr size_t NumLimbs = (Bits + limb_bit_count - 1) / limb_bit_count;
    using Storage = std::array<limb_t, NumLimbs>;
    Storage limbs;

public:
    BigInt() {
        limbs.fill(0);
    }

    explicit BigInt(uint64_t value) {
        limbs.fill(0);
        limbs[0] = static_cast<limb_t>(value);
    }

    // Constructor that takes a string and a base
    BigInt(const std::string& str, int base = 10) {
        limbs.fill(0);
        if (base < 2 || base > 256)
            throw std::invalid_argument("base must be in [2, 256]");

        // Convert string to byte array for GMP's mpn_set_str
        std::vector<unsigned char> str_bytes(str.size());
        for (size_t i = 0; i < str.size(); ++i) {
            char c = str[i];
            if (std::isdigit(static_cast<unsigned char>(c)))
                str_bytes[i] = c - '0';
            else if (std::isupper(static_cast<unsigned char>(c)))
                str_bytes[i] = 10 + (c - 'A');
            else if (std::islower(static_cast<unsigned char>(c)))
                str_bytes[i] = 36 + (c - 'a');
            else
                throw std::invalid_argument("invalid character in input string");
        }

        // Convert to GMP limb array
        mpn_set_str(limbs.data(), str_bytes.data(), str.size(), base);
    }


    // Clear to zero
    void clear() {
        limbs.fill(0);
    }

    const Storage& data() const { return limbs; }
    Storage& data() { return limbs; }

    std::string to_string(int base = 10) const {
        if (base < 2 || base > 256)
            throw std::invalid_argument("base must be in [2, 256]");

        size_t actual_limbs = NumLimbs;
        while (actual_limbs > 0 && limbs[actual_limbs - 1] == 0)
            --actual_limbs;

        if (actual_limbs == 0) return "0";

        // When base is not a power of 2, mpn_get_str may modify the input, so copy limbs
        std::vector<limb_t> temp_limbs(limbs.begin(), limbs.begin() + actual_limbs);

        // Allocate buffer size conservatively (should be large enough)
        std::vector<unsigned char> raw_buf(actual_limbs * limb_bit_count / 3 + 10);

        size_t count = mpn_get_str(raw_buf.data(), base, temp_limbs.data(), actual_limbs);

        // ASCII conversion (0-9, A-Z, a-...) supports up to base 256
        std::string result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            unsigned char digit = raw_buf[i];
            if (digit < 10)
                result += static_cast<char>('0' + digit);
            else if (digit < 36)
                result += static_cast<char>('A' + digit - 10);
            else
                result += static_cast<char>('a' + digit - 36);  // extended with a-
        }

        return result;
    }

    template <size_t OtherBits>
    BigInt<(Bits > OtherBits ? Bits : OtherBits)> add(const BigInt<OtherBits>& other, bool &carry) const {
        constexpr size_t SelfLimbs = (Bits + limb_bit_count - 1) / limb_bit_count;
        constexpr size_t OtherLimbs = (OtherBits + limb_bit_count - 1) / limb_bit_count;
        constexpr size_t ResultBits = (Bits > OtherBits ? Bits : OtherBits);
        constexpr size_t ResultLimbs = (ResultBits + limb_bit_count - 1) / limb_bit_count;

        BigInt<ResultBits> result;
        result.clear();

        limb_t tmp_carry = 0;

        if constexpr (SelfLimbs >= OtherLimbs) {
            tmp_carry = mpn_add(result.data().data(), limbs.data(), SelfLimbs, other.data().data(), OtherLimbs);
        } else {
            tmp_carry = mpn_add(result.data().data(), other.data().data(), OtherLimbs, limbs.data(), SelfLimbs);
        }

        carry = (tmp_carry != 0);
        return result;
    }

    template <size_t OtherBits>
    BigInt<(Bits > OtherBits ? Bits : OtherBits)> sub(const BigInt<OtherBits>& other, bool &carry) const {
        constexpr size_t SelfLimbs = (Bits + limb_bit_count - 1) / limb_bit_count;
        constexpr size_t OtherLimbs = (OtherBits + limb_bit_count - 1) / limb_bit_count;
        constexpr size_t ResultBits = (Bits > OtherBits ? Bits : OtherBits);
        constexpr size_t ResultLimbs = (ResultBits + limb_bit_count - 1) / limb_bit_count;

        BigInt<ResultBits> result;
        result.clear();

        limb_t tmp_carry = 0;

        if constexpr (SelfLimbs >= OtherLimbs) {
            tmp_carry = mpn_sub(result.data().data(), limbs.data(), SelfLimbs, other.data().data(), OtherLimbs);
        } else {
            tmp_carry = mpn_sub(result.data().data(), other.data().data(), OtherLimbs, limbs.data(), SelfLimbs);
        }

        carry = (tmp_carry != 0);
        return result;
    }

    template <size_t OtherBits>
    BigInt<Bits + OtherBits> mul(const BigInt<OtherBits>& other, bool& carry) const {
        constexpr size_t SelfLimbs = (Bits + limb_bit_count - 1) / limb_bit_count;
        constexpr size_t OtherLimbs = (OtherBits + limb_bit_count - 1) / limb_bit_count;
        constexpr size_t ResultBits = Bits + OtherBits;
        constexpr size_t ResultLimbs = SelfLimbs + OtherLimbs;

        BigInt<ResultBits> result;
        result.clear();

        limb_t tmp_carry = 0;
        if constexpr (SelfLimbs >= OtherLimbs) {
            tmp_carry = mpn_mul(result.data().data(), limbs.data(), SelfLimbs, other.data().data(), OtherLimbs);
        } else {
            tmp_carry = mpn_mul(result.data().data(), other.data().data(), OtherLimbs, limbs.data(), SelfLimbs);
        }

        carry = (tmp_carry != 0);
        return result;
    }
};

} // namespace bigint
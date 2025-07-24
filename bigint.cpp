#include <sstream>
#include <iomanip>
#include <array>
#include <cstddef>
#include <gmp.h>
#include <iostream>

namespace bigint {

// GMPのLimb型（通常はunsigned long）
using limb_t = mp_limb_t;

// limbのビット数（GMPが定義している）
constexpr size_t limb_bit_count = GMP_NUMB_BITS;

// Bits単位のテンプレートBigIntクラス
template <size_t Bits>
class BigInt {
public:
    static constexpr size_t NumLimbs = (Bits + limb_bit_count - 1) / limb_bit_count;

    using Storage = std::array<limb_t, NumLimbs>;

    BigInt() {
        limbs.fill(0);
    }

    explicit BigInt(uint64_t value) {
        limbs.fill(0);
        limbs[0] = static_cast<limb_t>(value);
    }

    // 文字列と基数を受け取るコンストラクタ
    BigInt(const std::string& str, int base = 10) {
        limbs.fill(0);
        if (base < 2 || base > 256)
            throw std::invalid_argument("base must be in [2, 256]");

        // 文字列を GMP の mpn_set_str 用バイト列に変換
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

        // GMP limb 配列へ変換
        mpn_set_str(limbs.data(), str_bytes.data(), str.size(), base);
    }

    // 足し算の例（未実装）
    BigInt operator+(const BigInt& other) const {
        BigInt result;
        // TODO: 実装 (mpn_add_nを使うとよい)
        return result;
    }

    // ゼロクリア
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

        // base ≠ 2^k の場合は limb が壊されるのでコピー
        std::vector<limb_t> temp_limbs(limbs.begin(), limbs.begin() + actual_limbs);

        // バッファサイズは保守的に確保（十分に大きければOK）
        std::vector<unsigned char> raw_buf(actual_limbs * limb_bit_count / 3 + 10);

        size_t count = mpn_get_str(raw_buf.data(), base, temp_limbs.data(), actual_limbs);

        // ASCII変換（0〜9, A〜Z, a〜...）最大256まで対応
        std::string result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            unsigned char digit = raw_buf[i];
            if (digit < 10)
                result += static_cast<char>('0' + digit);
            else if (digit < 36)
                result += static_cast<char>('A' + digit - 10);
            else
                result += static_cast<char>('a' + digit - 36);  // a〜 で拡張
        }

        return result;
    }
private:
    Storage limbs;
};

} // namespace bigint

int main() {
    using namespace bigint;

    BigInt<256> a(123456789);
    BigInt<256> b(987654321);
    std::cout << "a = " << a.to_string() << std::endl;

    BigInt<256> s_a("123456789", 10);
    BigInt<256> s_b("987654321", 10);
    std::cout << "a = " << a.to_string() << std::endl;

    BigInt<256> c = a + b;

    std::cout << "a + b = " << c.to_string() << std::endl;

    return 0;
}
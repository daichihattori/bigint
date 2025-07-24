#include <gtest/gtest.h>
#include "bigint.hpp"

using namespace bigint;

// Constructor from uint64_t
TEST(BigIntConstructorTest, FromUint64) {
    BigInt<256> a(123456789);
    EXPECT_EQ(a.to_string(10), "123456789");
}

// Constructor from decimal string
TEST(BigIntConstructorTest, FromStringDecimal) {
    BigInt<256> s_a("123456789", 10);
    EXPECT_EQ(s_a.to_string(10), "123456789");
}

// Constructor from hexadecimal string
TEST(BigIntConstructorTest, FromStringHex) {
    // 0x1abcdef (in decimal: 28126655)
    BigInt<256> s_hex("1ABCDEF", 16);
    EXPECT_EQ(s_hex.to_string(16), "1ABCDEF");
}

// to_string produces hex string from decimal value
TEST(BigIntToStringTest, HexFromDecimalValue) {
    BigInt<256> a(255);
    EXPECT_EQ(a.to_string(16), "FF");
}

// to_string for zero value
TEST(BigIntToStringTest, ZeroValue) {
    BigInt<256> z(0);
    EXPECT_EQ(z.to_string(10), "0");
    EXPECT_EQ(z.to_string(16), "0");
}

// Addition stub (not implemented): result should be zero
TEST(BigIntArithmeticTest, AddStub) {
    BigInt<256> a(123456789);
    BigInt<256> b(987654321);
    BigInt<256> c = a + b;
    EXPECT_EQ(c.to_string(10), "0");
}
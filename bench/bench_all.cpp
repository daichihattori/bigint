#include <benchmark/benchmark.h>
#include "bigint.hpp"
#include <gmp.h>
#include <gmpxx.h>

using namespace bigint;

struct BenchData {
    std::string a_str, b_str;
    BenchData(size_t bit_count) {
        gmp_randclass rr(gmp_randinit_default);
        rr.seed(time(NULL));
        mpz_class a = rr.get_z_bits(bit_count);
        mpz_class b = rr.get_z_bits(bit_count);
        a_str = a.get_str(10);
        b_str = b.get_str(10);
    }
};

static void BM_LimbInit(benchmark::State& state) {
    mp_size_t size = (state.range(0) + 63) / 64;
    for (auto _ : state) {
        mp_limb_t a[size];
        mpn_zero(a, size);
        benchmark::DoNotOptimize(a[0]);
    }
}

template<int Bits>
static void BM_BigIntInit(benchmark::State& state) {
    for (auto _ : state) {
        BigInt<Bits> a;
        benchmark::DoNotOptimize(a);
    }
}

static void BM_MpzInit(benchmark::State& state) {
    for (auto _ : state) {
        mpz_t a;
        mpz_init(a);
        benchmark::DoNotOptimize(a);
        mpz_clear(a);
    }
}

static void BM_LimbAdd(benchmark::State& state) {
    BenchData data(state.range(0));
    mpz_t a_z, b_z;
    mpz_init_set_str(a_z, data.a_str.c_str(), 10);
    mpz_init_set_str(b_z, data.b_str.c_str(), 10);

    mp_size_t size = (state.range(0) + 63) / 64;
    mp_limb_t a[size], b[size], result[size + 1];  // +1 for carry

    mpn_zero(a, size);
    mpn_zero(b, size);
    mpn_zero(result, size + 1);
    mpn_copyi(a, mpz_limbs_read(a_z), std::min<size_t>(static_cast<size_t>(mpz_size(a_z)), size));
    mpn_copyi(b, mpz_limbs_read(b_z), std::min<size_t>(static_cast<size_t>(mpz_size(b_z)), size));

    for (auto _ : state) {
        mpn_add_n(result, a, b, size);
        benchmark::DoNotOptimize(result[0]);
    }

    mpz_clear(a_z);
    mpz_clear(b_z);
}

template<int Bits>
static void BM_BigIntAdd(benchmark::State& state) {
    BenchData data(state.range(0));
    BigInt<Bits> a(data.a_str);
    BigInt<Bits> b(data.b_str);
    for (auto _ : state) {
        bool carry;
        auto result = a.add(b, carry);
        benchmark::DoNotOptimize(result);
    }
}

static void BM_MpzAdd(benchmark::State& state) {
    BenchData data(state.range(0));
    mpz_t a, b, result;
    mpz_init_set_str(a, data.a_str.c_str(), 10);
    mpz_init_set_str(b, data.b_str.c_str(), 10);
    mpz_init(result);
    for (auto _ : state) {
        mpz_add(result, a, b);
        benchmark::DoNotOptimize(result);
    }
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(result);
}

static void BM_LimbMul(benchmark::State& state) {
    BenchData data(state.range(0));
    mpz_t a_z, b_z;
    mpz_init_set_str(a_z, data.a_str.c_str(), 10);
    mpz_init_set_str(b_z, data.b_str.c_str(), 10);

    mp_size_t size = (state.range(0) + 63) / 64;
    mp_limb_t a[size], b[size], result[2 * size];

    mpn_zero(a, size);
    mpn_zero(b, size);
    mpn_zero(result, 2 * size);
    mpn_copyi(a, mpz_limbs_read(a_z), std::min<size_t>(static_cast<size_t>(mpz_size(a_z)), size));
    mpn_copyi(b, mpz_limbs_read(b_z), std::min<size_t>(static_cast<size_t>(mpz_size(b_z)), size));

    for (auto _ : state) {
        mpn_mul_n(result, a, b, size);
        benchmark::DoNotOptimize(result[0]);
    }

    mpz_clear(a_z);
    mpz_clear(b_z);
}

template<int Bits>
static void BM_BigIntMul(benchmark::State& state) {
    BenchData data(state.range(0));
    BigInt<Bits> a(data.a_str);
    BigInt<Bits> b(data.b_str);
    for (auto _ : state) {
        bool carry;
        auto result = a.mul(b, carry);
        benchmark::DoNotOptimize(result);
    }
}

static void BM_MpzMul(benchmark::State& state) {
    BenchData data(state.range(0));
    mpz_t a, b, result;
    mpz_init_set_str(a, data.a_str.c_str(), 10);
    mpz_init_set_str(b, data.b_str.c_str(), 10);
    mpz_init(result);
    for (auto _ : state) {
        mpz_mul(result, a, b);
        benchmark::DoNotOptimize(result);
    }
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(result);
}

BENCHMARK_MAIN();
BENCHMARK(BM_LimbInit)->Arg(128);
BENCHMARK_TEMPLATE(BM_BigIntInit, 128)->Arg(128);
BENCHMARK(BM_MpzInit)->Arg(128);

BENCHMARK(BM_LimbAdd)->Arg(128);
BENCHMARK_TEMPLATE(BM_BigIntAdd, 128)->Arg(128);
BENCHMARK(BM_MpzAdd)->Arg(128);

BENCHMARK(BM_LimbMul)->Arg(128);
BENCHMARK_TEMPLATE(BM_BigIntMul, 128)->Arg(128);
BENCHMARK(BM_MpzMul)->Arg(128);

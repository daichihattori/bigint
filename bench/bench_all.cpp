

#include <benchmark/benchmark.h>
#include "bigint.hpp"
#include <gmp.h>

using namespace bigint;

struct BenchData {
    const std::string a_str = "123456789123456789";
    const std::string b_str = "987654321987654321";
};

static void BM_AddWithResultArg(benchmark::State& state) {
    static const BenchData data;
    BigInt<256> a(data.a_str), b(data.b_str), result;
    for (auto _ : state) {
        benchmark::DoNotOptimize(a.add(b, result));
    }
}
BENCHMARK(BM_AddWithResultArg);

static void BM_AddWithPairReturn(benchmark::State& state) {
    static const BenchData data;
    BigInt<256> a(data.a_str), b(data.b_str);
    for (auto _ : state) {
        auto pair_result = a.add_ret(b);
        benchmark::DoNotOptimize(pair_result);
    }
}
BENCHMARK(BM_AddWithPairReturn);

static void BM_MpzAdd(benchmark::State& state) {
    static const BenchData data;
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
BENCHMARK(BM_MpzAdd);


static void BM_LimbAdd(benchmark::State& state) {
    static const BenchData data;
    mpz_t a_z, b_z;
    mpz_init_set_str(a_z, data.a_str.c_str(), 10);
    mpz_init_set_str(b_z, data.b_str.c_str(), 10);

    mp_size_t size = 4;
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
BENCHMARK(BM_LimbAdd);

BENCHMARK_MAIN();
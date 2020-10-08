cd "$(dirname "$0")"
.build/benchmarks/benchmarks --benchmark_out_format=csv --benchmark_out=result.csv --benchmark_repetitions=10

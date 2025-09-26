#pragma once

#include <duckdb/function/scalar_function.hpp>

namespace duckdb {
struct Simple {
	static void Register(ExtensionLoader& loader);
	static ScalarFunction GetFunction();
	static void Execute(DataChunk& args, ExpressionState& state, Vector& result);
};
}  // namespace duckdb

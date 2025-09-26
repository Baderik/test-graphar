#pragma once

#include <duckdb/function/scalar_function.hpp>
#include <duckdb/main/extension/extension_loader.hpp>

namespace duckdb {
struct MySimple {
	static void Register(ExtensionLoader& loader);
	static ScalarFunction GetFunction();
	static void Execute(DataChunk& args, ExpressionState& state, Vector& result);
};
}  // namespace duckdb

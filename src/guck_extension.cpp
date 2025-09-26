#define DUCKDB_EXTENSION_MAIN

#include "guck_extension.hpp"

#include "functions/scalar/simple.hpp"

#include <duckdb/common/exception.hpp>
#include <duckdb/common/string_util.hpp>
#include <duckdb/function/scalar_function.hpp>
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>


namespace duckdb {

inline void GuckScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &name_vector = args.data[0];
	UnaryExecutor::Execute<string_t, string_t>(name_vector, result, args.size(), [&](string_t name) {
		return StringVector::AddString(result, "Guck " + name.GetString() + " 🐥");
	});
}

static void LoadInternal(ExtensionLoader &loader) {
	// Register a scalar function
	auto guck_scalar_function = ScalarFunction("guck", {LogicalType::VARCHAR}, LogicalType::VARCHAR, GuckScalarFun);
	loader.RegisterFunction(guck_scalar_function);
	Simple::Register(loader);
}

void GuckExtension::Load(ExtensionLoader &loader) {
	LoadInternal(loader);
}
std::string GuckExtension::Name() {
	return "guck";
}

std::string GuckExtension::Version() const {
#ifdef EXT_VERSION_GUCK
	return EXT_VERSION_GUCK;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_CPP_EXTENSION_ENTRY(guck, loader) {
	duckdb::LoadInternal(loader);
}
}

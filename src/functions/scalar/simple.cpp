#include "functions/scalar/simple.hpp"

#include <duckdb/common/exception.hpp>
#include <duckdb/common/string_util.hpp>
#include <duckdb/function/scalar_function.hpp>

#include <graphar/graph_info.h>

#include <duckdb.hpp>

namespace duckdb {

void MySimple::Execute(DataChunk& args, ExpressionState& state, Vector& result) {
	const auto& context = state.GetContext();

	auto& arg_vector = args.data[0];
	const auto number = args.size();
	result.SetVectorType(VectorType::FLAT_VECTOR);
	auto result_data = FlatVector::GetData<std::string>(result);

	for (idx_t i = 0; i < number; i++) {
		const std::string file_path = arg_vector.GetValue(i).GetValue<std::string>();

		auto maybe_graph_info = graphar::GraphInfo::Load(file_path);
		if (!maybe_graph_info.has_value()) {
			throw InvalidInputException("Failed to load GraphInfo from path: " + file_path);
		}
		auto graph_info = maybe_graph_info.value();
		std::string name = graph_info->GetName();

		result_data[i] = name;
	}
}

ScalarFunction MySimple::GetFunction() {
	ScalarFunction scalar_func("simple", {LogicalType::VARCHAR}, LogicalType::VARCHAR, Execute);

	return scalar_func;
}

void MySimple::Register(ExtensionLoader& loader) {
	loader.RegisterFunction(GetFunction());
}
}  // namespace duckdb

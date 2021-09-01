#pragma once

namespace json
{
	enum class ValueType
	{
		Null,

		String,
		Int,
		Float,

		Object,
		List,
	};

	class Value
	{
	private:
		static const Value& NullValue();

	public:
		virtual ~Value() = default;

		virtual ValueType Type() const = 0;

		virtual const Value& operator[] (const std::string& name) const = 0;

		virtual Value& Insert(const std::string& name, const Value& before = NullValue()) = 0;
		virtual Value& Insert(std::string&& name, const Value& before = NullValue()) = 0;

		virtual Value& Remove(const std::string& name) = 0;
	};

	class Root
	{
	public:
		virtual Value& Tree() = 0;
		virtual const Value& Tree() const = 0;

		virtual std::tuple<bool, std::string> Load(const std::string& s) = 0;
		virtual std::tuple<bool, std::string> Load(const std::ostringstream& os) = 0;

		virtual std::tuple<bool, std::string> Save(std::string& s) = 0;
		virtual std::tuple<bool, std::string> Save(std::istringstream& is) = 0;
	};

	using RootPtr = std::unique_ptr<Root>;
	RootPtr CreateRoot();
};
#include "messages.h"

using namespace messages;

class SensorsMsgImpl : public SensorsMsg
{
public:
	void AddSensor(const std::string& className, const std::string& address, float value) override
	{
		Sensor sensor;

		sensor.className = className;
		sensor.address = address;
		sensor.type = "FLOAT";
		sensor.fValue = value;

		m_sensors.emplace_back(std::move(sensor));
	}

	void AddSensor(const std::string& className, const std::string& address, int value) override
	{
		Sensor sensor;

		sensor.className = className;
		sensor.address = address;
		sensor.type = "INT";
		sensor.iValue = value;

		m_sensors.emplace_back(std::move(sensor));
	}

	void AddSensor(const std::string& className, const std::string& address, const std::string& value) override
	{
		Sensor sensor;

		sensor.className = className;
		sensor.address = address;
		sensor.type = "STRING";
		sensor.value = value;

		m_sensors.emplace_back(std::move(sensor));
	}

	std::string MakeJson() const override
	{
		rapidjson::Document doc(rapidjson::kObjectType);

		rapidjson::Value items(rapidjson::kArrayType);

		for (const auto& sensor : m_sensors)
		{
			rapidjson::Value item(rapidjson::kObjectType);

			item.AddMember("className", rapidjson::Value(sensor.className.c_str(), sensor.className.size(), doc.GetAllocator()), doc.GetAllocator());
			item.AddMember("address", rapidjson::Value(sensor.address.c_str(), sensor.address.size(), doc.GetAllocator()), doc.GetAllocator());
			item.AddMember("valueType", rapidjson::Value(sensor.type.c_str(), sensor.type.size(), doc.GetAllocator()), doc.GetAllocator());

			if (sensor.type == "STRING")
			{
				item.AddMember("value", rapidjson::Value(sensor.value.c_str(), sensor.value.size(), doc.GetAllocator()), doc.GetAllocator());
			}
			else if (sensor.type == "FLOAT")
			{
				item.AddMember("value", sensor.fValue, doc.GetAllocator());
			}
			else if (sensor.type == "INT")
			{
				item.AddMember("value", sensor.iValue, doc.GetAllocator());
			}
			else
			{
				continue;
			}

			using clock = std::chrono::system_clock;
			const auto secondsUtc = std::chrono::duration_cast<std::chrono::seconds>(clock::now().time_since_epoch()).count();
			item.AddMember("timestamp", secondsUtc, doc.GetAllocator());

			items.PushBack(std::move(item), doc.GetAllocator());
		}

		rapidjson::Value sensors(rapidjson::kObjectType);
		sensors.AddMember("total", m_sensors.size(), doc.GetAllocator());
		sensors.AddMember("items", std::move(items), doc.GetAllocator());

		rapidjson::Value payload(rapidjson::kObjectType);
		payload.AddMember("version", rapidjson::Value("1.0", doc.GetAllocator()), doc.GetAllocator());
		payload.AddMember("sensors", std::move(sensors), doc.GetAllocator());

		rapidjson::Value request(rapidjson::kObjectType);
		request.AddMember("route", rapidjson::Value("sensors", doc.GetAllocator()), doc.GetAllocator());
		request.AddMember("command", rapidjson::Value("update", doc.GetAllocator()), doc.GetAllocator());
		request.AddMember("payload", std::move(payload), doc.GetAllocator());

		doc.AddMember("request", std::move(request), doc.GetAllocator());

		rapidjson::StringBuffer sb;
		rapidjson::Writer writer(sb);

		if (!doc.Accept(writer))
		{
			return "";
		}

		return sb.GetString();
	}

private:
	struct Sensor
	{
		std::string className;
		std::string address;
		std::string type;

		float fValue = 0.f;
		int iValue = 0;
		std::string value;
	};

	std::list<Sensor> m_sensors;
};

SensorsMsg::Ptr SensorsMsg::Create()
{
	return std::make_unique<SensorsMsgImpl>();
}

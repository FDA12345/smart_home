#pragma once

class Ver1 : public db::versioning::DbVersion
{
public:
	uint64_t Version() const override { return 1; }
	std::string Description() const override { return ""; }

	bool Upgrade(const db::Ptr& db) override
	{
		return false;
	}
};

m_verDb->AddVersion(std::unique_ptr<Ver1>());

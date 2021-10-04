#pragma once

class Ver1 : public db::versioning::DbVersion
{
public:
	uint64_t Version() const override { return 1; }
	std::string Description() const override { return "Base tags db"; }

	bool Upgrade(const db::Ptr& db) override
	{
		if (!db->Query("CREATE TABLE versions ON (id int, when_ DATETIME, note VARCHAR(255))"))
		{
			return false;
		}

		return false;
	}
};

m_verDb->AddVersion(std::unique_ptr<Ver1>());

#pragma once

class Ver1 : public db::versioning::DbVersion
{
public:
	uint64_t Version() const override { return 1; }
	std::string Description() const override { return "Base tags db"; }

	bool Upgrade(const db::Ptr& db) override
	{
		if (!db->Query("CREATE TABLE versions (id int PRIMARY KEY, when_ DATETIME, note VARCHAR(255))"))
		{
			return false;
		}

		return true;
	}

	bool CanDowngrade(const db::Ptr& db) const override { return true; }
	bool Downgrade(const db::Ptr& db) override
	{
		db->Query("DROP TABLE versions");
		return true;
	}
};

m_verDb->AddVersion(std::make_unique<Ver1>());

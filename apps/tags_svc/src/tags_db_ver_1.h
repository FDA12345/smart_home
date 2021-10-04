#pragma once

class Ver1 : public db::versioning::DbVersion
{
public:
	uint64_t Version() const override { return 1; }
	std::string Description() const override { return "Base tags db"; }

	bool Upgrade(const db::Ptr& db) override
	{
		if (!db->Query("CREATE TABLE versions (id int PRIMARY KEY, create_date DATETIME, note VARCHAR(255)) ENGINE=MyISAM"))
		{
			return false;
		}



		if (!db->Query("CREATE TABLE tags (id int PRIMARY KEY, parent_id int, name VARCHAR(50) NOT NULL, tag_type VARCHAR(20), digits int, note VARCHAR(255), address VARCHAR(100), update_date DATETIME) ENGINE=MyISAM"))
		{
			return false;
		}

		if (!db->Query("CREATE UNIQUE INDEX idx_tags__name ON tags (name)"))
		{
			return false;
		}

		if (!db->Query("CREATE UNIQUE INDEX idx_tags__address ON tags (address)"))
		{
			return false;
		}



		if (!db->Query("CREATE TABLE tag_values (tag_id int, create_date DATETIME, value FLOAT) ENGINE=MyISAM"))
		{
			return false;
		}

		if (!db->Query("CREATE INDEX idx_tag_values__create_date__id ON tag_values (create_date, tag_id)"))
		{
			return false;
		}

		return true;
	}

	bool CanDowngrade(const db::Ptr& db) const override
	{
		return true;
	}

	bool Downgrade(const db::Ptr& db) override
	{
		db->Query("DROP TABLE tag_values");
		db->Query("DROP TABLE tags");
		db->Query("DROP TABLE versions");
		return true;
	}
};

m_verDb->AddVersion(std::make_unique<Ver1>());

#pragma once

class Ver1 : public db::versioning::DbVersion
{
public:
	uint64_t Version() const override { return 1; }
	std::string Description() const override { return "Base tags db"; }

	bool Upgrade(const db::Ptr& db) override
	{
		if (!db->Query("CREATE TABLE versions (id int PRIMARY KEY, create_date DATETIME, note VARCHAR(1000)) ENGINE=MyISAM"))
		{
			return false;
		}



		if (!db->Query("CREATE TABLE source_types (id int PRIMARY KEY, name VARCHAR(50) NOT NULL) ENGINE=MyISAM"))
		{
			return false;
		}

		if (!db->Query("INSERT INTO source_types VALUES (1, 'sensor')"))
		{
			return false;
		}

		if (!db->Query("INSERT INTO source_types VALUES (2, 'tag')"))
		{
			return false;
		}



		if (!db->Query("CREATE TABLE value_types (id int PRIMARY KEY, name VARCHAR(50) NOT NULL) ENGINE=MyISAM"))
		{
			return false;
		}

		if (!db->Query("INSERT INTO value_types VALUES (1, 'float')"))
		{
			return false;
		}

		if (!db->Query("INSERT INTO value_types VALUES (2, 'int')"))
		{
			return false;
		}

		if (!db->Query("INSERT INTO value_types VALUES (3, 'string')"))
		{
			return false;
		}



		if (!db->Query("CREATE TABLE sensors (id int PRIMARY KEY, device_id int, name VARCHAR(50) UNIQUE NOT NULL, note VARCHAR(1000), address VARCHAR(200) UNIQUE, update_date DATETIME, value VARCHAR(255)) ENGINE=MyISAM"))
		{
			return false;
		}


		if (!db->Query("CREATE TABLE tags (id int PRIMARY KEY, name VARCHAR(50) UNIQUE NOT NULL, note VARCHAR(1000), update_date DATETIME, value_type_id int NOT NULL) ENGINE=MyISAM"))
		{
			return false;
		}



		if (!db->Query("CREATE TABLE agg_types (id int PRIMARY KEY, name VARCHAR(255), note VARCHAR(1000)) ENGINE=MyISAM"))
		{
			return false;
		}



		if (!db->Query("CREATE TABLE history (id int PRIMARY KEY, tag_id int NOT NULL, seconds_period int NOT NULL, create_date DATETIME, agg_type int not null, agg_params VARCHAR(10000), INDEX idx_history__tag_id(tag_id)) ENGINE=MyISAM"))
		{
			return false;
		}



		if (!db->Query("CREATE TABLE history_floats (history_id int NOT NULL, create_date DATETIME NOT NULL, value FLOAT NOT NULL, INDEX idx_history_floats__create_date(create_date)) ENGINE=MyISAM"))
		{
			return false;
		}

		if (!db->Query("CREATE TABLE history_integers (history_id int NOT NULL, create_date DATETIME NOT NULL, value int NOT NULL, INDEX idx_history_integers__create_date(create_date)) ENGINE=MyISAM"))
		{
			return false;
		}

		if (!db->Query("CREATE TABLE history_strings (history_id int NOT NULL, create_date DATETIME NOT NULL, value VARCHAR(1000), INDEX idx_history_strings__create_date(create_date)) ENGINE=MyISAM"))
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
		db->Query("DROP TABLE history_strings");
		db->Query("DROP TABLE history_integers");
		db->Query("DROP TABLE history_floats");
		db->Query("DROP TABLE history");
		db->Query("DROP TABLE agg_types");
		db->Query("DROP TABLE tags");
		db->Query("DROP TABLE sensors");
		db->Query("DROP TABLE value_types");
		db->Query("DROP TABLE source_types");
		db->Query("DROP TABLE versions");
		return true;
	}
};

m_verDb->AddVersion(std::make_unique<Ver1>());

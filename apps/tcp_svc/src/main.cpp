#include "stdafx.h"

namespace asio = boost::asio;

using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

namespace std
{
	using namespace placeholders;
}

class TcpClient : public std::enable_shared_from_this<TcpClient>
{
public:
	TcpClient(asio::io_service& io, tcp::socket&& socket)
		: m_io(io)
		, m_strand(m_io)
		, m_socket(std::move(socket))
		, m_timeout(m_io)
	{
	}

	void Run()
	{
		RestartTimer();
	}

	void Send(const std::shared_ptr<std::string>& sharedMsg)
	{
		asio::async_write
		(
			m_socket, asio::const_buffer(sharedMsg->data(), sharedMsg->length()),
			m_strand.wrap(std::bind(&TcpClient::OnWrite, shared_from_this(), std::_1, sharedMsg))
		);

		RestartTimer();
	}

private:
	void RestartTimer()
	{
		m_timeout.expires_from_now(boost::posix_time::seconds(30));

		m_timeout.async_wait
		(
			m_strand.wrap(std::bind(&TcpClient::OnTimeout, shared_from_this(), std::_1))
		);
	}

	void OnTimeout(const error_code& ec)
	{
		if (ec)
		{
			return;
		}

		m_socket.close();
	}

	void OnWrite(const error_code& ec, const std::shared_ptr<std::string>& sharedMsg)
	{
	}

private:
	asio::io_service& m_io;
	asio::io_service::strand m_strand;
	tcp::socket m_socket;
	asio::deadline_timer m_timeout;
};



class DataFlow
{
public:
	void AddClient(const std::shared_ptr<TcpClient>& client)
	{
		std::lock_guard lock(m_mx);
		m_clients[client.get()] = client;
	}

	void SendAll(const std::string& msg)
	{
		auto sharedMsg(std::make_shared<std::string>(msg));

		std::lock_guard lock(m_mx);

		for (auto it = m_clients.begin(); it != m_clients.end(); )
		{
			auto client = it->second.lock();
			if (!client)
			{
				it = m_clients.erase(it);
				continue;
			}

			client->Send(sharedMsg);

			++it;
		}
	}

private:
	std::mutex m_mx;
	std::map<const TcpClient*, std::weak_ptr<TcpClient>> m_clients;
};



class TcpServer
{
public:
	TcpServer()
		: m_acceptor(m_io)
		, m_ioThreads(ioThreadsTotal)
	{
	}

	void SendAll(const std::string& msg)
	{
		m_dataFlow.SendAll(msg);
	}

	bool Start()
	{
		error_code ec;

		const auto& address = asio::ip::make_address("0.0.0.0", ec);
		if (ec)
		{
			return false;
		}

		tcp::endpoint ep(address, 8888);

		m_acceptor.open(ep.protocol(), ec);
		if (ec)
		{
			return false;
		}

		m_acceptor.bind(ep, ec);
		if (ec)
		{
			return false;
		}

		m_acceptor.listen(tcp::acceptor::max_listen_connections, ec);
		if (ec)
		{
			return false;
		}

		StartAccept();

		for (auto & th : m_ioThreads)
		{
			th = std::thread{
				[this]()
				{
					m_io.run();
				}
			};
		}

		return true;
	}

	void Stop()
	{
		if (!m_io.stopped())
		{
			m_io.stop();

			for (auto& th : m_ioThreads)
			{
				th.join();
			}

			m_io.reset();
		}
	}

private:
	void StartAccept()
	{
		m_acceptor.async_accept(std::bind(&TcpServer::OnAccept, this, std::_1, std::_2));
	}

	void OnAccept(const error_code& ec, tcp::socket&& peer)
	{
		if (ec)
		{
			return;
		}

		auto client = std::make_shared<TcpClient>(m_io, std::move(peer));

		m_dataFlow.AddClient(client);
		client->Run();

		StartAccept();
	}

private:
	asio::io_service m_io;
	tcp::acceptor m_acceptor;

	const size_t ioThreadsTotal = 1;
	std::vector<std::thread> m_ioThreads;

	DataFlow m_dataFlow;
};

int main()
{
	std::unique_ptr<TcpServer> srv(std::make_unique<TcpServer>());

	if (srv->Start())
	{
		for (size_t i = 0; i < 10; ++i)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			srv->SendAll("kaka-" + std::to_string(i));
		}
	}

	srv->Stop();
	srv.reset();
}

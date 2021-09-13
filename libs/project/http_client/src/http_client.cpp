#include "stdafx.h"
#include "http_client.h"
#include "logger.h"
#include <curl/curl.h>

#include <mutex>

using namespace http_client;

class CurlGlobalLife
{
public:
	void Init()
	{
		std::lock_guard lock(m_mx);

		if (!m_initialized)
		{
			m_initialized = true;
			curl_global_init(CURL_GLOBAL_ALL);
		}
	}

	void Destroy()
	{
		std::lock_guard lock(m_mx);

		if (m_initialized)
		{
			m_initialized = false;
			curl_global_cleanup();
		}
	}

private:
	std::mutex m_mx;
	bool m_initialized = false;
};
CurlGlobalLife g_curlGlobalLife;


class CurlList
{
public:
	~CurlList()
	{
		if (m_lst)
		{
			curl_slist_free_all(m_lst);
			m_lst = nullptr;
		}
	}

	void Append(const std::string& value)
	{
		m_lst = curl_slist_append(m_lst, value.c_str());
	}

	curl_slist* List()
	{
		return m_lst;
	}

private:
	curl_slist* m_lst = nullptr;
};


class CurlHandle
{
public:
	CurlHandle()
	{
		g_curlGlobalLife.Init();
		m_curl = curl_easy_init();
	}

	~CurlHandle()
	{
		if (m_curl)
		{
			curl_easy_cleanup(m_curl);
			m_curl = nullptr;
		}
	}

	CURL* Handle() { return m_curl; }
	const CURL* Handle() const { return m_curl; }

private:
	CURL* m_curl = nullptr;
};


//поддерживает асинхронную работу через калбек asyncFn
class HttpTask : public std::enable_shared_from_this<HttpTask>
{
public:
	using AsyncFn = std::function<void(const std::shared_ptr<HttpTask> httpTask)>;

public:
	HttpTask(const Params& params, const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body, AsyncFn asyncFn)
		: m_log(logger::Create())
		, m_params(params)
		, m_headers(headers)
		, m_resource(resource)
		, m_resultFn(resultFn)
		, m_body(body)
		, m_curl(std::make_unique<CurlHandle>())
		, m_headerList(std::make_unique<CurlList>())
		, m_asyncFn(asyncFn)
	{
	}

	void Get()
	{
		StartMethod("GET");
	}

	void Post()
	{
		StartMethod("POST");
	}

	void Put()
	{
		StartPutMethod();
	}

	void Delete()
	{
		StartMethod("DELETE");
	}

	void Perform()
	{
		m_result = curl_easy_perform(m_curl->Handle()) == CURLE_OK;
	}

	void PopulateResult()
	{
		m_resultFn(m_result, m_inBuffer.str());
	}

private:
	void StartPutMethod()
	{
		if (!CreateRequest())
		{
			m_resultFn(false, "");
			return;
		}

		curl_easy_setopt(m_curl->Handle(), CURLOPT_UPLOAD, 1L);

		curl_easy_setopt(m_curl->Handle(), CURLOPT_READFUNCTION, &HttpTask::__ReadCallback);
		curl_easy_setopt(m_curl->Handle(), CURLOPT_READDATA, this);
		curl_easy_setopt(m_curl->Handle(), CURLOPT_INFILESIZE_LARGE, (curl_off_t)m_body.size());

		m_outBuffer = std::istringstream(m_body);

		m_asyncFn(shared_from_this());
	}

	void StartMethod(const std::string& method)
	{
		if (!CreateRequest())
		{
			m_resultFn(false, "");
			return;
		}

		if (method != "GET")
		{
			if (method == "POST")
			{
				curl_easy_setopt(m_curl->Handle(), CURLOPT_POST, 1L);
			}
			else
			{
				curl_easy_setopt(m_curl->Handle(), CURLOPT_CUSTOMREQUEST, method.c_str());
			}
		}

		m_asyncFn(shared_from_this());
	}

	bool CreateRequest()
	{
		if (!m_curl->Handle())
		{
			return false;
		}

		if (m_log->Level() == logger::LogLevel::Trace)
		{
			curl_easy_setopt(m_curl->Handle(), CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(m_curl->Handle(), CURLOPT_DEBUGFUNCTION, &HttpTask::__DebugCallback);
			curl_easy_setopt(m_curl->Handle(), CURLOPT_DEBUGDATA, this);
		}

		curl_easy_setopt(m_curl->Handle(), CURLOPT_TIMEOUT, m_params.timeout);

		const std::string url = m_params.scheme + "://" + m_params.host + ":" + std::to_string(m_params.port) + m_resource;
		curl_easy_setopt(m_curl->Handle(), CURLOPT_URL, url.c_str());

		curl_easy_setopt(m_curl->Handle(), CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(m_curl->Handle(), CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(m_curl->Handle(), CURLOPT_TCP_KEEPALIVE, 1L);

		switch (m_params.auth)
		{
		case AuthMode::Basic:  curl_easy_setopt(m_curl->Handle(), CURLOPT_HTTPAUTH, static_cast<long>(CURLAUTH_BASIC)); break;
		case AuthMode::Digest: curl_easy_setopt(m_curl->Handle(), CURLOPT_HTTPAUTH, static_cast<long>(CURLAUTH_DIGEST)); break;
		}

		if (m_params.auth != AuthMode::None)
		{
			curl_easy_setopt(m_curl->Handle(), CURLOPT_USERNAME, m_params.login.c_str());
			curl_easy_setopt(m_curl->Handle(), CURLOPT_PASSWORD, m_params.password.c_str());
			curl_easy_setopt(m_curl->Handle(), CURLOPT_HTTPAUTH, static_cast<long>(CURLAUTH_DIGEST));
		}

		curl_easy_setopt(m_curl->Handle(), CURLOPT_WRITEFUNCTION, &HttpTask::__WriteCallback);
		curl_easy_setopt(m_curl->Handle(), CURLOPT_WRITEDATA, this);

		if (!m_headers.empty())
		{
			for (const auto& hdr : m_headers)
			{
				m_headerList->Append(hdr.first + ": " + hdr.second);
			}

			curl_easy_setopt(m_curl->Handle(), CURLOPT_HTTPHEADER, m_headerList->List());
		}

		if (!m_body.empty())
		{
			curl_easy_setopt(m_curl->Handle(), CURLOPT_POSTFIELDS, m_body.c_str());
		}

		//curl_easy_setopt(m_curl->Handle(), CURLOPT_SSL_OPTIONS, CURLSSLOPT_AUTO_CLIENT_CERT);// | CURLSSLOPT_NO_REVOKE);

		return true;
	}

	static size_t __ReadCallback(char *buffer, size_t size, size_t nitems, void *userdata)
	{
		auto httpTask = reinterpret_cast<HttpTask*>(userdata);
		return httpTask->ReadCallback(buffer, size, nitems);
	}
	size_t ReadCallback(char *buffer, size_t size, size_t nitems)
	{
		const auto maxSize = size * nitems;
		const auto ret = m_outBuffer.readsome(buffer, maxSize);
		return size_t(ret);
	}

	static size_t __WriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
	{
		auto httpTask = reinterpret_cast<HttpTask*>(userdata);
		return httpTask->WriteCallback(ptr, size, nmemb);
	}
	size_t WriteCallback(char *ptr, size_t size, size_t nmemb)
	{
		m_inBuffer << std::string(ptr, nmemb);
		return nmemb;
	}

	static int __DebugCallback(CURL *handle, curl_infotype type, char *data, size_t size, void *userdata)
	{
		auto httpTask = reinterpret_cast<HttpTask*>(userdata);
		httpTask->DebugCallback(handle, type, data, size);
		return 0;
	}
	void DebugCallback(CURL *handle, curl_infotype type, char *data, size_t size)
	{
		std::string title;
		switch (type)
		{
		case CURLINFO_HEADER_IN:
			m_debugBuffer << std::string(data, size);
			return;

		case CURLINFO_HEADER_OUT:
			m_debugBuffer << std::string(data, size);
			return;

		case CURLINFO_DATA_IN:
			title = "Received ...\n";
			if (!m_debugBuffer.eof())
			{
				title += m_debugBuffer.str();
				m_debugBuffer = std::ostringstream();
			}
			break;

		case CURLINFO_DATA_OUT:
			title = "Sent ...\n";
			if (!m_debugBuffer.eof())
			{
				title += m_debugBuffer.str();
				m_debugBuffer = std::ostringstream();
			}
			break;

		case CURLINFO_SSL_DATA_IN:
			title = "Received SSL data ...";
			break;

		case CURLINFO_SSL_DATA_OUT:
			title = "Sent SSL data ...";
			break;

		case CURLINFO_TEXT:
			logDEBUG(__FUNCTION__, "curl text: "<< std::string(data, size));
			return;

		default: return;
		}

		logDEBUG(__FUNCTION__, title << std::endl << std::string(data, size) << std::endl);
	}

private:
	const logger::Ptr m_log;
	const Params m_params;
	const Headers m_headers;
	const std::string m_resource;
	const ResultFn m_resultFn;
	const std::string m_body;
	const std::unique_ptr<CurlHandle> m_curl;
	const std::unique_ptr<CurlList> m_headerList;
	const AsyncFn m_asyncFn;

	std::ostringstream m_inBuffer;
	std::ostringstream m_debugBuffer;
	std::istringstream m_outBuffer;
	bool m_result = false;
};


class HttpClientImpl : public HttpClient
{
public:
	HttpClientImpl(const Params& params)
		: m_params(params)
	{
	}

	void Get(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body) override
	{
		CreateTask(headers, resource, resultFn, body)->Get();
	}

	void Post(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body) override
	{
		CreateTask(headers, resource, resultFn, body)->Post();
	}

	void Put(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body) override
	{
		CreateTask(headers, resource, resultFn, body)->Put();
	}

	void Delete(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body) override
	{
		CreateTask(headers, resource, resultFn, body)->Delete();
	}

private:
	std::shared_ptr<HttpTask> CreateTask(const Headers& headers, const std::string& resource, ResultFn resultFn, const std::string& body)
	{
		return std::make_shared<HttpTask>
			(
				m_params,
				headers,
				resource,
				resultFn,
				body,
				std::bind(&HttpClientImpl::RunAsync, this, std::placeholders::_1)
			);
	}

	void RunAsync(const std::shared_ptr<HttpTask> httpTask)
	{
		/*
		m_appartInner->GetService().post([curlTask, appartOutter = m_appartOutter]()
		{
			curlTask->Perform();

			appartOutter->GetStrand().post([curlTask]()
			{
				curlTask->PopulateResult();
			});
		});
		*/

		httpTask->Perform();
		httpTask->PopulateResult();
	}

private:
	const logger::Ptr m_log = logger::Create();
	const Params m_params;
};


Ptr http_client::Create(const std::string& host, uint16_t port)
{
	Params params;

	params.host = host;
	params.port = port;

	return std::make_unique<HttpClientImpl>(params);
}

Ptr http_client::Create(AuthMode auth, const std::string& login, const std::string& password, const std::string& host, uint16_t port)
{
	Params params;

	params.auth = auth;
	params.login = login;
	params.password = password;

	params.host = host;
	params.port = port;

	return std::make_unique<HttpClientImpl>(params);
}

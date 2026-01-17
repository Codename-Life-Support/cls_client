#pragma once







#pragma once
#include <string>
#include <thread>
#include <atomic>

class HttpRequest
{
public:
    HttpRequest(const std::string& url, const std::string& content)
        : m_url(url), m_content(content), m_done(false)
    {
        m_thread = std::thread([this]() { this->Run(); });
        m_thread.detach();
    }

    ~HttpRequest() = default;

    HttpRequest(HttpRequest&&) noexcept = default;
    HttpRequest& operator=(HttpRequest&&) noexcept = default;

    HttpRequest(const HttpRequest&) = delete;
    HttpRequest& operator=(const HttpRequest&) = delete;

    bool HasResult() const {
        return m_done.load();
    }

    const std::string& GetResult() const {
        return m_response;
    }
    const long GetResultCode() const {
        return m_http_code;
    }
    const char* GetResultError() const {
        return error_message;
    }

private:
    std::string m_url;
    std::string m_content;
    std::string m_response;
    CURLcode m_response_code;
    const char* error_message;
    long m_http_code;
    std::atomic<bool> m_done;
    std::thread m_thread;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        size_t total = size * nmemb;
        std::string* resp = static_cast<std::string*>(userp);
        resp->append((char*)contents, total);
        return total;
    }

    void Run()
    {
        CURL* curl = curl_easy_init();
        if (curl)
        {
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            // Set curl options
            curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_content.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, m_content.size());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_response);

            // Perform the request
            m_response_code = curl_easy_perform(curl);

            if (m_response_code != CURLE_OK) {
                m_http_code = 0;
                error_message = curl_easy_strerror(m_response_code);
            }
            else {
                error_message = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &m_http_code);
            }

            // Cleanup
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        else {
            std::cerr << "Failed to initialize CURL.\n";
        }

        

        m_done = true;
    }
};

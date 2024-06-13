#include "wizchip/http/server.h"
#include "wizchip/http/client.h"
#include "wizchip/dns.h"
#include "main.hpp"
#include "etl/json.h"
#include "etl/keywords.h"

using namespace Project;


[[export, init]]
void test_http_server() {
    static auto server = wizchip::http::Server({
        .port=5000,
    });

    // global headers
    server.global_headers["Server"] = []() -> std::string {
        return "stm32-wizchip/" WIZCHIP_VERSION;
    };

    // print hello
    server.Get("/hello", "text/plain", [](const wizchip::http::Request&, wizchip::http::Response& response) {
        response.body = "Hello world from stm32-wizchip/" WIZCHIP_VERSION "\n";
    });


    // get heap info
    server.Get("/heap", "application/json", [] (const wizchip::http::Request&, wizchip::http::Response& response) {
        response.body.reserve(128);
        response.body = "{";
        response.body +=    "\"freeSize\":" + std::to_string(etl::heap::freeSize) + ",";
        response.body +=    "\"totalSize\":" + std::to_string(etl::heap::totalSize) + ",";
        response.body +=    "\"minimumEverFreeSize\":" + std::to_string(etl::heap::minimumEverFreeSize);
        response.body += "}";
    });

    // display all routes
    server.Get("/routes", "application/json", [] (const wizchip::http::Request&, wizchip::http::Response& response) {
        response.body.reserve(256);
        response.body = "[";
        for (auto &router in server.routers) {
            auto methods = std::string();
            for (auto method in router.methods) {
                methods += method;
                methods += " ";
            }
            methods.pop_back();

            response.body += "{";
            response.body +=    "\"methods\":\"" + methods + "\",";
            response.body +=    "\"path\":\"" + router.path + "\"";
            response.body += "},";
        }
        response.body.back() = ']';
    });

    // extract queries
    server.Get("/queries", "application/json", [] (const wizchip::http::Request& request, wizchip::http::Response& response) {
        response.body.reserve(256);
        response.body = "{\"path\":\"" + request.path.full_path + "\"}";
        for (auto &[key, value] in request.path.queries) {
            response.body.back() = ',';
            response.body += "\"" + key +"\":\"" + value + "\"}";
        }
    });

    // get the ip and port from host
    server.Get("/client", "application/json", [] (const wizchip::http::Request& request, wizchip::http::Response& response) {
        if (not request.path.queries.has("host")) {
            response.status = wizchip::http::StatusBadRequest;
            response.body = "Host is not specified";
            response.headers["Content-Type"] = "text/plain"; 
            return;
        }

        auto cli = wizchip::http::Client(request.path.queries["host"]);

        response.body.reserve(128);
        response.body = "{"
            "\"host\":\"" + std::to_string(cli.host[0]) + "." + std::to_string(cli.host[1]) + "." + std::to_string(cli.host[2]) + "." + std::to_string(cli.host[3]) + "\","
            "\"port\":" + std::to_string(cli.port) + 
        "}";
    });

    // post json example
    server.Post("/dns", "application/json", [] (const wizchip::http::Request& request, wizchip::http::Response& response) {
        auto json = etl::Json::parse(etl::string_view(request.body.c_str(), request.body.size()));
        auto json_err = json.error_message();

        if (json_err) {
            response.status = wizchip::http::StatusBadRequest;
            response.body = std::string() + "{\"errorJson\":\"" + json_err.data() + "\",\"body\":\"" + request.body + "\"}";
            return;
        }

        auto domain = json["domain"].to_string();
        if (not domain) {
            response.status = 400;
            response.body = "{\"error\":\"Request json doesn't have 'domain' key\"}";
            return;
        }

        auto ip = wizchip::dns::get_ip(std::string(domain.begin(), domain.end())).await().unwrap();
        response.body = "{"
            "\"ip\":\"" + std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." + std::to_string(ip[2]) + "." + std::to_string(ip[3]) + "\""
        "}";
    });

    server.start();
}

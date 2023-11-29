#include "httplib.h"
#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/prctl.h>

#include <json/json.h>

pid_t getCurrentThreadId()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}


int main(void)
{
    using namespace httplib;

    Server svr;

    svr.Get("/h1", [](const Request& req, Response& res) {
        Json::Value root;
        root["status"] = "OK";
        root["code"] = 200;
        Json::Value data;
        data["message"] = "Hello, World!";
        root["data"] = data;
        root["req_str"] = req.get_header_value("str1");
        root["req_int"] = Json::Value::UInt64(req.get_header_value_u64("int1"));

        Json::StreamWriterBuilder builder;
        std::string jsonString = Json::writeString(builder, root);
        std::cout << "h1 tid: " << getCurrentThreadId() << " jsonstring: " << jsonString << std::endl;
        res.set_content(jsonString, "text/plain");
    });

    svr.Get("/h2", [](const Request& req, Response& res) {
        std::cout << "h2 tid: " << getCurrentThreadId() << std::endl;
        res.set_content("Hello World2! " + std::to_string(getCurrentThreadId()), "text/plain");
    });

    svr.Get("/h3", [](const Request& req, Response& res) {
        std::cout << "h3 tid: " << getCurrentThreadId() << std::endl;
        res.set_content("Hello World! " + std::to_string(getCurrentThreadId()), "text/plain");
    });

    svr.Get("/stop", [&](const Request& req, Response& res) {
        svr.stop();
    });

    std::cout << "http server listen on 1234" << std::endl;

    svr.listen("0.0.0.0", 1234);
}
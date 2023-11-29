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

    svr.Post("/h1", [](const Request& req, Response& res) {

        std::cout << "http body: " << req.body << std::endl;

        Json::Value root;
        Json::Reader reader;
        if (reader.parse(req.body, root)) {
            Json::Value respRoot;
            respRoot["status"] = "OK";
            respRoot["code"] = 200;
            Json::Value data;
            data["message"] = "Hello, World!";
            respRoot["data"] = data;
            respRoot["req_header_str"] = req.get_header_value("req_str");
            respRoot["req_header_int64"] = Json::Value::UInt64(req.get_header_value_u64("req_int64"));
            respRoot["req_body_modelId"] = root["modelId"];
            respRoot["req_body_prompts_value"] = root["prompts"][0]["value"];
            Json::StreamWriterBuilder builder;
            std::string jsonString = Json::writeString(builder, respRoot);
            std::cout << "h1 tid: " << getCurrentThreadId() << " jsonstring: " << jsonString << std::endl;
            res.set_content(jsonString, "text/json");
        } else {
            Json::Value respRoot;
            respRoot["status"] = "FAIL";
            respRoot["code"] = 502;
            Json::StreamWriterBuilder builder;
            std::string jsonString = Json::writeString(builder, respRoot);
            res.set_content(jsonString, "text/json");
        }
    });

    svr.Post("/stop", [&](const Request& req, Response& res) {
        svr.stop();
    });

    std::cout << "http server listen on 1234" << std::endl;

    svr.listen("0.0.0.0", 1234);
}
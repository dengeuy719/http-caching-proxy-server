#include "../src/HTTPResponse.h"
#include "boost/beast.hpp"
#include "../src/TimeParser.h"
#include <iostream>

int main() {
    using namespace boost::beast::http;

    parseTime("Mon, 07 Nov 1994 08:49:37 GMT", "%a, %d %b %Y %H:%M:%S GMT");
    //Create an instance of the response class
    response<dynamic_body> res;

    // Set the status code
    res.result(status::ok);

    // Set the response body
    res.prepare_payload();

    // Set the content length
    res.set(field::date, "Sun, 06 Nov 1994 08:49:37 EST");
    res.set(field::cache_control, "sdfsdf, max-age=86410,sdfsdf");
    HTTPResponse response(res);
    std::cout << response.getExpireTime() - parseTime("Mon, 07 Nov 1994 08:49:37 EST", "%a, %d %b %Y %H:%M:%S %Z") << std::endl;
}
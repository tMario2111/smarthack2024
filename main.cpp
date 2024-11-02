#include <iostream>
#include <string>

#include <cpr/cpr.h>

const std::string API_KEY = "7bcd6334-bc2e-4cbf-b9d4-61cb9e868869";

int main()
{
    cpr::Response r = cpr::Get(cpr::Url("192.168.123.221:8090/api-docs"));
    std::cout << r.status_code << '\n';
    std::cout << r.text << '\n';

    return 0;
}

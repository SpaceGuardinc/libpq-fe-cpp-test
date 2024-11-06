// main.cpp
#include "ipgqsldatabase.hpp"
#include <iostream>
#include <string>

int main() {
    IPGQSLDatabase db("config.cfg");
    db.connect();
    
    db.executeQuery("SET search_path = testlibpq3");
    db.executeQuery("SELECT * FROM public.users;");
    db.showResult();

    return 0;
}


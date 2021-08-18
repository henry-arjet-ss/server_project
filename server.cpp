#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>
#include <cpprest/details/http_helpers.h>

using std::cout;
using std::endl;
using std::string;

using web::http::status_codes;
using web::http::http_request;

string distFolder = "../dist/"; //The folder where we keep our client-side resources

//contentMap relates file extentions to mime types 
std::unordered_map<utility::string_t, utility::string_t> contentMap;

void createContentMap() {
    contentMap[U(".html")] = U("text/html");
    contentMap[U(".js")] = U("application/javascript");
    contentMap[U(".css")] = U("text/css");
    contentMap[U(".png")] == U("image/png");
}

void handle_error(pplx::task<void>& t) { //cpprestsdk uses this. I don't
    try {
        t.get();
    }
    catch (...) {}
}

void handleGet(http_request const& req){ //This is a barebones server, so this is all I'm including
   
   auto path = req.relative_uri().to_string();
   std::cout << "GET called at " << path << endl;

   if (req.relative_uri().path().find_last_of(L'.') != std::string::npos) {
       //stolen from handleGetFile function
        cout << "searching for file" << endl;
        auto path = distFolder + req.relative_uri().path();
        auto extension = path.substr(path.find_last_of('.'));
        auto content_type_it = contentMap.find(extension);
        if (content_type_it == contentMap.end()) { //if the file isn't found
            req.reply(404U);
            return;
        }
        string content_type = content_type_it->second;


        concurrency::streams::istream is = concurrency::streams::fstream::open_istream(path, std::ios::in).get();
        req.reply(200U, is, content_type).then([](pplx::task<void> t) { handle_error(t); }).wait();
        is.close();
   }
    else{
        //stolen from the handleGetRoot function
        try {

                concurrency::streams::istream is = concurrency::streams::fstream::open_istream(distFolder + "index.html", std::ios::in).get();
                req.reply(200U, is, "text/html").then([](pplx::task<void> t) { handle_error(t); }).wait();
                is.close();
            }

            catch (web::http::http_exception& ex) {
                cout << "HTTP EXCEPTION: " << ex.what() << endl;
                throw;
            }
            catch (std::exception& ex) {
                cout << "STD EXCEPTION: " << ex.what() << endl;
                throw;
            }
            catch (const char* ex) {
                cout << "EXCEPTION: " << ex << endl;
                throw;
            }
    }
   
}

int main() {
    
    createContentMap();

    const std::string base_url = "http://0.0.0.0:8086/";
    web::http::experimental::listener::http_listener listener(base_url);

    listener.support(web::http::methods::GET, handleGet);

    try {
        listener.open().wait();
    }
    catch (web::http::http_exception& ex) {
        cout << "HTTP EXCEPTION: " << ex.what() << endl;
        throw;
    }
    catch (std::exception& ex) {
        cout << "STD EXCEPTION: " << ex.what() << endl;
        throw;
    }
    catch (const char* ex) {
        cout << "EXCEPTION: " << ex << endl;
        throw;
    }

    std::cout << std::string(U("Listining for requests at: ")) << base_url << std::endl;

    std::this_thread::sleep_for(std::chrono::hours(10000));

    listener.close().wait();
}
#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
// устойчивый таймер
boost::asio::steady_timer make_timer(boost::asio::io_context& io_context){
    return boost::asio::steady_timer{
        io_context,std::chrono::steady_clock::now() + std::chrono::seconds{ 3 }
    };
}

void asiotimers(){
    boost::asio::io_context io_context;
    auto timer1 = make_timer(io_context);
    std::cout << "entering steady_timer::wait\n";
    timer1.wait();
    std::cout << "exited steady_timer::wait\n";
    auto timer2 = make_timer(io_context);
    std::cout << "entering steady_timer::async_wait\n";
    timer2.async_wait([](const boost::system::error_code& error){
        if(!error) std::cout << "<<callback function>>\n";
    });
    std::cout << "exited steady_timer::async_wait\n";
    std::cout << "entering io_context::run\n";
    io_context.run();
    std::cout << "exited io_context::run\n";
}

void asioresolver(){
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver { io_context };
    boost::system::error_code ec;
    for(auto&& result : resolver.resolve("nikodimus.ru", "http", ec)){
        std::cout << result.service_name() << " "
        << result.host_name() << " "
        << result.endpoint() << std::endl;
    }
    if(ec) std::cout << "Error code: " << ec << std::endl;
}

void asioasyncresolver(){
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver { io_context };
    //boost::system::error_code ec;
    std::cout << "Async resolver" << std::endl;
    resolver.async_resolve("nikodimus.ru", "http", [](boost::system::error_code ec, const auto& results){
        if(ec){
            std::cerr << "Error:" << ec << std::endl;
            return;
        }
        for(auto&& result : results){
            std::cout << result.service_name() << " "
        << result.host_name() << " "
        << result.endpoint() << std::endl;
        }
    });
    io_context.run();
}

void asioconnect(){
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver { io_context };
    boost::asio::ip::tcp::socket socket{ io_context };
    try{
        auto endpoints = resolver.resolve("www.nostarch.com", "http");
        const auto connected_endpoint = boost::asio::connect(socket, endpoints);
        std::cout << connected_endpoint << std::endl;
    } catch(boost::system::system_error& se){
        std::cerr << "Error: " << se.what() << std::endl;
    }
}

void asioasyncconnect(){
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver { io_context };
    boost::asio::ip::tcp::socket socket{ io_context };
    boost::asio::async_connect(socket, resolver.resolve("yandex.ru", "http"),
    [](boost::system::error_code ec, const auto& endpoint){
        std::cout << endpoint << std::endl;
    });
    io_context.run();
}

std::string request(std::string host, boost::asio::io_context& io_context){
    std::stringstream request_stream;
    request_stream << "GET / HTTP/1.1\r\n"
    "Host: " << host << "\r\n"
    "Accept: text/html\r\n"
    "Accept-Language: en-us\r\n"
    "Accept-Encoding: identity\r\n"
    "Connection: close\r\n\r\n";
    const auto request = request_stream.str();
    boost::asio::ip::tcp::resolver resolver{ io_context };
    const auto endpoints = resolver.resolve(host, "http");
    boost::asio::ip::tcp::socket socket{ io_context };
    const auto connected_endpoint = boost::asio::connect(socket, endpoints);
    boost::asio::write(socket, boost::asio::buffer(request));
    std::string response;
    boost::system::error_code ec;
    boost::asio::read(socket,boost::asio::dynamic_buffer(response), ec);
    if(ec && ec.value() != 2) throw boost::system::system_error{ ec };
    return response;
}

using ResolveResult = boost::asio::ip::tcp::resolver::results_type;
using Endpoint = boost::asio::ip::tcp::endpoint;

struct Request{
    explicit Request(boost::asio::io_context& io_context, std::string host)
    : resolver{io_context}, socket{io_context},host{std::move(host)}{
    std::stringstream request_stream;
    request_stream << "GET / HTTP/1.1\r\n"
    "Host: " << host << "\r\n"
    "Accept: text/html\r\n"
    "Accept-Language: en-us\r\n"
    "Accept-Encoding: identity\r\n"
    "Connection: close\r\n\r\n";
    //"User-Agent: C++ Crash Course Client\r\n\r\n";
    request = request_stream.str();
    resolver.async_resolve(this->host, "http", 
        [this](boost::system::error_code ec,const ResolveResult& results){
        resolution_handler(ec, results);
    });
    }
    void resolution_handler(boost::system::error_code ec, const ResolveResult& results){
        if(ec){
            std::cerr << "Error resolving " << host << ": " << ec << std::endl;
            return;
        }
        boost::asio::async_connect(socket, results,[this](boost::system::error_code ec, const Endpoint& endpoint){
            connection_handler(ec, endpoint);
        });
    }
    void connection_handler(boost::system::error_code ec, const Endpoint& endpoint){
        if(ec){
            std::cerr << "Error connecting to " << host << ": " << ec.message() << std::endl;
            return;
        }
        boost::asio::async_write(socket, boost::asio::buffer(request),
    [this](boost::system::error_code ec, size_t transferred){
        write_handler(ec,transferred);
    });
    }
    void write_handler(boost::system::error_code ec, size_t transferred){
        if(ec){
            std::cerr << "Error writing to " << host << ": " << ec.message() << std::endl;
        } else if(request.size() != transferred){
            request.erase(0, transferred);
            boost::asio::async_write(socket, boost::asio::buffer(request),
            [this](boost::system::error_code ec, size_t transferred){
                write_handler(ec,transferred);
            });
        } else {
            boost::asio::async_read(socket, boost::asio::dynamic_buffer(response),
            [this](boost::system::error_code ec, size_t transferred){
                read_handler(ec, transferred);
            });
        }
    }
    void read_handler(boost::system::error_code ec, size_t transferred){
        if(ec && ec.value() != 2){
            std::cerr << "Error reading from " << host << ": " << ec.message() << std::endl;
        }
    }
    const std::string& get_response() const noexcept{ return response; }
    private:
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::ip::tcp::socket socket;
    std::string request, response;
    const std::string host;
};

void aclient(){
    boost::asio::io_context io_context;
    Request request{ io_context, "google.com" };
    //Request request{ io_context, "yandex.ru" };
    //Request request{ io_context, "nikodimus.ru" };
    io_context.run();
    std::cout << request.get_response() << std::endl;
}

void client(){
    boost::asio::io_context io_context;
    try{
        //const auto response = request("www.arcyber.army.mil", io_context);
        //const auto response = request("whitehouse.gov", io_context);
        //const auto response = request("nikodimus.ru", io_context);
        const auto response = request("google.com", io_context);
        std::cout << response << "\n";
    } catch(boost::system::system_error& se){
        std::cerr << "Error: " << se.what() << std::endl;
    }
}

int main(){
    client();
    //aclient();
    //asioconnect();
    //asioasyncconnect();
    //asioresolver();
    //asioasyncresolver();
    //asiotimers();
    return 0;
}
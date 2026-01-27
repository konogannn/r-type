#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

int main()
{
    boost::asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), 0));
    udp::endpoint remote_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 4242);

    std::string message = "Hello";
    socket.send_to(boost::asio::buffer(message), remote_endpoint);

    char buffer[1024];
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(boost::asio::buffer(buffer), sender_endpoint);

    std::cout << "Received: " << std::string(buffer, len) << std::endl;

    return 0;
}

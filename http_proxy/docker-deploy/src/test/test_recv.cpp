#include <boost/asio.hpp>
#include <iostream>
#include <vector>

using boost::asio::ip::tcp;

void write_handler(const boost::system::error_code& error, std::size_t length)
{
  if (!error)
  {
    std::cout << "Sent " << length << " bytes." << std::endl;
  }
  else
  {
    std::cerr << "Write error: " << error.message() << std::endl;
  }
}

void read_handler(const boost::system::error_code& error, std::vector<char>& data, std::size_t length)
{
  std::cout.write(data.data(), data.size());
  std::cout << std::endl;
  if (!error)
  {
  }
  else
  {

    std::cerr << "read error: " << error.message() << std::endl;
  }
}

int main(int argc, char ** argv)
{
  try
  {
    boost::asio::io_context io_context;

    tcp::socket dest_socket(io_context);
    int port = std::stol(std::string(argv[1]));
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    acceptor.accept(dest_socket);

    std::vector<char> data;

    // char ch;
    // while (std::cin.get(ch)) {
    //     data.push_back(ch);
    // }

    //boost::asio::async_write(dest_socket, boost::asio::buffer(data), write_handler);

    std::vector<char> data_recv(20);
    boost::asio::async_read(dest_socket, boost::asio::buffer(data_recv), 
      [&](const boost::system::error_code& error, std::size_t length) { 
        read_handler(error, data_recv, length); });

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}


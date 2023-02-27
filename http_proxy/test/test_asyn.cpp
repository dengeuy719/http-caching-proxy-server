#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

using boost::asio::ip::tcp;

void receive_handler(const boost::system::error_code & error, std::size_t length,
                     std::vector<char>& data, boost::asio::ip::tcp::socket & source_socket, 
                     boost::asio::ip::tcp::socket & dest_socket, std::size_t sum, boost::asio::io_context & io_context) {
    if (!error) {
        // if (length < data.size()) {
        //   return;
        // }
        // data.clear();
        // boost::asio::async_read(source_socket, boost::asio::buffer(data),
        // [&](const boost::system::error_code& error, std::size_t length) {
        //     receive_handler(error, length, data, source_socket, dest_socket, 0, io_context);
        // });
    } else {
      std::cerr << "read error: " << error.message() << std::endl;
        if (length == 0) {
            io_context.stop();
        }
    }
    // boost::asio::write(dest_socket, boost::asio::buffer(data),
    //         [&](const boost::system::error_code& error, std::size_t length) {
    //             if (error) {
    //                 std::cerr << "Write error: " << error.message() << std::endl;
    //             }
    //         });
    boost::asio::write(dest_socket, boost::asio::buffer(data));
    data.clear();
    boost::asio::async_read(source_socket, boost::asio::buffer(data),
        [&](const boost::system::error_code& error, std::size_t length) {
            receive_handler(error, length, data, source_socket, dest_socket, 0, io_context);
        });
}

bool handle_data(int from_fd, int to_fd) {
  std::vector<char> buffer(1024);
  int n = recv(from_fd, buffer.data(), buffer.size(), 0);
  if (n == 0) {
    return false;
  }
  send(to_fd, buffer.data(), buffer.size(), 0);
  return true;
}

int main()
{
  try
  {
    boost::asio::io_context io_context;

    tcp::socket source_socket(io_context);
    source_socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12345));

    tcp::socket dest_socket(io_context);
    dest_socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 56789));

    std::vector<char> client_data(1024);
    int source_fd = source_socket.native_handle();
    int dest_fd = dest_socket.native_handle();
    fd_set read_fdset;
    FD_ZERO(&read_fdset);
    FD_SET(source_fd, &read_fdset);
    FD_SET(dest_fd, &read_fdset);
    int max_fd = std::max(source_fd, dest_fd);
    while (true) {
      fd_set temp_fdset = read_fdset;
      int status = select(max_fd + 1, &temp_fdset, NULL, NULL, NULL);
        if (status < 0) {
            perror("Select error!");
            exit(EXIT_FAILURE);
        }
      if (FD_ISSET(source_fd, &temp_fdset)) {
        if (!handle_data(source_fd, dest_fd)) {
          break;
        }

      }
      if (FD_ISSET(dest_fd, &temp_fdset)) {
        if (!handle_data(dest_fd, source_fd)) {
          break;
        }

      }
    }





    // boost::asio::async_read(source_socket, boost::asio::buffer(client_data),
    //     [&](const boost::system::error_code& error, std::size_t length) {
    //         receive_handler(error, length, client_data, source_socket, dest_socket, 0, io_context);
    //     });
    // std::vector<char> server_data(10);
    // boost::asio::async_read(dest_socket, boost::asio::buffer(server_data),
    //     [&](const boost::system::error_code& error, std::size_t length) {
    //         receive_handler(error, length, server_data, dest_socket, source_socket, 0, io_context);
    //     });
    
    std::cout << "end\n";
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
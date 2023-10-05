#include "Connection.hpp"
#include <iostream>

int main(int argc, char **argv) {
	Client client("15466.courses.cs.cmu.edu", "15466"); //connect to a local server at port 1337

	{ //send handshake message:
		Connection &con = client.connection;
		con.send_buffer.emplace_back('H');
		con.send_buffer.emplace_back( 13 );
		con.send_buffer.emplace_back('g');
		for (char c : std::string("inesup")) {
			con.send_buffer.emplace_back(c);
		}
		for (char c : std::string("ntonev")) {
			con.send_buffer.emplace_back(c);
		}
	}

	while (true) {
		client.poll([](Connection *connection, Connection::Event evt){
					},
					0.0 //timeout (in seconds)
		);

		Connection &con = client.connection;

		while (true) {
			if (con.recv_buffer.size() < 2) break;
			char type = char(con.recv_buffer[0]);
			size_t length = size_t(con.recv_buffer[1]);
			if (con.recv_buffer.size() < 2 + length) break;

			if (type == 'T') { /* tell message */
				const char *as_chars = reinterpret_cast< const char * >(con.recv_buffer.data());
				std::string message(as_chars + 2, as_chars + 2 + length);

				std::cout << "T: '" << message << "'" << std::endl;
			} else if (type == 'G') {
				std::cout << "G: GEM!!!!!" << std::endl;
			} else if (type == 'V') {
				const char *as_chars = reinterpret_cast< const char * >(con.recv_buffer.data());
				std::string view_message(as_chars + 2, as_chars + 11);

				std::cout << "V: " << std::endl << "-----" << std::endl;
				for (size_t i = 0; i < 9; i += 3) {
					std::cout << "|" << view_message[i] << view_message[i + 1] << view_message[i + 2] << "|" << std::endl;
				}
				std::cout << "-----" << std::endl;

				char command;
				std::cin >> command;

				auto send_command = [&con](const char command) {
					con.send_buffer.emplace_back('M');
					con.send_buffer.emplace_back( 1 );
					con.send_buffer.emplace_back(command);
				};

				switch (command) {
					case 'w':
						send_command('N');
						break;

					case 'a':
						send_command('W');
						break;

					case 's':
						send_command('S');
						break;

					case 'd':
						send_command('E');
						break;

					case 'f':
						send_command('f');
						break;

					default:
						send_command('.');
						break;
				}
			} else {
				std::cout << "Ignored a " << type << " message of length " << length << "." << std::endl;
			}

			con.recv_buffer.erase(con.recv_buffer.begin(), con.recv_buffer.begin() + 2 + length);
		}
	}
}
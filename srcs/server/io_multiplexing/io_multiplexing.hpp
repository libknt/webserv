#ifndef SO_MULTIPLEING_HPP
# define SO_MULTIPLEING_HPP

#include "socket.hpp"
#include <vector>

namespace server
{
  

class IoMultipleing {
  private:
  std::vector<server::Socket> socket_;
  int max_sd_;
  int max_clients_;
  struct timeval timeout_;
	fd_set master_set_;
  fd_set working_set_;
};

} // namespace server

#endif
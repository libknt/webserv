#include "server.hpp"

int conf_setting() {
  return true;
}

int main (int argc ,char **argv) {

  if (conf_setting()) {
    return 1;
  }
  server();

}
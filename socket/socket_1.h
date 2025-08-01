#include "headers/abstract_socket.h"

extern"C"{
#include "cycle_buf.h"
}
#include <stdint.h>

class Socket: public svlp::ABstractSocket
{
public:
	Socket();
public:
    uint32_t read(void * const buf, const uint32_t len);
    uint32_t skip(uint32_t const  len);
    bool is_ready_read();
    void flush();
    uint32_t write(const void *buf, const uint32_t len);

    bool open();
    bool close();
private:
};


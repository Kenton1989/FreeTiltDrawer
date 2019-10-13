#include "port_monitor.h"
#include "SerialStream.h"
#include <unistd.h>
#include <iostream>


class SerialStreamRaii {
public:
    SerialStreamRaii(): data_() {}
    ~SerialStreamRaii() { data_.Close(); }
    LibSerial::SerialStream& data() { return data_; }
private:
    LibSerial::SerialStream data_;
};

namespace GG4U {

int arduino_port_monitor(const char* device,
                         const Signal_Func* signal_func,
                         int signal_num) {
    using namespace LibSerial;
    SerialStreamRaii port_raii;
    LibSerial::SerialStream& port = port_raii.data();

    port.Open(device);
    if (!port.good()) {
        std::cerr << "[ " << __FILE__ << ": " << __LINE__ << "]\n"
                  << "Error: Fail to open device: " << device << std::endl;
        return -1;
    }
    port.SetBaudRate(SerialStreamBuf::BAUD_115200);
    if (!port.good()) {
        std::cerr << "[ " << __FILE__ << ": " << __LINE__ << "]\n"
                  << "Error: Fail to change bund rate." << std::endl;
        return -1;
    }

    while(port.good()) {
        char signal;
        port.get(signal);
        for(int i = 0; i < signal_num; ++i) {
            if(signal == signal_func[i].first) {
                signal_func[i].second();
                break;
            }
        }
    }
}

} //namespace GG4U


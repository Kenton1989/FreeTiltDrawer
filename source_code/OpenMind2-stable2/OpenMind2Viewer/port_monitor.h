#ifndef GG4U_ARDUINO_PORT_MONITOR_H_
#define GG4U_ARDUINO_PORT_MONITOR_H_

#include <utility>

namespace GG4U {

typedef void (*FuncT) (void);

typedef std::pair<char, FuncT> Signal_Func;

int arduino_port_monitor(const char* device,
                         const Signal_Func* signal_function,
                         int signal_num);


}//GG4U

#endif //GG4U_ARDUINO_PORT_MONITOR_H_

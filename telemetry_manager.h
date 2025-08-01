#ifndef __TELEMETRY_MANAGER_H__
#define __TELEMETRY_MANAGER_H__

#include "headers/svlp_writer.h"

#include "common_data_structures.h"
#include "Temperature18B20.h"

class Telemetry_manager
{
public:
	Telemetry_manager(svlp::SVLP_Writer& writer):
	writer(writer)
	{
	}

	void send_telemetry()
	{
		svlp::svlp_message msg;

		telemetry_electronic_load telemetry;
		telemetry.temp = getTemp() * gradient_multiplyer;

		msg = writer.create_svlp_message(svlp::infocode_telemetry, msg_code_telemetry_kpa, telemetry);
		writer.send_message(msg);
	}

private:
	svlp::SVLP_Writer& writer;
};

#endif //__TELEMETRY_MANAGER_H__

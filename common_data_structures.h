#include  <stdint.h>

const uint8_t gradient_multiplyer = 100;

enum cmd_telemetry
{
    msg_code_telemetry_kpa = 12
};

struct telemetry_electronic_load
{
	uint16_t temp;
	uint16_t current;
	uint16_t voltage;
	uint16_t power;
};

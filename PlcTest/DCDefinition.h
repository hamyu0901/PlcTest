#pragma once

// PLC
#define COLLECTOR_MESECL_LOGICAL_STATION_ID		2
#define COLLECTOR_AB_CONNECTINO_SERIAL_NO		62

#define DEFAULT_PLC_IP							_T("192.168.100.1")
#define DEFAULT_PLC_PORT						5002
#define DEFAULT_PC_STATION_NO					62
#define DEFAULT_PLC_STATION_NO					1
#define DEFAULT_PLC_NETWORK_NO					10
#define DEFAULT_PLC_TIMEOUT						1000
#define DEFAULT_PLC_STARTADDR					0
#define DEFAULT_PLC_ENDADDR						983039

#define PLC_CPU_TYPE_12H						0x00000024
#define PLC_CPU_TYPE_13UDH						0x00000073


//XML Config
#define NODE_ADDR_CONFIG						_T("//ADDRCONFIG")				// Historian

#define ATTR_PLC_TYPE							_T("PLCType")

#define ATTR_USABLE								_T("Usable")
#define ATTR_DEVICE								_T("Device")
#define ATTR_MONITORING_START_ADDR				_T("MStartAddr")
#define ATTR_MONITORING_END_ADDR				_T("MEndAddr")
#define ATTR_MONITORING_ITEM_COUNT				_T("MItemCount")
#define ATTR_MONITORING_ITEM_NAME				_T("MItemName")
#define ATTR_DONE_START_ADDR					_T("DStartAddr")
#define ATTR_DONE_END_ADDR						_T("DEndAddr")
#define ATTR_DONE_COUNT							_T("DoneCount")
#define ATTR_DONE_NAME							_T("DoneName")
#define ATTR_VIN_NO								_T("Vinno")

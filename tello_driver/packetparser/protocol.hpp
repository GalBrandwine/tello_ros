#pragma once
#include <chrono>
#include <string>
#include <algorithm>
#include "utils.hpp"
#include "crc.hpp"
#include <iostream>
#include <memory>
namespace tello_protocol
{
    //low-level Protocol (https://tellopilots.com/wiki/protocol/#MessageIDs)
    static const unsigned int START_OF_PACKET = 0xcc;
    static const unsigned int SSID_MSG = 0x0011;                //
    static const unsigned int SSID_CMD = 0x0012;                //
    static const unsigned int SSID_PASSWORD_MSG = 0x0013;       //
    static const unsigned int SSID_PASSWORD_CMD = 0x0014;       //
    static const unsigned int WIFI_REGION_MSG = 0x0015;         //
    static const unsigned int WIFI_REGION_CMD = 0x0016;         //
    static const unsigned int WIFI_MSG = 0x001a;                //
    static const unsigned int VIDEO_ENCODER_RATE_CMD = 0x0020;  //
    static const unsigned int VIDEO_DYN_ADJ_RATE_CMD = 0x0021;  //
    static const unsigned int EIS_CMD = 0x0024;                 //
    static const unsigned int VIDEO_START_CMD = 0x0025;         //
    static const unsigned int VIDEO_RATE_QUERY = 0x0028;        //
    static const unsigned int TAKE_PICTURE_COMMAND = 0x0030;    //
    static const unsigned int VIDEO_MODE_CMD = 0x0031;          //
    static const unsigned int VIDEO_RECORD_CMD = 0x0032;        //
    static const unsigned int EXPOSURE_CMD = 0x0034;            //
    static const unsigned int LIGHT_MSG = 0x0035;               //
    static const unsigned int JPEG_QUALITY_MSG = 0x0037;        //
    static const unsigned int ERROR_1_MSG = 0x0043;             //
    static const unsigned int ERROR_2_MSG = 0x0044;             //
    static const unsigned int VERSION_MSG = 0x0045;             //
    static const unsigned int TIME_CMD = 0x0046;                //
    static const unsigned int ACTIVATION_TIME_MSG = 0x0047;     //
    static const unsigned int LOADER_VERSION_MSG = 0x0049;      //
    static const unsigned int STICK_CMD = 0x0050;               //
    static const unsigned int TAKEOFF_CMD = 0x0054;             //
    static const unsigned int LAND_CMD = 0x0055;                //
    static const unsigned int FLIGHT_MSG = 0x0056;              //
    static const unsigned int SET_ALT_LIMIT_CMD = 0x0058;       //
    static const unsigned int FLIP_CMD = 0x005c;                //
    static const unsigned int THROW_AND_GO_CMD = 0x005d;        //
    static const unsigned int PALM_LAND_CMD = 0x005e;           //
    static const unsigned int TELLO_CMD_FILE_SIZE = 0x0062;     //  # pt50
    static const unsigned int TELLO_CMD_FILE_DATA = 0x0063;     //  # pt50
    static const unsigned int TELLO_CMD_FILE_COMPLETE = 0x0064; //  # pt48
    static const unsigned int SMART_VIDEO_CMD = 0x0080;         //
    static const unsigned int SMART_VIDEO_STATUS_MSG = 0x0081;  //
    static const unsigned int LOG_HEADER_MSG = 0x1050;          //
    static const unsigned int LOG_DATA_MSG = 0x1051;            //
    static const unsigned int LOG_CONFIG_MSG = 0x1052;          //
    static const unsigned int BOUNCE_CMD = 0x1053;              //
    static const unsigned int CALIBRATE_CMD = 0x1054;           //
    static const unsigned int LOW_BAT_THRESHOLD_CMD = 0x1055;   //
    static const unsigned int ALT_LIMIT_MSG = 0x1056;           //
    static const unsigned int LOW_BAT_THRESHOLD_MSG = 0x1057;   //
    static const unsigned int ATT_LIMIT_CMD = 0x1058;           // #Stated incorrectly by Wiki (checked from raw packets)
    static const unsigned int ATT_LIMIT_MSG = 0x1059;           //

    static const std::string EMERGENCY_CMD("emergency");

    //Flip commands taken from Go version of code
    //FlipFront flips forward.;
    static const int FlipFront = 0;
    //FlipLeft flips left.;
    static const int FlipLeft = 1;
    //FlipBack flips backwards.;
    static const int FlipBack = 2;
    //FlipRight flips to the right.;
    static const int FlipRight = 3;
    //FlipForwardLeft flips forwards and to the left.;
    static const int FlipForwardLeft = 4;
    //FlipBackLeft flips backwards and to the left.;
    static const int FlipBackLeft = 5;
    //FlipBackRight flips backwards and to the right.;
    static const int FlipBackRight = 6;
    //FlipForwardRight flips forwards and to the right.;
    static const int FlipForwardRight = 7;

    class Packet
    {
    public:
        Packet(const std::string &cmd);
        Packet(const std::string &cmd, const std::string &payload, const unsigned char pkt_type = 0x68);
        Packet(const unsigned int cmd, const unsigned char pkt_type = 0x68);
        Packet(const unsigned int cmd, const std::string &payload, const unsigned char pkt_type = 0x68);
        ~Packet();
        void Fixup(int seq_num = 0);
        const std::string &GetBuffer() const;
        const int GetBufferSize() const;
        const std::string GetData() const;

        std::unique_ptr<tm*> GetTime(unsigned char *buff = nullptr) const;
        void AddTime(const tm *time);
        void AddByte(const unsigned char val);

    private:
        int m_bufSize;
        std::string m_buf_hex; // = nullptr;
        std::string m_buf_dec; // = nullptr;
        void addInt16(const int16_t val);
    };
}; // namespace tello_protocol

static std::ostream &operator<<(std::ostream &os, const tello_protocol::Packet &pkt)
{
    os << "Packet: ";
    for (size_t i = 0; i < pkt.GetBufferSize(); i++)
    {
        os << std::hex << (int)(pkt.GetBuffer()[i]) << " ";
    }
    os << "\n";
    return os;
};
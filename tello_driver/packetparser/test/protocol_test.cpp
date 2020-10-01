#include "protocol.hpp"
#include <iostream>
#include <string>
#include <gtest/gtest.h>

TEST(TelloProtocolTest, TestHexValue)
{
    ASSERT_EQ(0x1051, tello_protocol::LOG_DATA_MSG);
}

TEST(TelloProtocolTest, PacketStringCmd)
{
    const std::string cmd("TAKEOFF");
    auto packet = tello_protocol::Packet(cmd);
    for (int i = 0; i < packet.GetBufferSize(); i++)
    {
        ASSERT_EQ(cmd.at(i), packet.GetBuffer().at(i));
    }
}

TEST(TelloProtocolTest, PacketByteCmd)
{
    // Create Takeoff CMD
    auto pkt = tello_protocol::Packet(tello_protocol::TAKEOFF_CMD);
    pkt.Fixup();

    // std::cout << pkt;

    // Test

    /*     
        Position	Usage
        0	        0xcc indicates the start of a packet
        1-2	        Packet size, 13 bit encoded ([2] << 8) | ([1] >> 3)
        3	        CRC8 of bytes 0-2
        4	        Packet type ID
        5-6	        Command ID, little endian
        7-8	        Sequence number of the packet, little endian
        9-(n-2)	    Data (if any)
        (n-1)-n	    CRC16 of bytes 0 to n-2


                      START_OF_PACKET
                           |
                           |   PacketSize
                           |   0x58[16]==88[10](bits)==11[bytes]
                           |      |            
                           |      |
                           |      |
                           |      |      TAKEOFF_CMD = 0x00 0x54 (little endian)
                           |      |                 |    |
    */
    unsigned char res[] = {0xcc, 0x58, 0x00, 0x7c, 0x68, 0x54, 0x00, 0x00, 0x00, 0xb2, 0x89};
    for (int i = 0; i < pkt.GetBufferSize(); i++)
    {
        ASSERT_EQ(res[i], (unsigned char)pkt.GetBuffer()[i]);
    }
}

TEST(TelloProtocolTest, PacketGetData)
{

    // std::string cmd();
    auto pkt = tello_protocol::Packet(tello_protocol::TAKEOFF_CMD);
    //bytearray(b'\xcc\x00\x00\x00hT\x00\x00\x00') T == 0x54, h == 0x68
    unsigned char test[] = {0xcc, 0x00, 0x00, 0x00, 0x68, 0x54, 0x00, 0x00, 0x00};
    for (int i = 0; i < pkt.GetBufferSize(); i++)
    {
        ASSERT_EQ(test[i], (unsigned char)pkt.GetBuffer()[i]);
    }

    pkt.Fixup();
    std::cout << pkt;

    // Test - Data start's from 9th Byte
    // bytearray(b'\xccX\x00|hT\x00\x00\x00\xb2\x89')
    unsigned char test2[] = {0xcc, 0x58, 0x00, 0x7c, 0x68, 0x54, 0x00, 0x00, 0x00, 0xb2, 0x89};
    auto testData = pkt.GetBuffer();
    for (int i = 0; i < pkt.GetBufferSize(); i++)
    {
        ASSERT_EQ(test2[i], (unsigned char)testData.at(i));
    }
}

TEST(TelloProtocolTest, PacketAddTime)
{

    std::string cmd("TestTest0TAKEOFF");
    auto pkt = tello_protocol::Packet(cmd);

    time_t now = time(0);
    auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    // convert now to string form
    char *dt = ctime(&timenow);

    std::cout << "The local date and time is: " << dt << std::endl;

    tm *gmtm = gmtime(&timenow);
    pkt.AddTime(gmtm);
    pkt.Fixup();

    std::cout << pkt << std::endl;

    // Test - Data start's from 9th Byte
    auto test = pkt.GetData();
    // Extract time:
    auto time_str = test.substr(7);
    // Little endian
    auto hour = (unsigned char *)time_str.substr(0, 2).c_str();
    ASSERT_EQ(*hour, gmtm->tm_hour);

    auto min = (unsigned char *)time_str.substr(2, 3).c_str();
    ASSERT_EQ(*min, gmtm->tm_min);

    auto sec = (unsigned char *)time_str.substr(4, 6).c_str();
    ASSERT_EQ(*sec, gmtm->tm_sec);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
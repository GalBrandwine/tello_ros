#include "protocol.hpp"
#include <iostream>
#include <gtest/gtest.h>

TEST(TelloProtocolTest, TestHexValue)
{
    ASSERT_EQ(0x1051, tello_protocol::LOG_DATA_MSG);
}

TEST(TelloProtocolTest, PacketStringCmd)
{
    std::string cmd("TAKEOFF");
    auto packet = tello_protocol::Packet(cmd);
    for (int i = 0; i < packet.GetBufferSize(); i++)
    {
        ASSERT_EQ(cmd.at(i), packet.GetBuffer()[i]);
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
    unsigned char res[] = {0xcc, 0x58, 0x0, 0x5B, 0x68, 0x54, 0, 0, 0};
    for (int i = 0; i < pkt.GetBufferSize(); i++)
    {
        ASSERT_EQ(res[i], pkt.GetBuffer()[i]);
    }
}

TEST(TelloProtocolTest, PacketGetData)
{

    std::string cmd("TestTest0TAKEOFF");
    auto pkt = tello_protocol::Packet(cmd);
    pkt.Fixup();

    std::cout << pkt;

    // Test - Data start's from 9th Byte
    auto test = pkt.GetData();
    for (int i = 0; i < cmd.length() - 9; i++)
    {
        // std::cout << cmd.at(i + 9) << ", " << test.get()[i] << "\n";
        ASSERT_EQ(cmd.at(i + 9), test.get()[i]);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
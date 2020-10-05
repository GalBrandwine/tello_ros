#include "protocol.hpp"
#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <asio.hpp>
TEST(TelloProtocolTest, TestHexValue)
{
    ASSERT_EQ(0x1051, tello_protocol::LOG_DATA_MSG);
}

TEST(TelloProtocolTest, PacketStringCmd)
{
    std::string cmd(" ");
    cmd.at(0) = tello_protocol::START_OF_PACKET;
    cmd.append("TAKEOFF");
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
    TearDownTestCase();
}

TEST(TelloProtocolTest, PacketAddTime)
{
    auto pkt = tello_protocol::Packet(tello_protocol::START_OF_PACKET);

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
    // auto time_str = test.substr(7);
    // Little endian
    auto hour = (unsigned char *)test.substr(0, 2).c_str();
    ASSERT_EQ(*hour, gmtm->tm_hour);

    auto min = (unsigned char *)test.substr(2, 3).c_str();
    ASSERT_EQ(*min, gmtm->tm_min);

    auto sec = (unsigned char *)test.substr(4, 6).c_str();
    ASSERT_EQ(*sec, gmtm->tm_sec);
    TearDownTestCase();
}

TEST(TelloProtocolTest, PacketGetTime)
{

    auto pkt = tello_protocol::Packet(tello_protocol::START_OF_PACKET);

    time_t now = time(0);
    auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    // convert now to string form
    char *dt = ctime(&timenow);

    std::cout << "The local date and time is: " << dt << std::endl;

    tm *gmtm = gmtime(&timenow);
    pkt.AddTime(gmtm);
    pkt.Fixup();

    std::cout << pkt << std::endl;

    // Test
    auto test = (*pkt.GetTime().get());

    ASSERT_EQ(test->tm_hour, gmtm->tm_hour);
    ASSERT_EQ(test->tm_min, gmtm->tm_min);
    ASSERT_EQ(test->tm_sec, gmtm->tm_sec);
    TearDownTestCase();
}

TEST(TelloProtocolTest, PacketConnReq)
{

    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    std::cout << pkt << std::endl;

    // Test
    unsigned char conn_req_from_drone[] = {0x63, 0x6f, 0x6e, 0x6e, 0x5f, 0x72, 0x65, 0x71, 0x3a, 0x96, 0x17}; // "conn_req:\x96\x17" (Sniffed using wireshark)
    int index = 0;
    for (auto &letter : pkt.GetBuffer())
    {

        ASSERT_EQ(conn_req_from_drone[index], (unsigned char)letter);
        index++;
    }
}

TEST(TelloCommandTest, SendConnReq)
{
    // Send "conn_req:\x96\x17" to drone, test if received "conn_ack:\x96\x17"

    using asio::ip::udp;

    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    /*
    Packets from the drone suppose to arrive at this (address,port): ('',9000)
    Packets are being send from client to drone to this (address,port) = ('192.168.10.1', 8889)
    */
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);

    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket

    //Run
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    std::cout << pkt << std::endl;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    // Test
    using namespace std::chrono_literals;
    bool keep_sending = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    // It is known that conn_req answer is in size 11 bytes
    size_t r = 0;

    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    while (keep_sending)
    {
        r = tello_socket.receive(asio::buffer(buffer_));
        if (r == 11)
            keep_sending = false;
        std::this_thread::sleep_for(0.5s);
    }

    // "conn_ack:\x96\x17" (Sniffed using wireshark)
    std::vector<unsigned char> conn_ack_from_drone = {0x63, 0x6f, 0x6e, 0x6e, 0x5f, 0x61, 0x63, 0x6b, 0x3a, 0x96, 0x17};

    int index = 0;
    for (auto &letter : conn_ack_from_drone)
    {
        ASSERT_EQ(buffer_[index], letter);
        index++;
    }
    TearDownTestCase();
}

TEST(TelloCommandTest, SendTakeoff)
{
    /* 
    This is the most stupid test I have ever done. 
    Im testing 'Takeoff' command, without even knowing how to land the drone.
    
    Luckily, this is a small drone,
    so I just caught it by hand.
    
    NEVER DO SUCH A THING WITH BIG DRONES!!!
    */

    // Setup
    using asio::ip::udp;
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket
    using namespace std::chrono_literals;
    bool keep_sending = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;

    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    while (keep_sending)
    {
        r = tello_socket.receive(asio::buffer(buffer_));
        if (r == 11)
            keep_sending = false;
        std::this_thread::sleep_for(0.5s);
    }

    // Set max height
    pkt = tello_protocol::Packet(tello_protocol::SET_ALT_LIMIT_CMD);
    pkt.AddByte(0x1e); // 30m maximum height
    pkt.AddByte(0x00); // little endian
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    // Test
    pkt = tello_protocol::Packet(tello_protocol::TAKEOFF_CMD);
    pkt.Fixup();
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);



    // using namespace std::chrono_literals;
    // bool keep_sending = true;
    // auto buffer_ = std::vector<unsigned char>(1024);
    // // It is known that conn_req answer is in size 11 bytes
    // size_t r = 0;

    // tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    // while (keep_sending)
    // {
    //     r = tello_socket.receive(asio::buffer(buffer_));
    //     if (r == 11)
    //         keep_sending = false;
    //     std::this_thread::sleep_for(0.5s);
    // }

    // // "conn_ack:\x96\x17" (Sniffed using wireshark)
    // std::vector<unsigned char> conn_ack_from_drone = {0x63, 0x6f, 0x6e, 0x6e, 0x5f, 0x61, 0x63, 0x6b, 0x3a, 0x96, 0x17};

    // int index = 0;
    // for (auto &letter : conn_ack_from_drone)
    // {
    //     ASSERT_EQ(buffer_[index], letter);
    //     index++;
    // }
    TearDownTestCase();
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
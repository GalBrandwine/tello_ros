#include "flight_data_test.hpp"

TEST(TelloCommandTest, SendConnReq)
{
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
TEST(TelloTelemetryTest, ReceiveLogHeaderOnce)
{
    /* 
    LOG_DATA session with the Tello drone is triggered with '"conn_req:\x96\x17"' request.
    Once this request has been sent, the client will receive "ack_req:\x96\x17".
    And a LOG_DATA session will start.

    CMD will be 'LOG_HEADER_MSG' only once, at the time of receiveing the ACK.
    If cmd is 'LOG_DATA_MSG', it means that a LOG_DATA session has been established. So this test is over.
    */

    // Setup
    using asio::ip::udp;

    // Input socket
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket

    // Output socket
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);

    using namespace std::chrono_literals;
    bool keep_receiving = true, is_log_header_received = false;
    int received_counter;
    auto buffer_ = std::vector<unsigned char>(1024);

    auto telloTelemerty = tello_protocol::TelloTelemetry(spdlog::stdout_color_mt("tellemetry"));
    //Run

    // Connect
    auto conn_req = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(conn_req.GetBuffer()), remote_endpoint_);

    size_t r = 0;

    auto log_header_logger = spdlog::stdout_color_mt("log_header_message");

    auto send_ack = [&tello_socket, &remote_endpoint_, &log_header_logger](const uint16_t id) {
        auto pkt = tello_protocol::Packet(tello_protocol::LOG_HEADER_MSG, 0x50);
        pkt.AddByte(0x00);
        Byte byte = le16(id);
        pkt.AddByte(byte.LeftNibble);
        pkt.AddByte(byte.RightNibble);
        pkt.Fixup();
        log_header_logger->info("Senging conn_ack msg: {}", spdlog::to_hex(pkt.GetBuffer()));
        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    while (keep_receiving && !is_log_header_received)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r);
        auto received = tello_protocol::Packet(data);

        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);
        if (cmd == tello_protocol::LOG_HEADER_MSG)
        {
            // LOG HEADER MSG is the biggest log message.
            // Drone will continue to send log_header connection requests with incremental ID.
            // Until sending back to drone 'ack_conn'.
            auto id = uint16(data[9], data[10]);

            telloTelemerty.SetBuildDate(received.GetBuffer().substr(28, 26));
            // DJI LOG VERSION something like this: DJI_LOG_V3I��Rc
            telloTelemerty.SetDJILogVersion(received.GetBuffer().substr(245, 6));

            // After sending back ack. the drone will not sent LOG_HEADER_MSG anymore.
            send_ack(id);
            is_log_header_received = true;
        }
        else if (cmd == tello_protocol::LOG_DATA_MSG)
        {
            /* 
            Is cmd  is LOG_DATA_MSG.
            It means that a LOG_DATA connections has been already assured. 
            So this test is pseudo-passed.
            */
            is_log_header_received = true;
            keep_receiving = false;
        }

        std::fill(buffer_.begin(), buffer_.end(), 0);

        // received_counter++;
        // if (received_counter > 1000)
        // {
        //     keep_receiving = false;
        // }
    }
    ASSERT_TRUE(is_log_header_received);

    TearDownTestCase();
}
TEST(TelloTelemetryTest, ReceiveLogDataMvoMsg)
{

    // Setup
    using asio::ip::udp;

    // Input socket
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket

    // Output socket
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);

    using namespace std::chrono_literals;
    bool keep_receiving = true, is_log_header_received = false;
    int log_data_msg_counter = 0;
    auto buffer_ = std::vector<unsigned char>(1024);

    // Create TelloTelemetry instance.
    auto telloTelemerty = tello_protocol::TelloTelemetry(spdlog::stdout_color_mt("tellemetry"));
    auto logdata = std::make_shared<tello_protocol::LogData>(spdlog::stdout_color_mt("logdata"));
    telloTelemerty.SetLogData(logdata);

    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    auto log_msg_logger = spdlog::stdout_color_mt("log_msg_logger");

    auto send_ack = [&tello_socket, &remote_endpoint_, &log_msg_logger](const uint16_t id) {
        auto pkt = tello_protocol::Packet(tello_protocol::LOG_HEADER_MSG, 0x50);
        pkt.AddByte(0x00);
        Byte byte = le16(id);
        pkt.AddByte(byte.LeftNibble);
        pkt.AddByte(byte.RightNibble);
        pkt.Fixup();

        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    while (keep_receiving && log_data_msg_counter < 100)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r);
        auto received = tello_protocol::Packet(data);

        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);
        if (cmd == tello_protocol::LOG_HEADER_MSG)
        {
            /* 
            This part is covered in previeus 'ReceiveLogHeaderOnce' test case.
            It is necessary if drone hasnt started LOG_DATA session.
            */
            auto id = uint16(data[9], data[10]);
            send_ack(id);
        }

        else if (cmd == tello_protocol::LOG_DATA_MSG)
        {
            telloTelemerty.GetLogData()->Update(received.GetBuffer().substr(10));
            // std::cout << *telloTelemerty.GetLogData();
            log_data_msg_counter++;
        }

        std::fill(buffer_.begin(), buffer_.end(), 0);
    }

    ASSERT_GT(telloTelemerty.GetLogData()->GetLogMvo().GetUpdateCounter(), 0);
    TearDownTestCase();
}
TEST(TelloTelemetryTest, ReceiveLogDataImuMsg)
{

    // Setup
    using asio::ip::udp;

    // Input socket
    asio::io_service io_service_; // Manages IO for this socket
    unsigned short port = 9000;
    udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket

    // Output socket
    unsigned short drone_port = 8889;
    std::string drone_ip = "192.168.10.1";
    udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);

    using namespace std::chrono_literals;
    bool keep_receiving = true, is_log_header_received = false;
    int log_data_msg_counter = 0;
    auto buffer_ = std::vector<unsigned char>(1024);

    // Create TelloTelemetry instance.
    auto telloTelemerty = tello_protocol::TelloTelemetry(spdlog::stdout_color_mt("tellemetry"));
    auto logdata = std::make_shared<tello_protocol::LogData>(spdlog::stdout_color_mt("logdata"));
    telloTelemerty.SetLogData(logdata);

    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    auto log_msg_logger = spdlog::stdout_color_mt("log_msg_logger");

    auto send_ack = [&tello_socket, &remote_endpoint_, &log_msg_logger](const uint16_t id) {
        auto pkt = tello_protocol::Packet(tello_protocol::LOG_HEADER_MSG, 0x50);
        pkt.AddByte(0x00);
        Byte byte = le16(id);
        pkt.AddByte(byte.LeftNibble);
        pkt.AddByte(byte.RightNibble);
        pkt.Fixup();

        tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    };

    while (keep_receiving && log_data_msg_counter < 100)
    {
        r = tello_socket.receive(asio::buffer(buffer_));

        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r);
        auto received = tello_protocol::Packet(data);

        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);
        if (cmd == tello_protocol::LOG_HEADER_MSG)
        {
            /* 
            This part is covered in previeus 'ReceiveLogHeaderOnce' test case.
            It is necessary if drone hasnt started LOG_DATA session.
            */
            auto id = uint16(data[9], data[10]);
            send_ack(id);
        }

        else if (cmd == tello_protocol::LOG_DATA_MSG)
        {
            telloTelemerty.GetLogData()->Update(received.GetBuffer().substr(10));
            log_data_msg_counter++;
        }

        std::fill(buffer_.begin(), buffer_.end(), 0);
    }

    ASSERT_GT(telloTelemerty.GetLogData()->GetLogImuAtti().GetUpdateCounter(), 0);
    TearDownTestCase();
}
TEST(TelloTelemetryTest, ReceiveFlightData)
{
    /* 
    Test if FlightData being received and parsed correctly.
    Test pass if FlightMode == 6;
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
    bool keep_receiving = true;
    auto buffer_ = std::vector<unsigned char>(1024);
    //Run

    // Connect
    auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

    size_t r = 0;
    tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
    std::this_thread::sleep_for(0.5s);

    auto flight_data = tello_protocol::FlightData(spdlog::stdout_color_mt("flight_data"));
    while (keep_receiving)
    {
        r = tello_socket.receive(asio::buffer(buffer_));
        std::cout << "r: " << r << '\n';

        // Create function
        std::vector<unsigned char> data(buffer_.begin(), buffer_.begin() + r); // Strip last 2 bytes. they are CRC16

        auto received = tello_protocol::Packet(data);
        auto cmd = uint16(received.GetBuffer()[5], received.GetBuffer()[6]);

        if (cmd == tello_protocol::FLIGHT_MSG)
        {

            if (flight_data.SetData(received.GetData()))
            {
                ASSERT_EQ(flight_data.GetFlightMode(), 6);
                keep_receiving = false;
            }
            // std::cout << "recv: flight data: " << flight_data << "\n";

            // TODO: Test this in WIFI_MSG tests:
            // flight_data.wifi_strength = self.wifi_strength;
        }
        std::fill(buffer_.begin(), buffer_.end(), 0);
        std::this_thread::sleep_for(0.5s);
    }
    TearDownTestCase();
}

// TEST(TelloCommandTest, SendTakeoff)
// {
//     /*
//     This is the most stupid test I have ever done.
//     Im testing 'Takeoff' command, without even knowing how to land the drone.

//     Luckily, this is a small drone,
//     so I just caught it by hand.

//     NEVER DO SUCH A THING WITH BIG DRONES!!!
//     */

//     // Setup
//     using asio::ip::udp;
//     asio::io_service io_service_; // Manages IO for this socket
//     unsigned short port = 9000;
//     unsigned short drone_port = 8889;
//     std::string drone_ip = "192.168.10.1";
//     udp::endpoint remote_endpoint_(asio::ip::address_v4::from_string(drone_ip), drone_port);
//     udp::socket tello_socket(io_service_, udp::endpoint(udp::v4(), port)); // The socket
//     using namespace std::chrono_literals;
//     bool keep_sending = true;
//     auto buffer_ = std::vector<unsigned char>(1024);
//     //Run

//     // Connect
//     auto pkt = tello_protocol::Packet("conn_req:\x96\x17");
//     tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

//     size_t r = 0;

//     tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);
//     std::this_thread::sleep_for(0.5s);

//     while (keep_sending)
//     {
//         r = tello_socket.receive(asio::buffer(buffer_));
//         if (r == 11)
//             keep_sending = false;
//         std::this_thread::sleep_for(0.5s);
//     }

//     // Set max height
//     pkt = tello_protocol::Packet(tello_protocol::SET_ALT_LIMIT_CMD);
//     pkt.AddByte(0x1e); // 30m maximum height
//     pkt.AddByte(0x00); // little endian
//     tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

//     // Test
//     pkt = tello_protocol::Packet(tello_protocol::TAKEOFF_CMD);
//     pkt.Fixup();
//     tello_socket.send_to(asio::buffer(pkt.GetBuffer()), remote_endpoint_);

//     TearDownTestCase();
// }
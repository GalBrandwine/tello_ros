#include "protocol.hpp"

namespace tello_protocol
{
    Packet::Packet(const std::string &cmd)
    {
        // if ((unsigned char)cmd.at(0) != tello_protocol::START_OF_PACKET)
        // {
        //     throw std::invalid_argument{"Not A Tello packet"};
        //     return;
        // }
        m_bufSize = cmd.length();
        m_buf_hex = cmd;
    }
    Packet::Packet(const std::string &cmd, const std::string &payload, const unsigned char pkt_type)
    {
        if ((unsigned char)cmd.at(0) != tello_protocol::START_OF_PACKET)
        {
            throw std::invalid_argument{"Not A Tello packet"};
            return;
        }
        m_bufSize = cmd.length();
        m_buf_hex = cmd;
    }
    // Packet::Packet(unsigned char *cmd, const unsigned char pkt_type, const unsigned char *payoad)
    // {
    //     // TODO: calculate buff size;
    //     m_bufSize = -1;
    //     m_buf_hex = std::string(cmd);
    // }

    Packet::Packet(const unsigned int cmd, const unsigned char pkt_type)
    {
        m_buf_hex.reserve(2000); // Bytes;
        m_buf_hex.push_back(tello_protocol::START_OF_PACKET);
        m_buf_hex.push_back(0);
        m_buf_hex.push_back(0);
        m_buf_hex.push_back(0);
        m_buf_hex.push_back(pkt_type);
        m_buf_hex.push_back(cmd & 0xff);
        m_buf_hex.push_back((cmd >> 8) & 0xff);
        m_buf_hex.push_back(0);
        m_buf_hex.push_back(0);

        m_bufSize = m_buf_hex.length();
    }
    Packet::Packet(const unsigned int cmd, const std::string &payload, const unsigned char pkt_type)
    {
        m_buf_hex.reserve(2000); // Bytes;
        m_buf_hex.at(0) = tello_protocol::START_OF_PACKET;
        m_buf_hex.at(1) = 0;
        m_buf_hex.at(2) = 0;
        m_buf_hex.at(3) = 0;
        m_buf_hex.at(4) = pkt_type;
        m_buf_hex.at(5) = (cmd & 0xff);
        m_buf_hex.at(6) = ((cmd >> 8) & 0xff);
        m_buf_hex.at(7) = 0;
        m_buf_hex.at(8) = 0;

        if (payload.length() != 0)
            m_buf_hex.append(payload);

        m_bufSize = m_buf_hex.length();
    }
    Packet::~Packet()
    {
        // delete[] m_buf;
    }
    const std::string Packet::GetData() const
    {
        // Data section start after 9 bytes
        return m_buf_hex.substr(9);
    }
    const std::string &Packet::GetBuffer() const
    {
        return m_buf_hex;
    }
    const int Packet::GetBufferSize() const
    {
        return m_bufSize;
    }
    void Packet::Fixup(int seq_num)
    {

        if ((unsigned char)m_buf_hex.at(0) == tello_protocol::START_OF_PACKET)
        {
            auto byte = le16(m_bufSize + 2);
            m_buf_hex.at(1) = byte.LeftNibble;
            m_buf_hex.at(2) = byte.RightNibble;
            m_buf_hex.at(1) = (m_buf_hex.at(1) << 3);
            m_buf_hex.at(3) = crc8(m_buf_hex.substr(0, 3)); //From original packet_processor [python]: buf[0:3]
            byte = le16(seq_num);
            m_buf_hex.at(7) = byte.LeftNibble;
            m_buf_hex.at(8) = byte.RightNibble;
            addInt16(crc16(m_buf_hex));
        }
    }
    void Packet::addInt16(const int16_t val)
    {

        AddByte(val);
        AddByte(val >> 8);
    }
    void Packet::AddByte(const unsigned char val)
    {
        /* To data section */
        m_buf_hex += (val & 0xff);
        m_bufSize++;
    }

    void Packet::AddTime(const tm *time)
    {
        addInt16(time->tm_hour);
        addInt16(time->tm_min);
        addInt16(time->tm_sec);
    }
    std::unique_ptr<tm *> Packet::GetTime(unsigned char *buff) const
    {
        if (buff == nullptr)
        {
            auto data = GetData();
            buff = (unsigned char *)data.substr(0, data.length()).c_str();
        }

        uint16_t hour = int16(buff[0], buff[1]);
        uint16_t min = int16(buff[2], buff[3]);
        uint16_t sec = int16(buff[4], buff[5]);

        auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        tm *gmtm = gmtime(&timenow);
        // std::unique_ptr<tm *> ptrGmtm =

        gmtm->tm_hour = hour;
        gmtm->tm_min = min;
        gmtm->tm_sec = sec;

        return std::make_unique<tm *>(gmtm);
    }
}; // namespace tello_protocol
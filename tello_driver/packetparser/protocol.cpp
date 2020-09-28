#include "protocol.hpp"

namespace tello_protocol
{
    Packet::Packet(const std::string &cmd, const unsigned char pkt_type, const unsigned char *payoads)
    {

        m_bufSize = cmd.length();
        m_buf = new unsigned char[m_bufSize]();
        std::copy(cmd.begin(), cmd.end(), m_buf);
    }
    Packet::Packet(unsigned char *cmd, const unsigned char pkt_type, const unsigned char *payoad)
    {
        // TODO: calculate buff size;
        m_bufSize = -1;
        m_buf = cmd;
    }
    Packet::Packet(const int cmd, const unsigned char pkt_type, const unsigned char *payoad)
    {
        m_bufSize = 9;
        m_buf = new unsigned char[m_bufSize]();
        m_buf[0] = tello_protocol::START_OF_PACKET;
        m_buf[1] = 0;
        m_buf[2] = 0;
        m_buf[3] = 0;
        m_buf[4] = pkt_type;
        m_buf[5] = (cmd & 0xff);
        m_buf[6] = ((cmd >> 8) & 0xff);
        m_buf[7] = 0;
        m_buf[8] = 0;

        if(payoad != nullptr)
        // self.buf.extend(payload)
    }

    Packet::~Packet()
    {
        delete[] m_buf;
    }
    std::unique_ptr<unsigned char> Packet::GetData() const
    {
        // Data section start after 9 bytes
        auto ret = std::make_unique<unsigned char>();
        if (m_buf != nullptr)
        {
            memcpy(ret.get(), &m_buf[9], m_bufSize - 9);
            return ret;
        }
        return nullptr;
    }
    const unsigned char *Packet::GetBuffer() const
    {
        return m_buf;
    }
    const int Packet::GetBufferSize() const
    {
        return m_bufSize;
    }
    void Packet::Fixup(int seq_num)
    {

        if (m_buf[0] == tello_protocol::START_OF_PACKET)
        {
            auto byte = le16(m_bufSize + 2);
            m_buf[1] = byte.LeftNibble;
            m_buf[2] = byte.RightNibble;
            m_buf[1] = (m_buf[1] << 3);
            m_buf[3] = crc8(m_buf, 4); //From original packet_processor [python]: buf[0:3]
            byte = le16(seq_num);
            m_buf[7] = byte.LeftNibble;
            m_buf[8] = byte.RightNibble;
            addInt16(crc16(m_buf, m_bufSize), m_bufSize + 1);
        }
    }
    void Packet::addInt16(const int16_t val, const int index)
    {
        AddByte(val, index);
        AddByte(val >> 8, index + 1);
    }
    void Packet::AddByte(const unsigned char val, const int index)
    {
        m_buf[index] = (val & 0xff);
    }

}; // namespace tello_protocol
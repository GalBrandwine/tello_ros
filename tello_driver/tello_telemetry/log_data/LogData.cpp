#include "log_data/LogData.hpp"
namespace tello_protocol
{
    void LogData::Update(const std::string &data)
    {
        /* 
          def update(self, data):
        if isinstance(data, bytearray):
            data = str(data)
        */
        // m_logger->info("LogData:  data length={}, data:{}", data.size(), spdlog::to_hex(data));
        m_count++;
        short pos = 0;
        while (pos < data.size() - 2)
        {
            // m_logger->info("pos: {}",pos);
            /* 
             if (struct.unpack_from('B', data, pos+0)[0] != 0x55):
                raise Exception('LogData: corrupted data at pos=%d, data=%s'
                               % (pos, byte_to_hexstring(data[pos:])))
            */
            if (data[pos + 0] != 0x55) // Little endian
            {
                m_logger->error("LogData: corrupted data at pos={}, data={}", pos, spdlog::to_hex(data.substr(pos)));
                return;
            }

            /* 
            length = struct.unpack_from('<h', data, pos+1)[0]
            checksum = data[pos+3]
            id = struct.unpack_from('<H', data, pos+4)[0]
            # 4bytes data[6:9] is tick
            # last 2 bytes are CRC
            # length-12 is the byte length of payload
            xorval = data[pos+6]
             */

            // LOG DATA: id= 2048, length -12 = 120 . length-12 is the byte length of payload
            // LOG DATA: id=   29, length - 12=  80
            short length = int16(data[pos + 1],data[pos + 2]);
            auto checksum = data[pos + 3];
            unsigned short id = uint16(data[pos + 4],data[pos + 5]);
            unsigned char xorval = data[pos + 6];
            //auto payload = bytearray([x ^ xorval for x in data[pos+10:pos+10+length-12]])

            /* 
            For ID 29: bytearray(b'.\x01\x00\x00\x00\x00\x00\x00\x7f\xb9\x83<K"D<\xe0\x9e\x9d\xbb\xbe7\x865\x00\x00\x00\x00\x00\x00\x80?\xbe7\x865\x00\x00\x00\x00\xbe7\x865\xbe7\x865\x00\x00\x00\x00\xa2}\x9b\xbc\xbe7\x865\x00\x98\x0b<\xbe7\x865\xcd\xcc\xcc\xbd\x0b\xd7\xa3<\x08\x00\x00\n')
            */

            auto payload = data.substr(pos + 10, pos + 10 + length - 12);
            for (auto &i : payload)
            {
                i ^= xorval;
            }
            // m_logger->info("payload: {}", spdlog::to_hex(payload));

            if (id == ID_NEW_MVO_FEEDBACK)
            {
                m_LogMvoFeedback.Update(payload, m_count);
            }
            else if (id == ID_IMU_ATTI)
            {
                // m_logger->info("\n\nData is IMU_ATTI\n\n");
                // m_LogImuFeedback.Update(payload, m_count);
            }
            else
            {
                for (const auto &unknownID : m_UnknownIDs)
                {
                    if (id != unknownID)
                    {
                        m_logger->error("UNHANDLED LOG DATA: id={}, length={}",id, length - 12);
                        m_UnknownIDs.push_back(id);
                    }
                    
                }
                
                /* 
                not id in self.unknowns : self.log.info('LogData: UNHANDLED LOG DATA: id=%5d, length=%4d' % (id, length - 12))
                self.unknowns.append(id)
                */
            }

            pos += length;
        }

        /*

            if isinstance(data, str):
                payload = bytearray([ord(x) ^ ord(xorval) for x in data[pos+10:pos+10+length-12]])
            else:
                payload = bytearray([x ^ xorval for x in data[pos+10:pos+10+length-12]])
            if id == self.ID_NEW_MVO_FEEDBACK:
                self.mvo.update(payload, self.count)
            elif id == self.ID_IMU_ATTI:
                self.imu.update(payload, self.count)
            else:
                if not id in self.unknowns:
                    self.log.info('LogData: UNHANDLED LOG DATA: id=%5d, length=%4d' % (id, length-12))
                    self.unknowns.append(id)

            pos += length
        if pos != len(data) - 2:
            raise Exception('LogData: corrupted data at pos=%d, data=%s'
                            % (pos, byte_to_hexstring(data[pos:])))
         */
    }
    LogData::LogData(std::shared_ptr<spdlog::logger> logger)
        : m_logger(logger),
          m_LogMvoFeedback(logger)
    {
    }

    LogData::~LogData()
    {
    }
} // namespace tello_protocol

#pragma once
#include "protocol.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tello_protocol
{
    class LogData
    {
    public:
        LogData(std::shared_ptr<spdlog::logger>);
        ~LogData();

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };

} // namespace tello_protocol

/* 
 class LogData(object):
    ID_NEW_MVO_FEEDBACK                = 29
    ID_IMU_ATTI                        = 2048
    unknowns = []

    def __init__(self, log, data = None):
        self.log = log
        self.count = 0
        self.mvo = LogNewMvoFeedback(log)
        self.imu = LogImuAtti(log)
        if data:
            self.update(data)

    def __str__(self):
        return ('MVO: ' + str(self.mvo) +
                '|IMU: ' + str(self.imu) +
                "")

    def format_cvs(self):
        return (
            self.mvo.format_cvs() +
            ',' + self.imu.format_cvs() +
            "")

    def format_cvs_header(self):
        return (
            self.mvo.format_cvs_header() +
            ',' + self.imu.format_cvs_header() +
            "")

    def update(self, data):
        if isinstance(data, bytearray):
            data = str(data)

        self.log.debug('LogData: data length=%d' % len(data))
        self.count += 1
        pos = 0
        while (pos < len(data) - 2):
            if (struct.unpack_from('B', data, pos+0)[0] != 0x55):
                raise Exception('LogData: corrupted data at pos=%d, data=%s'
                               % (pos, byte_to_hexstring(data[pos:])))
            length = struct.unpack_from('<h', data, pos+1)[0]
            checksum = data[pos+3]
            id = struct.unpack_from('<H', data, pos+4)[0]
            # 4bytes data[6:9] is tick
            # last 2 bytes are CRC
            # length-12 is the byte length of payload
            xorval = data[pos+6]
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
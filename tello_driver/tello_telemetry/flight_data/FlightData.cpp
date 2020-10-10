#pragma once
#include "flight_data/FlightData.hpp"

namespace tello_protocol
{

    const double FlightData::GetFlightMode() const
    {
        return m_fly_mode;
    }

    bool FlightData::SetData(const std::string &data)
    {
        if (data.length() < 24)
        {
            if (m_logger)
                m_logger->warn("data length didnt match.");
            return false;
        }

        m_height = int16(data[0], data[1]);
        m_north_speed = int16(data[2], data[3]);
        m_east_speed = int16(data[4], data[5]);
        m_ground_speed = int16(data[6], data[7]);
        m_fly_time = int16(data[8], data[9]);
        m_imu_state = ((data[10] >> 0) & 0x1);
        m_pressure_state = ((data[10] >> 1) & 0x1);
        m_down_visual_state = ((data[10] >> 2) & 0x1);
        m_power_state = ((data[10] >> 3) & 0x1);
        m_battery_state = ((data[10] >> 4) & 0x1);
        m_gravity_state = ((data[10] >> 5) & 0x1);
        m_wind_state = ((data[10] >> 7) & 0x1);
        m_imu_calibration_state = data[11];
        m_battery_percentage = data[12];
        m_drone_battery_left = int16(data[13], data[14]);
        m_drone_fly_time_left = int16(data[15], data[16]);
        m_em_sky = ((data[17] >> 0) & 0x1);
        m_em_ground = ((data[17] >> 1) & 0x1);
        m_em_open = ((data[17] >> 2) & 0x1);
        m_drone_hover = ((data[17] >> 3) & 0x1);
        m_outage_recording = ((data[17] >> 4) & 0x1);
        m_battery_low = ((data[17] >> 5) & 0x1);
        m_battery_lower = ((data[17] >> 6) & 0x1);
        m_factory_mode = ((data[17] >> 7) & 0x1);
        m_fly_mode = data[18];
        m_throw_fly_timer = data[19];
        m_camera_state = data[20];
        m_electrical_machinery_state = data[21];
        m_front_in = ((data[22] >> 0) & 0x1);
        m_front_out = ((data[22] >> 1) & 0x1);
        m_front_lsc = ((data[22] >> 2) & 0x1);
        m_temperature_height = ((data[23] >> 0) & 0x1);

        if (m_logger)
        {
            m_logger->info("Binary : {}", spdlog::to_hex(data));
        }

        return true;
    };
    FlightData::FlightData(std::shared_ptr<spdlog::logger> logger)
        : m_logger(logger)
    {
    }
    FlightData::~FlightData(){};
} // namespace tello_protocol
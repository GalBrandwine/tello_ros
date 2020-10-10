#pragma once
#include <iostream>
#include "protocol.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace tello_protocol
{

    class FlightData
    {
    public:
        const double GetFlightMode() const;
        bool SetData(const std::string &data);

        FlightData(std::shared_ptr<spdlog::logger>);
        ~FlightData();
        friend std::ostream &operator<<(std::ostream &os, const FlightData &fd)
        {
            os << "ALT: " << fd.m_height << '\n'
               << "| SPD: " << fd.m_ground_speed << '\n'
               << "| BAT: " << fd.m_battery_percentage << '\n'
               << "| WIFI: " << fd.m_wifi_strength << '\n'
               << "| CAM state: " << fd.m_camera_state << '\n'
               << "| MODE: " << fd.m_fly_mode << '\n';
            return os;
        }

    private:
        std::shared_ptr<spdlog::logger> m_logger;
        double m_battery_low;
        double m_battery_lower;
        double m_battery_percentage;
        double m_battery_state;
        double m_camera_state;
        double m_down_visual_state;
        double m_drone_battery_left;
        double m_drone_fly_time_left;
        double m_drone_hover;
        double m_em_open;
        double m_em_sky;
        double m_em_ground;
        double m_east_speed;
        double m_electrical_machinery_state;
        double m_factory_mode;
        double m_fly_mode;
        double m_fly_speed;
        double m_fly_time;
        double m_front_in;
        double m_front_lsc;
        double m_front_out;
        double m_gravity_state;
        double m_ground_speed;
        double m_height;
        double m_imu_calibration_state;
        double m_imu_state;
        double m_light_strength;
        double m_north_speed;
        double m_outage_recording;
        double m_power_state;
        double m_pressure_state;
        double m_smart_video_exit_mode;
        double m_temperature_height;
        double m_throw_fly_timer;
        double m_wifi_disturb;
        double m_wifi_strength;
        double m_wind_state;
    };
} // namespace tello_protocol
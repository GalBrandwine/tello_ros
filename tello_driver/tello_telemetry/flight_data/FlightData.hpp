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
        const float GetAttLimit() const{return m_attitude_limit;};
        void SetAttLimit(const std::string &);

        const short GetAltLimit() { return m_alt_limit; };
        void SetAltLimit(const std::string &);
        
        void SetWifiStrength(short strength) { m_wifi_strength = strength; };
        const short GetWifiStrength() const { return m_wifi_strength; };
        
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
        double m_battery_low = -1;
        double m_battery_lower = -1;
        double m_battery_percentage = -1;
        double m_battery_state = -1;
        double m_camera_state = -1;
        double m_down_visual_state = -1;
        double m_drone_battery_left = -1;
        double m_drone_fly_time_left = -1;
        double m_drone_hover = -1;
        double m_em_open = -1;
        double m_em_sky = -1;
        double m_em_ground = -1;
        double m_east_speed = -1;
        double m_electrical_machinery_state = -1;
        double m_factory_mode = -1;
        double m_fly_mode = -1;
        double m_fly_speed = -1;
        double m_fly_time = -1;
        double m_front_in = -1;
        double m_front_lsc = -1;
        double m_front_out = -1;
        double m_gravity_state = -1;
        double m_ground_speed = -1;
        double m_height = -1;
        double m_imu_calibration_state = -1;
        double m_imu_state = -1;
        double m_light_strength = -1;
        double m_north_speed = -1;
        double m_outage_recording = -1;
        double m_power_state = -1;
        double m_pressure_state = -1;
        double m_smart_video_exit_mode = -1;
        double m_temperature_height = -1;
        double m_throw_fly_timer = -1;
        double m_wifi_disturb = -1;
        short m_wifi_strength = -1;
        short m_alt_limit = -1;
        double m_wind_state = -1;
        float m_attitude_limit = -1;
    };
} // namespace tello_protocol
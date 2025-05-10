#include "config/settings.hpp"

void Settings::validate()
{
    if (launches < 1)
    {
        throw std::invalid_argument("Launches should be greater than or equal to 1.");
    }

    StandardInfo standard_info;
    try
    {
        standard_info = StandardManager::get_standard_info(standard_type);
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Invalid standard.");
    }

    auto is_allowed_scheduler_type =
        std::find(
            standard_info.schedulers.begin(),
            standard_info.schedulers.end(),
            scheduler_type) != standard_info.schedulers.end();

    if (!is_allowed_scheduler_type)
    {
        throw std::invalid_argument("Invalid scheduler type.");
    }

    try
    {
        StandardManager::get_tti(tti_duration);
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Invalid TTI.");
    }

    try
    {
        StandardManager::get_channel_sync_interval(channel_sync_interval);
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Invalid channel sync interval.");
    }

    try
    {
        StandardManager::get_cqi_efficiency(base_cqi);
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Invalid CQI.");
    }

    try
    {
        StandardManager::get_rb_number_from_bandwidth(bs_config.get_bandwidth());
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("Bandwidth should be one of [1.4, 3, 5, 10, 15, 20] MHz.");
    }

    if (total_packet_count < 1)
    {
        throw std::invalid_argument("Packet count should be greater than or equal to 1.");
    }

    if (queue_count < 1)
    {
        throw std::invalid_argument("Queue count should be greater than or equal to 1.");
    }

    int queue_id = 0;
    for (auto &queue : queue_configs)
    {
        if (queue.get_packet_count() < 1){
            throw std::invalid_argument(
                "Queue #" + std::to_string(queue_id) +
                " has invalid packet count: " +
                std::to_string(queue.get_packet_count()) + " \n" +
                "Expected packet count: [1, +inf)");
        }

        if (queue.get_quant() <= epsilon)
        {
            throw std::invalid_argument(
                "Queue #" + std::to_string(queue_id) +
                " has invalid quant: " +
                std::to_string(queue.get_quant()) + " \n" +
                "Expected quant: (0, +inf)");
        }

        if (queue.get_time_lambda() + epsilon <= 0)
        {
            throw std::invalid_argument(
                "Queue #" + std::to_string(queue_id) +
                " has invalid time lambda: " +
                std::to_string(queue.get_quant()) + " \n" +
                "Expected time lambda: (0, +inf)");
        }

        ++queue_id;
    }

    if (bs_config.get_x() < 0 - epsilon ||
        bs_config.get_y() < 0 - epsilon ||
        bs_config.get_z() < 25 - epsilon || bs_config.get_z() > 50 + epsilon)
    {
        throw std::invalid_argument("Base station should be place in {0, 0, [25; 50]}.");
    }

    if (user_configs.size() < 1){
        throw std::invalid_argument("At least 1 user required.");
    }

    int user_id = 0;
    for (auto &user : user_configs)
    {
        double x = user.get_x();
        double y = user.get_y();
        double bs_to_ue_2d_distance = std::sqrt(x * x + y * y);
        if (bs_to_ue_2d_distance > BS_TO_UE_DISTANCE_MAX + epsilon ||
            bs_to_ue_2d_distance < BS_TO_UE_DISTANCE_MIN - epsilon ||
            user.get_z() < 1.5 || user.get_z() > 22.5)
        {
            throw std::invalid_argument(
                "User #" + std::to_string(user_id) +
                " is out of bounds: (" +
                std::to_string(user.get_x()) + ", " +
                std::to_string(user.get_y()) + ", " +
                std::to_string(user.get_z()) + "). " +
                "\nExpected range: " +
                "2D distance ∈ [1000, 20000] m, " +
                "z ≈ 25 m");
        }

        if (user.get_speed() < -epsilon)
        {
            throw std::invalid_argument(
                "User #" + std::to_string(user_id) +
                " has invalid speed " +
                std::to_string(user.get_speed()) +
                "\nExpected range: " +
                "speed ∈ [0, 100] km/h");
        }

        if (user.get_direction() != "forward" && user.get_direction() != "backward" &&
            user.get_direction() != "left" && user.get_direction() != "right" &&
            user.get_direction() != "random")
        {
            throw std::invalid_argument(
                "User #" + std::to_string(user_id) +
                " has invalid mobility direction: " +
                user.get_direction() + " \n" +
                "Expected direction: \"forward\", \"backward\", " +
                "\"left\", \"right\", \"random\"");
        }

        if (user.get_quant() <= epsilon)
        {
            throw std::invalid_argument(
                "User #" + std::to_string(user_id) +
                " has invalid quant: " +
                std::to_string(user.get_quant()) + " \n" +
                "Expected quant: (0, +inf)");
        }

        if (user.get_packet_size() < 1 || 
            user.get_packet_size() > get_packet_size_limit())
        {
            throw std::invalid_argument(
                "User #" + std::to_string(user_id) +
                " has invalid packet size: " +
                std::to_string(user.get_packet_size()) + " \n" +
                "Expected packet size: [1, " + 
                std::to_string(get_packet_size_limit()) + 
                "] bytes). Maximum defined by current bandwidth and base CQI."
            );
        }

        if (user.get_traffic_part() <= epsilon)
        {
            throw std::invalid_argument(
                "User #" + std::to_string(user_id) +
                " has invalid traffic_part: " +
                std::to_string(user.get_traffic_part()) + " \n" +
                "Expected traffic part: (0, +inf)");
        }

        ++user_id;
    }

    if (throughput_history_size < 0 || throughput_history_size > 10000)
    {
        throw std::invalid_argument("Invalid throughput history size. Allowed [0, 10000]");
    }

    auto is_allowed_area_type =
        std::find(
            standard_info.area_types.begin(),
            standard_info.area_types.end(),
            area_type) != standard_info.area_types.end();

    if (!is_allowed_area_type)
    {
        throw std::invalid_argument("Invalid area type.");
    }

    double carrier_frequency = bs_config.get_carrier_frequency();
    if (carrier_frequency < 700 - epsilon || carrier_frequency > 3000 + epsilon)
    {
        throw std::invalid_argument(
            "Carrier frequency should be in range [700, 3000] MHz.");
    }

    int bs_tx_power = bs_config.get_transmission_power();
    if (bs_tx_power != 43 && bs_tx_power != 46 && bs_tx_power != 49)
    {
        throw std::invalid_argument("BS transmission power should be in range (43, 46, 49) dB.");
    }

    auto is_allowed_users_limit =
        std::find(
            standard_info.users_per_tti_limits.begin(),
            standard_info.users_per_tti_limits.end(),
            users_per_tti_limit) != standard_info.users_per_tti_limits.end();

    if (!is_allowed_users_limit)
    {
        throw std::invalid_argument("Invalid users limit, allowed (4, 8).");
    }

    if (fairness_history_size < 1 || fairness_history_size > 1000){
        throw std::invalid_argument("Invalid fairness history size, allowed [1, 1000].");
    }
}

void Settings::validate_scheduler_specific_parameters()
{
    if (scheduler_type == "DefaultPFScheduler")
    {
        if (base_cqi != 1)
        {
            throw std::invalid_argument("Allowed base CQI value for PF scheduler is 1.");
        }

        if (queue_count != 1)
        {
            throw std::invalid_argument("Allowed queue count for PF scheduler is 1.");
        }
    }

    if (scheduler_type == "DefaultRRScheduler" ||
        scheduler_type == "FixedDRRScheduler" ||
        scheduler_type == "FixedDRRSchedulerWithUserQuant" ||
        scheduler_type == "CyclicDRRScheduler" ||
        scheduler_type == "CyclicDRRSchedulerWithUserQuant" ||
        scheduler_type == "DefaultDRRScheduler" ||
        scheduler_type == "DefaultDRRSchedulerWithUserQuant")
    {
        if (base_cqi < 1 || base_cqi > 15)
        {
            throw std::invalid_argument("Allowed base CQI value for RR-based scheduler is [1, 15].");
        }
    }
}
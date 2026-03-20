#include <stdexcept>
#include <iostream>

struct SpeedUpdate
{
    double velocity_mps;
};
struct CarDetected
{
    double distance_m;
    double velocity_mps;
};
struct BrakeCommand
{
    double time_to_collision_s;
};

struct ServiceBus{
    void publish(const BrakeCommand&){
        std::cout << "publish BrakeCommand in ServiceBus..." << std::endl;
    }
};

template <typename T>
struct AutoBrake{
    AutoBrake(const T& publish): collision_treshold_s{ 5 }, speed_mps{}, m_publish{ publish }{}

    void observe(const SpeedUpdate& cd){
        speed_mps = cd.velocity_mps;
    }
    void observe(const CarDetected& cd){
        const auto relative_velocity_mps = speed_mps - cd.velocity_mps;
        const auto time_to_collision_s = cd.distance_m/relative_velocity_mps;
        if(time_to_collision_s > 0 && time_to_collision_s <= collision_treshold_s){
            m_publish(BrakeCommand{ time_to_collision_s });
        }
    }

    void set_collision_treshold_s(double x){
        if(x < 1) throw std::invalid_argument{ "Collision less than 1." }; 
        collision_treshold_s = x; 
    }
    double get_collision_treshold_s() const { return collision_treshold_s; }
    double get_speed_mps() const { return speed_mps; }
    
    private:
    double collision_treshold_s;
    double speed_mps;
    const T& m_publish;
};

constexpr void assert_that(bool statement, const char* message){
    if(!statement) throw std::runtime_error{ message };
}
void run_test(void(*unit_test)(), const char* name){
    try{
        unit_test();
        printf("[+] Test %s successful.\n", name);
    } catch(const std::exception& e){
        printf("[-] Test failure in %s. %s\n", name, e.what());
    }
}


void initial_speed_is_zero(){
    AutoBrake auto_brake{ [](const BrakeCommand&){} };
    assert_that(auto_brake.get_speed_mps() == 0L, "speed not equal 0");
}
void initial_sensitivity_is_five(){
    AutoBrake auto_brake{ [](const BrakeCommand&){} };
    assert_that(auto_brake.get_collision_treshold_s() == 5L, "sensitivity is not 5");
}
void sensitivity_greater_than_1(){
    AutoBrake auto_brake{ [](const BrakeCommand&){} };
    try {
        auto_brake.set_collision_treshold_s(0.5L);
    } catch(const std::exception&){
        return;
    }
    assert_that(false, "no_execption thrown");
}
void speed_is_saved(){
    AutoBrake auto_brake{ [](const BrakeCommand&){} };
    auto_brake.observe(SpeedUpdate{ 100L });
    assert_that(100L == auto_brake.get_speed_mps(), "speed not saved to 100");
    auto_brake.observe(SpeedUpdate{ 50L });
    assert_that(50L == auto_brake.get_speed_mps(), "speed not saved to 50");
    auto_brake.observe(SpeedUpdate{ 0L });
    assert_that(0L == auto_brake.get_speed_mps(), "speed not saved to 0");
}
void alert_when_iminent(){
    int brake_command_published{};
    auto arg = [&brake_command_published](const BrakeCommand& cmd) {
            brake_command_published++;
    };
    AutoBrake auto_brake(arg);
    /*AutoBrake auto_brake{
        [&brake_command_published](const BrakeCommand& cmd) {
            brake_command_published++;
        }
    };*/
    auto_brake.set_collision_treshold_s(10L);
    auto_brake.observe(SpeedUpdate{ 100L });
    auto_brake.observe(CarDetected{ 100L, 0L });
    //std::cout << "brake_command_published = " << brake_command_published << std::endl;
    assert_that(brake_command_published == 1, "brake commands published not one");
}
void no_alert_when_iminent(){
    int brake_command_published{};
    AutoBrake auto_brake{
        [&brake_command_published](const BrakeCommand&) {
            brake_command_published++;
        }
    };
    auto_brake.set_collision_treshold_s(2L);
    auto_brake.observe(SpeedUpdate{ 100L });
    auto_brake.observe(CarDetected{ 1000L, 50L });
    assert_that(brake_command_published == 0, "brake commands published");
}

int main(){
    run_test(initial_speed_is_zero, "initial speed is 0");
    run_test(initial_sensitivity_is_five, "initial sensitivity is 5");
    run_test(sensitivity_greater_than_1, "sensitivity greater than 1");
    run_test(speed_is_saved, "speed is saved");
    run_test(alert_when_iminent, "alert when imminent");
    run_test(no_alert_when_iminent, "alert when not imminent");
    //assert_that(1 + 2 > 2, "Something is profoundly wrong with the universe");
    //assert_that(24 == 43, "This assertion will generate an exception");
    /*ServiceBus bus;
    AutoBrake auto_brake{ [&bus](const auto& cmd){ bus.publish(cmd); } };
    while (true)
    {
        auto_brake.observe(SpeedUpdate{ 10L });
        auto_brake.observe(CarDetected{ 250L, 25L });
    }*/
   return 0;
}
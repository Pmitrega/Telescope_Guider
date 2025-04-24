#include <utility>
#include <chrono>

class motorMonitor{
public:
    void resetMotorsPosition();
    double getPositionRa();
    double getPositonDec();
    void updateRaSpeed(double speed);
    void updateDecSpeed(double speed);
    void updateRaPosition(double t_diff);
    void updateDecPosition(double t_diff);
private:
    // 144 steps per minute is 15'/s. It means: 15*60 = 900 = 144 * steps_to_deg/3600
    const double steps_to_deg = 900/144/3600;
    double ra_motor_steps = 0;
    double dec_motor_steps = 0;

    double dec_motor_speed = 0;
    double ra_motor_speed = 0;

};
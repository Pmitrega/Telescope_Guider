#include <utility>
#include <chrono>

class motorMonitor{
public:
    void resetMotorsPosition();
    double getPositionRa();
    double getPositionDec();
    void updateRaSpeed(double speed);
    void updateDecSpeed(double speed);
    void updateRaPosition(double t_diff);
    void updateDecPosition(double t_diff);
private:
<<<<<<< Updated upstream
    // 144 steps per minute is 15'/s. It means: 15*60 = 900 = 144 * steps_to_deg/3600
    const double steps_to_deg = 900.0/144.0/3600.0;
=======
    // 144 steps per minute is 15'/s. It means: 15*60 = 900 = 144 * steps_to_deg*3600
    const double steps_to_deg = 900/144/3600;
>>>>>>> Stashed changes
    double ra_motor_steps = 0;
    double dec_motor_steps = 0;

    double dec_motor_speed = 0;
    double ra_motor_speed = 0;

};
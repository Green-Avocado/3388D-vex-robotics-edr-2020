#include "main.h"
#include <cmath>
using namespace okapi;

//drive declaration
/*
okapi::MotorGroup driveLeft({3_mtr, 4_mtr});
okapi::MotorGroup driveRight({8_rmtr, 9_rmtr});
auto drive = ChassisControllerFactory::create(driveLeft, driveRight);
*/

//motors declaration
pros::Motor driveLeft1(3, 0);
pros::Motor driveLeft2(4, 0);
pros::Motor driveRight1(8, 1);
pros::Motor driveRight2(9, 1);
pros::Motor arm(10, 0);
pros::Motor intakeLeft(1, 1);
pros::Motor intakeRight(2, 0);
pros::Motor tray(6, 1);

//controller declarations
//Controller masterController;
pros::Controller master (CONTROLLER_MASTER);

ControllerButton trayUpButton(ControllerDigital::L1);
ControllerButton trayDownButton(ControllerDigital::L2);

ControllerButton armUpButton(ControllerDigital::R1);
ControllerButton armDownButton(ControllerDigital::R2);

//constants
#define driveSpeed 0.8
#define armSpeed 0.75
#define intakeSpeed 1
#define traySpeed 0.7
#define replayFrames 150

//replay memory
int driveLeftArr[replayFrames];
int driveRightArr[replayFrames];
int armArr[replayFrames];
int intakeArr[replayFrames];
int trayArr[replayFrames];

//motor functions
void Fdrive(int x, int y) {
    if(abs(y + x) > 3)
    {
        driveLeft1.move((y + x) / 2 * driveSpeed);
        driveLeft2.move((y + x) / 2 * driveSpeed);
        driveRight1.move((y - x) / 2 * driveSpeed);
        driveRight2.move((y - x) / 2 * driveSpeed);
    }
    else
    {
        driveLeft1.move(0);
        driveLeft2.move(0);
        driveRight1.move(0);
        driveRight2.move(0);
    }
}

void Farm(bool x, bool y) {
    if(x)
    {
        arm.move(127 * armSpeed);
    }
    else if(y)
    {
        arm.move(-127 * armSpeed);
    }
    else
    {
        arm.move(0);
    }
}

void Fintake(int x) {
    if(abs(x) > 3)
    {
        intakeLeft.move(x * intakeSpeed);
        intakeRight.move(x * intakeSpeed);
    }
    else
    {
        intakeLeft.move(0);
        intakeRight.move(0);
    }
}

void Ftray(bool x, bool y) {
    if(x)
    {
        tray.move(127 * traySpeed);
    }
    else if(y)
    {
        tray.move(-127 * traySpeed);
    }
    else
    {
        tray.move(0);
    }
}

void replay() {
    for(int i; i < replayFrames; i++) {
    }
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();

    //driveLeft.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
    //driveRight.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);

    arm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    intakeLeft.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    intakeRight.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    tray.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	while (true) {
        Fdrive(master.get_analog(ANALOG_LEFT_Y), master.get_analog(ANALOG_LEFT_X));
        Farm(armUpButton.isPressed(), armDownButton.isPressed());
        Fintake(master.get_analog(ANALOG_RIGHT_Y));
        Ftray(trayUpButton.isPressed(), trayDownButton.isPressed());

        /* OLD CONTROLS
        //drivetrain arcade movement
        if(abs(master.get_analog(ANALOG_LEFT_Y) + master.get_analog(ANALOG_LEFT_X)) > 3)
        {
            drive.arcade(masterController.getAnalog(ControllerAnalog::leftY), masterController.getAnalog(ControllerAnalog::leftX)*driveSpeed);
        }
        else
        {
            drive.stop();
        }
        
        //arm movement
        if(armUpButton.isPressed())
        {
            arm.move(127*armSpeed);
        }
        else if(armDownButton.isPressed())
        {
            arm.move(-127*armSpeed);
        }
        else
        {
            arm.move(0);
        }
        
        //intake control
        if(abs(master.get_analog(ANALOG_RIGHT_Y)) > 3)
        {
            intakeLeft.move(master.get_analog(ANALOG_RIGHT_Y)*intakeSpeed);
            intakeRight.move(master.get_analog(ANALOG_RIGHT_Y)*intakeSpeed);
        }
        else
        {
            intakeLeft.move(0);
            intakeRight.move(0);
        }
        
        //tray movement
        if(trayUpButton.isPressed())
        {
            tray.move(127*traySpeed);
        }
        else if(trayDownButton.isPressed())
        {
            tray.move(-127*traySpeed);
        }
        else
        {
            tray.move(0);
        }
        */
        
        pros::delay(20);
	}
}


#include "main.h"
#include <cmath>
using namespace okapi;

okapi::MotorGroup driveLeft({3_mtr, 4_mtr});
okapi::MotorGroup driveRight({8_rmtr, 9_rmtr});
auto drive = ChassisControllerFactory::create(driveLeft, driveRight);

pros::Motor arm(10, 0);
pros::Motor intakeLeft(1, 1);
pros::Motor intakeRight(2, 0);
pros::Motor tray(6, 1);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();

    driveLeft.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);
    driveRight.setBrakeMode(okapi::AbstractMotor::brakeMode::hold);

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
	Controller masterController;
    pros::Controller master (CONTROLLER_MASTER);

    ControllerButton trayUpButton(ControllerDigital::L1);
    ControllerButton trayDownButton(ControllerDigital::L2);

    ControllerButton armUpButton(ControllerDigital::R1);
    ControllerButton armDownButton(ControllerDigital::R2);

	while (true) {
        //drivetrain arcade movement
        if(abs(master.get_analog(ANALOG_LEFT_Y) + master.get_analog(ANALOG_LEFT_X)) > 3)
        {
            drive.arcade(masterController.getAnalog(ControllerAnalog::leftY), masterController.getAnalog(ControllerAnalog::leftX));
        }
        else
        {
            drive.stop();
        }
        
        //arm movement
        if(armUpButton.isPressed())
        {
            arm.move(100);
        }
        else if(armDownButton.isPressed())
        {
            arm.move(-100);
        }
        else
        {
            arm.move(0);
        }
        
        //intake control
        intakeLeft.move(master.get_analog(ANALOG_RIGHT_Y));
        intakeRight.move(master.get_analog(ANALOG_RIGHT_Y));
        
        //tray movement
        if(trayUpButton.isPressed())
        {
            tray.move(63);
        }
        else if(trayDownButton.isPressed())
        {
            tray.move(-63);
        }
        else
        {
            tray.move(0);
        }
        
        pros::delay(20);
	}
}


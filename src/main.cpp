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

//ControllerButton recordButton(ControllerDigital::A);
//ControllerButton replayButton(ControllerDigital::B);
//ControllerButton writeButton(ControllerDigital::X);
//ControllerButton readButton(ControllerDigital::Y);

ControllerButton menuUp(ControllerDigital::up);
ControllerButton menuDown(ControllerDigital::down);
ControllerButton menuForward(ControllerDigital::A);
ControllerButton menuBack(ControllerDigital::B);

//constants
#define driveSpeed 0.8
#define armSpeed 0.75
#define intakeSpeed 1
#define traySpeed 0.7
#define minFrames 750
#define maxFrames 3000
#define textDuration 200

//user interface
int menuSelection = 0;
int menuLevel = 0;
int replaySlot = 0;

//replay memory
char filename[] = "/usd/rec0.txt";
int replayFrames = minFrames;
int replayInterval = 20;
int framesToRecord = minFrames;
int intervalToRecord = 20;
int driveX[maxFrames];
int driveY[maxFrames];
int armX[maxFrames];
int intakeX[maxFrames];
int trayX[maxFrames];

void menuPrint(int line, int selection) {
    switch(selection)
    {
        case 0:
            master.set_text(line, 2, "Read");
            break;

        case 1:
            master.set_text(line, 2, "Write");
            break;

        case 2:
            master.set_text(line, 2, "Record");
            break;

        case 3:
            master.set_text(line, 2, "Replay");
            break;

        default:
            master.set_text(line, 2, "ERROR");
            break;
    }
}

void replayPrint(int line, int selection) {
    master.print(line, 2, "Slot %d", selection);
}

void menuChange(int change) {
    master.clear();
    master.set_text(2, 0, ">");
    if(menuLevel == 0)
    {
        menuSelection += change;
        menuSelection = menuSelection % 4;
        for(int i = -1; i < 2; i++)
        {
            menuPrint(i + 1, (menuSelection + i) % 4);
        }
    }
    else if(menuLevel == 1)
    {
        replaySlot += change;
        replaySlot = replaySlot % 10;
        for(int i = -1; i < 2; i++)
        {
            replayPrint(i + 1, (replaySlot + i) % 10);
        }
    }
}

//write file
void writeSD() {
    master.set_text(0, 0, "Writing SD");
    FILE* usd_file_write = fopen(filename, "w");
    fprintf(usd_file_write, "%d %d\n", replayFrames, replayInterval);
    for(int i = 0; i < replayFrames; i++)
    {
        fprintf(usd_file_write, "%d ", *(driveX + i));
    }
    fprintf(usd_file_write, "\n");
    for(int i = 0; i < replayFrames; i++)
    {
        fprintf(usd_file_write, "%d ", *(driveY + i));
    }
    fprintf(usd_file_write, "\n");
    for(int i = 0; i < replayFrames; i++)
    {
        fprintf(usd_file_write, "%d ", *(armX + i));
    }
    fprintf(usd_file_write, "\n");
    for(int i = 0; i < replayFrames; i++)
    {
        fprintf(usd_file_write, "%d ", *(intakeX + i));
    }
    fprintf(usd_file_write, "\n");
    for(int i = 0; i < replayFrames; i++)
    {
        fprintf(usd_file_write, "%d ", *(trayX + i));
    }
    fclose(usd_file_write);
    master.set_text(1, 0, "Done");
    pros::delay(textDuration);
    menuLevel = 0;
    menuChange(0);
}

//read file
void readSD() {
    master.set_text(0, 0, "Reading SD");
    FILE* usd_file_read = fopen(filename, "r");
    fscanf(usd_file_read, "%d%d", &replayFrames, &replayInterval);
    for(int i = 0; i < replayFrames; i++)
    {
        fscanf(usd_file_read, "%d", driveX + i);
    }
    for(int i = 0; i < replayFrames; i++)
    {
        fscanf(usd_file_read, "%d", driveY + i);
    }
    for(int i = 0; i < replayFrames; i++)
    {
        fscanf(usd_file_read, "%d", armX + i);
    }
    for(int i = 0; i < replayFrames; i++)
    {
        fscanf(usd_file_read, "%d", intakeX + i);
    }
    for(int i = 0; i < replayFrames; i++)
    {
        fscanf(usd_file_read, "%d", trayX + i);
    }
    fclose(usd_file_read);
    master.set_text(1, 0, "Done");
    pros::delay(textDuration);
    menuLevel = 0;
    menuChange(0);
}

//interpret button presses
int button_to_int(bool x, bool y) {
    if(x)
    {
        return 127;
    }
    else if(y)
    {
        return -127;
    }
    else
    {
        return 0;
    }
}

//motor functions
void Fdrive(int x, int y) {
    if(abs(y + x) > 3)
    {
        driveLeft1.move((y + x) / 2);
        driveLeft2.move((y + x) / 2);
        driveRight1.move((y - x) / 2);
        driveRight2.move((y - x) / 2);
    }
    else
    {
        driveLeft1.move(0);
        driveLeft2.move(0);
        driveRight1.move(0);
        driveRight2.move(0);
    }
}

void Farm(int x) {
    arm.move(x);
}

void Fintake(int x) {
    if(abs(x) > 3)
    {
        intakeLeft.move(x);
        intakeRight.move(x);
    }
    else
    {
        intakeLeft.move(0);
        intakeRight.move(0);
    }
}

void Ftray(int x) {
    tray.move(x);
}

void record() {
    master.set_text(0, 0, "Recording");
    replayFrames = framesToRecord;
    for(int i = 0; i < replayFrames; i++)
    {
        int Xint;
        int Yint;

        Xint = master.get_analog(ANALOG_LEFT_X) * driveSpeed;
        Yint = master.get_analog(ANALOG_LEFT_Y) * driveSpeed;
        Fdrive(Xint, Yint);
        driveX[i] = Xint;
        driveY[i] = Yint;

        Xint = button_to_int(armUpButton.isPressed(), armDownButton.isPressed()) * armSpeed;
        Farm(Xint);
        armX[i] = Xint;

        Xint = master.get_analog(ANALOG_RIGHT_Y) * intakeSpeed;
        Fintake(Xint);
        intakeX[i] = Xint;

        Xint = button_to_int(trayUpButton.isPressed(), trayDownButton.isPressed()) * traySpeed;
        Ftray(Xint);
        trayX[i] = Xint;

        pros::delay(replayInterval);
    }
    master.set_text(1, 0, "Done");
    pros::delay(textDuration);
    menuLevel = 0;
    menuChange(0);
}

void replay() {
    master.set_text(0, 0, "Running Auton");
    for(int i = 0; i < replayFrames; i++) {
        Fdrive(driveX[i], driveY[i]);
        Farm(armX[i]);
        Fintake(intakeX[i]);
        Ftray(trayX[i]);
        pros::delay(replayInterval);
    }
    master.set_text(1, 0, "Done");
    pros::delay(textDuration);
    menuLevel = 0;
    menuChange(0);
}

void levelChange(int change) {
    if(change == -1)
    {
        if(menuLevel > 0)
        {
            menuLevel += change;
            menuChange(0);
        }
    }
    else
    {
        if(menuLevel == 1)
        {
            sprintf(filename, "/usd/rec%d.txt", replaySlot);
            if(menuSelection == 0)
            {
                readSD();
            }
            else
            {
                writeSD();
            }
        }
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
    for(int i = 0; i < replayFrames; i++) {
      driveX[i] = 0;
      driveY[i] = 0;
      armX[i] = 0;
      intakeX[i] = 0;
      trayX[i] = 0;
    }

    arm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    intakeLeft.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    intakeRight.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    tray.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    menuChange(0);

    //readSD();
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
void autonomous() {
    replay();
}

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
    bool menuUpNew = true;
    bool menuDownNew = true;
    bool menuForwardNew = true;
    bool menuBackNew = true;
	while (true) {
        Fdrive(master.get_analog(ANALOG_LEFT_X) * driveSpeed, master.get_analog(ANALOG_LEFT_Y) * driveSpeed);
        Farm(button_to_int(armUpButton.isPressed(), armDownButton.isPressed()) * armSpeed);
        Fintake(master.get_analog(ANALOG_RIGHT_Y) * intakeSpeed);
        Ftray(button_to_int(trayUpButton.isPressed(), trayDownButton.isPressed()) * traySpeed);

        //UI management
        if(menuUp.isPressed())
        {
            if(menuUpNew)
            {
                menuChange(1);
                menuUpNew = false;
            }
        }
        else menuUpNew = true;

        if(menuDown.isPressed())
        {
            if(menuDownNew)
            {
                menuChange(-1);
                menuDownNew = false;
            }
        }
        else menuDownNew = true;

        if(menuForward.isPressed())
        {
            if(menuForwardNew)
            {
                levelChange(1);
                menuForwardNew = false;
            }
        }
        else menuForwardNew = true;

        if(menuBack.isPressed())
        {
            if(menuBackNew)
            {
                levelChange(-1);
                menuBackNew = false;
            }
        }
        else menuBackNew = true;

        /*
        //record inputs
        if(recordButton.isPressed())
        {
            record();
        }

        //replay
        if(replayButton.isPressed())
        {
            replay();
        }

        //write
        if(writeButton.isPressed())
        {
            writeSD();
        }

        //read
        if(readButton.isPressed())
        {
            readSD();
        }
        */

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

#include "main.h"
#include <cmath>

//motors declarations
pros::Motor driveLeft1(3, 0);
pros::Motor driveLeft2(4, 0);
pros::Motor driveRight1(8, 1);
pros::Motor driveRight2(9, 1);
pros::Motor arm(10, 0);
pros::Motor intakeLeft(1, 1);
pros::Motor intakeRight(2, 0);
pros::Motor tray(6, 1);

//controller declarations
pros::Controller master (pros::E_CONTROLLER_MASTER);

okapi::ControllerButton trayUpButton(okapi::ControllerDigital::L1);
okapi::ControllerButton trayDownButton(okapi::ControllerDigital::L2);

okapi::ControllerButton armUpButton(okapi::ControllerDigital::R1);
okapi::ControllerButton armDownButton(okapi::ControllerDigital::R2);

okapi::ControllerButton menuUp(okapi::ControllerDigital::up);
okapi::ControllerButton menuDown(okapi::ControllerDigital::down);
okapi::ControllerButton menuForward(okapi::ControllerDigital::A);
okapi::ControllerButton menuBack(okapi::ControllerDigital::B);
okapi::ControllerButton valueInc(okapi::ControllerDigital::right);
okapi::ControllerButton valueDec(okapi::ControllerDigital::left);

//speed factors
#define driveSpeed 1
#define armSpeed 0.75
#define intakeSpeed 1
#define traySpeed 0.7

//recording settings
#define minFrames 750
#define maxFrames 3000

//display text
#define textDuration 200
#define textUpdateBuffer 60

//menu settings
#define menuItems 5
#define saveSlots 10

//user interface
int menuSelection = 0;
int menuLevel = 0;
int replaySlot = 0;
int settingsSlot = 0;
const char *menuItemNames[] = {
    "Read            ",
    "Write           ",
    "Record          ",
    "Replay          ",
    "Settings        ",
};

//replay memory
char filename[] = "/usd/rec0.txt";
char recordVersion = 'A';
char replayVersion = recordVersion;
int replayFrames = minFrames;
int replayInterval = 20;
int framesToRecord = replayFrames;
int intervalToRecord = 20;
int driveX[maxFrames];
int driveY[maxFrames];
int armX[maxFrames];
int intakeX[maxFrames];
int trayX[maxFrames];
int timerSec;
int timerTenth;
int timerMS;
int alignSpacing;

//screen functions
void lineClear(int line)
{
    pros::delay(textUpdateBuffer);
    master.set_text(line, 0, "                ");
    pros::delay(textUpdateBuffer);
}

void screenClear()
{
    lineClear(0);
    lineClear(1);
    lineClear(2);
}

//menu functions
void menuPrint(int line, int selection)
{
    pros::delay(textUpdateBuffer);
    master.print(line, 2, "%s", menuItemNames[selection]);
}

void replayPrint(int line, int selection)
{
    pros::delay(textUpdateBuffer);
    master.print(line, 2, "Slot %d", selection);
}

void menuChange(int change)
{
    screenClear();
    if(menuLevel == 0)
    {
        master.set_text(1, 0, ">");
        menuSelection += change;
        if(menuSelection < 0) menuSelection += menuItems;
        else if(menuSelection > menuItems - 1) menuSelection += -menuItems;
        for(int i = -1; i < 2; i++)
        {
            menuPrint(i + 1, (menuSelection + i + menuItems) % menuItems);
        }
    }
    else if(menuLevel == 1)
    {
        master.set_text(1, 0, ">");
        replaySlot += change;
        if(replaySlot < 0) replaySlot += saveSlots;
        else if(replaySlot > saveSlots - 1) replaySlot += -saveSlots;
        for(int i = -1; i < 2; i++)
        {
            replayPrint(i + 1, (replaySlot + i + saveSlots) % saveSlots);
        }
    }
    else if(menuLevel == 2)
    {
        pros::delay(textUpdateBuffer);
        master.print(0, 2, "Seconds: %d", framesToRecord*intervalToRecord/1000);
        pros::delay(textUpdateBuffer);
        master.print(1, 2, "Interval: %d", intervalToRecord);
        if(change != 0)
        {
            if(settingsSlot == 0) settingsSlot = 1;
            else settingsSlot = 0;
        }
        pros::delay(textUpdateBuffer);
        master.print(settingsSlot, 0, ">");
    }
}

//file functions
void writeSD()
{
    screenClear();
    master.set_text(0, 0, "Writing SD...");
    FILE* usd_file_write = fopen(filename, "w");
    fprintf(usd_file_write, "%c %d %d\n", replayVersion, replayFrames, replayInterval);
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
    pros::delay(textUpdateBuffer);
    master.set_text(2, 0, "Done");
    pros::delay(textDuration);
    menuLevel = 0;
    menuChange(0);
}

void readSD()
{
    screenClear();
    master.set_text(0, 0, "Reading SD...");
    FILE* usd_file_read = fopen(filename, "r");
    fscanf(usd_file_read, "%c%d%d", &replayVersion, &replayFrames, &replayInterval);
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
    pros::delay(textUpdateBuffer);
    master.set_text(2, 0, "Done");
    pros::delay(textDuration);
    menuLevel = 0;
    menuChange(0);
}

//interpret button presses
int button_to_int(bool x, bool y)
{
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
void Fdrive(int x, int y)
{
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

void Farm(int x)
{
    arm.move(x);
}

void Fintake(int x)
{
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

void Ftray(int x)
{
    tray.move(x);
}

//rerun functions
void updateTimers()
{
    timerMS -= replayInterval;
    timerSec = timerMS / 1000;
    timerTenth = (timerMS % 1000) / 100;
    alignSpacing = 6 - (timerSec / 10);
}

void record()
{
    screenClear();
    master.set_text(0, 0, "Recording...");
    pros::delay(textUpdateBuffer);

    replayFrames = framesToRecord;
    replayInterval = intervalToRecord;
    replayVersion = recordVersion;

    timerMS = replayFrames * replayInterval;
    updateTimers();
    master.print(1, alignSpacing, "%d.%d       ", timerSec, timerTenth);

    for(int i = 0; i < replayFrames; i++)
    {
        driveX[i] = master.get_analog(ANALOG_LEFT_X) * driveSpeed;
        driveY[i] = master.get_analog(ANALOG_LEFT_Y) * driveSpeed;
        Fdrive(driveX[i], driveY[i]);

        armX[i] = button_to_int(armUpButton.isPressed(), armDownButton.isPressed()) * armSpeed;
        Farm(armX[i]);

        intakeX[i] = master.get_analog(ANALOG_RIGHT_Y) * intakeSpeed;
        Fintake(intakeX[i]);

        trayX[i] = button_to_int(trayUpButton.isPressed(), trayDownButton.isPressed());
        Ftray(trayX[i]);

        updateTimers();
        if(timerTenth != ((timerMS + replayInterval) % 1000) / 100)
            master.print(1, alignSpacing, "%d.%d       ", timerSec, timerTenth);

        pros::delay(replayInterval);
    }
    pros::delay(textUpdateBuffer);
    master.set_text(2, 0, "Done");
    pros::delay(textDuration);
    menuLevel = 0;
    menuChange(0);
}

void replay()
{
    screenClear();
    master.set_text(0, 0, "Running");
    pros::delay(textUpdateBuffer);
    master.set_text(1, 0, "Autonomous...");

    timerMS = replayFrames * replayInterval;
    updateTimers();
    master.print(1, alignSpacing, "%d.%d       ", timerSec, timerTenth);

    if(replayVersion == 'A')
    {
        for(int i = 0; i < replayFrames; i++)
        {
            Fdrive(driveX[i], driveY[i]);
            Farm(armX[i]);
            Fintake(intakeX[i]);
            Ftray(trayX[i]);

            updateTimers();
            if(timerTenth != ((timerMS + replayInterval) % 1000) / 100)
                master.print(1, alignSpacing, "%d.%d       ", timerSec, timerTenth);

            pros::delay(replayInterval);
        }
    }
    pros::delay(textUpdateBuffer);
    master.set_text(2, 0, "Done");
    pros::delay(textDuration);
    menuLevel = 0;
    menuChange(0);
}

//UI management
void levelChange(int change)
{
    if(change == -1)
    {
        if(menuLevel > 0)
        {
            menuLevel = 0;
            menuChange(0);
        }
    }
    else
    {
        if(menuLevel == 0)
        {
            if(menuSelection == 0 || menuSelection == 1)
            {
                menuLevel = 1;
                menuChange(0);
            }
            else if(menuSelection == 2 || menuSelection == 3)
            {
                switch(menuSelection)
                {
                    case 2:
                        record();
                        break;

                    case 3:
                        replay();
                        break;
                }
            }
            else if(menuSelection == 4)
            {
                menuLevel = 2;
                settingsSlot = 0;
                menuChange(0);
            }
        }
        else if(menuLevel == 1)
        {
            sprintf(filename, "/usd/rec%d.txt", replaySlot);
            switch(menuSelection)
            {
                case 0:
                    readSD();
                    break;

                case 1:
                    writeSD();
                    break;
            }
        }
    }
}

void changeFramesByTime(int seconds)
{
    framesToRecord += seconds*1000/intervalToRecord;
    if(framesToRecord > maxFrames || framesToRecord < minFrames)
    {
        framesToRecord -= seconds*1000/intervalToRecord;
    }
}

void valueChange(int change)
{
    if(menuLevel == 2);
    {
        while(valueInc.isPressed() && change > 0 || valueDec.isPressed() && change < 0)
        {
            if(settingsSlot == 0) // if frame count is selected
            {
                if(change > 0)
                {
                    changeFramesByTime(15); // increment 15 seconds intervals, can be changed
                }
                else if(change < 0)
                {
                    changeFramesByTime(-15);
                }
            }
            else
            {
                if(intervalToRecord + 5 * change > 0)
                {
                    intervalToRecord += 5 * change; // 5 ms interval steps to ensure that frame count is always an int
                }
            }
            menuChange(0);
            pros::delay(200);
        }
    }
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	pros::lcd::initialize();

    for(int i = 0; i < maxFrames; i++)
    {
      driveX[i] = 0;
      driveY[i] = 0;
      armX[i] = 0;
      intakeX[i] = 0;
      trayX[i] = 0;
    }

    driveLeft1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    driveLeft2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    driveRight1.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    driveRight2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    arm.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    intakeLeft.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    intakeRight.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    tray.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    menuChange(0);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize()
{
}

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
void autonomous()
{
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
void opcontrol()
{
    bool menuUpNew = true;
    bool menuDownNew = true;
    bool menuForwardNew = true;
    bool menuBackNew = true;
    bool valueIncNew = true;
    bool valueDecNew = true;

	while (true)
    {
        //user controls
        Fdrive(master.get_analog(ANALOG_LEFT_X) * driveSpeed, master.get_analog(ANALOG_LEFT_Y) * driveSpeed);
        Farm(button_to_int(armUpButton.isPressed(), armDownButton.isPressed()) * armSpeed);
        Fintake(master.get_analog(ANALOG_RIGHT_Y) * intakeSpeed);
        Ftray(button_to_int(trayUpButton.isPressed(), trayDownButton.isPressed()) * traySpeed);

        //UI management
        if(menuUp.isPressed())
        {
            if(menuUpNew)
            {
                menuChange(-1);
                menuUpNew = false;
            }
        }
        else menuUpNew = true;

        if(menuDown.isPressed())
        {
            if(menuDownNew)
            {
                menuChange(1);
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

        if(valueInc.isPressed())
        {
            if(valueIncNew)
            {
                valueChange(1);
                valueIncNew = false;
            }
        }
        else valueIncNew = true;

        if(valueDec.isPressed())
        {
            if(valueDecNew)
            {
                valueChange(-1);
                valueDecNew = false;
            }
        }
        else valueDecNew = true;

        pros::delay(20);
	}
}

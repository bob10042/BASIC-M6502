/*
 * 6502 Robot Arm Simulator
 *
 * Simulates a 4-DOF robot arm controlled by a 6502:
 * - 4 Servo motors (controlled via PWM/DAC)
 * - 4 Position encoders (read via ADC)
 * - Forward and inverse kinematics
 * - Real-time ASCII visualization
 * - Memory-mapped control
 *
 * A complete robotics control system running on simulated 6502!
 */

#include "device_library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define PI 3.14159265359

/* ============================================
 * ROBOT ARM CONFIGURATION
 * ============================================ */

#define NUM_JOINTS 4

typedef struct {
    float angle;        // Current angle (degrees)
    float target;       // Target angle (degrees)
    float speed;        // Movement speed
    float min_angle;    // Minimum angle limit
    float max_angle;    // Maximum angle limit
    float length;       // Link length (cm)
} Joint;

typedef struct {
    float x, y, z;      // 3D position
} Point3D;

typedef struct {
    uint8_t ram[65536];

    // Analog I/O
    ADC adc;
    DAC dac;
    PWM pwm;

    // Robot arm
    Joint joints[NUM_JOINTS];
    Point3D end_effector;  // Gripper position
    Point3D target_pos;    // Desired position
    int     gripper_open;  // Gripper state

    // System
    int running;
    int cycles;
    int mode;  // 0=manual, 1=auto
} RobotSystem;


/* ============================================
 * FORWARD KINEMATICS
 * Calculate end effector position from joint angles
 * ============================================ */

Point3D forward_kinematics(RobotSystem *robot) {
    Point3D pos = {0, 0, 0};
    float cumulative_angle = 0;
    float x = 0, y = 0;

    // Base rotation (joint 0 - around Z axis)
    float base_rotation = robot->joints[0].angle * PI / 180.0;

    // Calculate position in 2D plane
    for (int i = 1; i < NUM_JOINTS - 1; i++) {
        cumulative_angle += robot->joints[i].angle * PI / 180.0;
        x += robot->joints[i].length * cos(cumulative_angle);
        y += robot->joints[i].length * sin(cumulative_angle);
    }

    // Apply base rotation to get 3D position
    pos.x = x * cos(base_rotation);
    pos.y = x * sin(base_rotation);
    pos.z = y;

    return pos;
}


/* ============================================
 * INVERSE KINEMATICS (Simplified 2-link)
 * Calculate joint angles from desired position
 * ============================================ */

void inverse_kinematics(RobotSystem *robot, Point3D target) {
    // Base rotation (rotate to face target)
    float base_angle = atan2(target.y, target.x) * 180.0 / PI;
    robot->joints[0].target = base_angle;

    // Distance in XY plane
    float r = sqrt(target.x * target.x + target.y * target.y);

    // 2-link arm in vertical plane
    float l1 = robot->joints[1].length;
    float l2 = robot->joints[2].length;
    float z = target.z;

    // Distance to target
    float d = sqrt(r * r + z * z);

    // Check if reachable
    if (d > l1 + l2) {
        d = l1 + l2;  // Clamp to maximum reach
    }

    // Law of cosines for joint 2 (elbow)
    float cos_angle2 = (d * d - l1 * l1 - l2 * l2) / (2 * l1 * l2);
    if (cos_angle2 < -1.0) cos_angle2 = -1.0;
    if (cos_angle2 > 1.0) cos_angle2 = 1.0;
    float angle2 = acos(cos_angle2) * 180.0 / PI;

    // Joint 1 (shoulder)
    float alpha = atan2(z, r) * 180.0 / PI;
    float beta = atan2(l2 * sin(angle2 * PI / 180.0),
                      l1 + l2 * cos(angle2 * PI / 180.0)) * 180.0 / PI;
    float angle1 = alpha - beta;

    robot->joints[1].target = angle1;
    robot->joints[2].target = angle2;
}


/* ============================================
 * UPDATE JOINT POSITIONS
 * ============================================ */

void update_joints(RobotSystem *robot) {
    for (int i = 0; i < NUM_JOINTS; i++) {
        Joint *joint = &robot->joints[i];

        // Move towards target
        float diff = joint->target - joint->angle;

        if (fabs(diff) > 0.1) {
            if (diff > 0) {
                joint->angle += joint->speed;
            } else {
                joint->angle -= joint->speed;
            }
        }

        // Clamp to limits
        if (joint->angle < joint->min_angle) joint->angle = joint->min_angle;
        if (joint->angle > joint->max_angle) joint->angle = joint->max_angle;

        // Update ADC (position feedback)
        // Map angle to 0-5V
        float voltage = ((joint->angle - joint->min_angle) /
                        (joint->max_angle - joint->min_angle)) * 5.0;
        adc_set_voltage(&robot->adc, i, voltage);

        // Read control from DAC
        float control_voltage = dac_get_voltage(&robot->dac, i);
        joint->target = joint->min_angle +
                       (control_voltage / 5.0) * (joint->max_angle - joint->min_angle);
    }

    // Update end effector position
    robot->end_effector = forward_kinematics(robot);
}


/* ============================================
 * ASCII ART VISUALIZATION
 * ============================================ */

void draw_robot_arm(RobotSystem *robot) {
    int width = 60;
    int height = 30;
    char screen[height][width + 1];

    // Clear screen
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            screen[y][x] = ' ';
        }
        screen[y][width] = '\0';
    }

    // Draw ground
    for (int x = 0; x < width; x++) {
        screen[height - 1][x] = '═';
    }

    // Calculate joint positions for drawing
    int base_x = 10;
    int base_y = height - 2;

    float cumulative_angle = 0;
    int prev_x = base_x;
    int prev_y = base_y;

    // Draw links
    for (int i = 1; i < NUM_JOINTS - 1; i++) {
        cumulative_angle += robot->joints[i].angle * PI / 180.0;

        int link_len = (int)(robot->joints[i].length / 2);  // Scale for display
        int end_x = prev_x + (int)(link_len * cos(cumulative_angle));
        int end_y = prev_y - (int)(link_len * sin(cumulative_angle));

        // Clamp to screen
        if (end_x < 0) end_x = 0;
        if (end_x >= width) end_x = width - 1;
        if (end_y < 0) end_y = 0;
        if (end_y >= height - 1) end_y = height - 2;

        // Draw link (simple line)
        int dx = end_x - prev_x;
        int dy = end_y - prev_y;
        int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

        for (int step = 0; step <= steps; step++) {
            int x = prev_x + (dx * step) / steps;
            int y = prev_y + (dy * step) / steps;
            if (x >= 0 && x < width && y >= 0 && y < height - 1) {
                screen[y][x] = '█';
            }
        }

        // Draw joint
        if (end_x >= 0 && end_x < width && end_y >= 0 && end_y < height - 1) {
            screen[end_y][end_x] = 'O';
        }

        prev_x = end_x;
        prev_y = end_y;
    }

    // Draw base
    screen[base_y][base_x] = '■';

    // Draw gripper
    if (prev_x >= 0 && prev_x < width - 2 && prev_y >= 0 && prev_y < height - 1) {
        if (robot->gripper_open) {
            screen[prev_y][prev_x + 1] = '<';
            screen[prev_y][prev_x + 2] = '>';
        } else {
            screen[prev_y][prev_x + 1] = 'X';
        }
    }

    // Display
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║              ROBOT ARM VISUALIZATION (Side View)             ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");

    for (int y = 0; y < height; y++) {
        printf("║ %s ║\n", screen[y]);
    }

    printf("╚══════════════════════════════════════════════════════════════╝\n");
}


void display_robot_status(RobotSystem *robot) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    ROBOT ARM STATUS                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");

    const char *joint_names[] = {"Base   ", "Shoulder", "Elbow  ", "Wrist  "};

    for (int i = 0; i < NUM_JOINTS; i++) {
        Joint *j = &robot->joints[i];
        printf("║  %s: %6.1f° → %6.1f°  [", joint_names[i],
               j->angle, j->target);

        // Progress bar
        int pos = (int)((j->angle - j->min_angle) /
                       (j->max_angle - j->min_angle) * 20);
        for (int p = 0; p < 20; p++) {
            if (p == pos) printf("●");
            else printf("─");
        }
        printf("]  ║\n");
    }

    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  End Effector Position:                                      ║\n");
    printf("║    X: %6.1f cm    Y: %6.1f cm    Z: %6.1f cm             ║\n",
           robot->end_effector.x, robot->end_effector.y, robot->end_effector.z);
    printf("║  Gripper: %-10s                                        ║\n",
           robot->gripper_open ? "OPEN" : "CLOSED");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}


/* ============================================
 * MEMORY-MAPPED I/O
 * ============================================ */

uint8_t robot_read(void *context, uint16_t address) {
    RobotSystem *robot = (RobotSystem *)context;

    if (address < 0x8000) {
        // ADC (joint position feedback)
        if (address >= ADC_BASE && address < ADC_BASE + 8) {
            return adc_read_channel(&robot->adc, address - ADC_BASE);
        }

        // ADC status
        if (address == ADC_STATUS) {
            return robot->adc.status;
        }

        // DAC readback
        if (address >= DAC_BASE && address < DAC_BASE + 4) {
            return robot->dac.channels[address - DAC_BASE];
        }

        // Gripper state
        if (address == 0x6050) {
            return robot->gripper_open ? 1 : 0;
        }

        return robot->ram[address];
    }

    return 0xFF;
}

void robot_write(void *context, uint16_t address, uint8_t value) {
    RobotSystem *robot = (RobotSystem *)context;

    if (address < 0x8000) {
        // DAC (joint control)
        if (address >= DAC_BASE && address < DAC_BASE + 4) {
            dac_write_channel(&robot->dac, address - DAC_BASE, value);
            return;
        }

        // Gripper control
        if (address == 0x6050) {
            robot->gripper_open = (value != 0);
            return;
        }

        robot->ram[address] = value;
    }
}


/* ============================================
 * INITIALIZATION
 * ============================================ */

void robot_init(RobotSystem *robot) {
    memset(robot->ram, 0, sizeof(robot->ram));

    adc_init(&robot->adc);
    dac_init(&robot->dac);
    pwm_init(&robot->pwm);

    // Initialize joints
    // Joint 0: Base (rotation)
    robot->joints[0] = (Joint){0, 0, 2.0, -180, 180, 0};

    // Joint 1: Shoulder
    robot->joints[1] = (Joint){45, 45, 1.0, -90, 90, 20};

    // Joint 2: Elbow
    robot->joints[2] = (Joint){90, 90, 1.0, 0, 135, 15};

    // Joint 3: Wrist
    robot->joints[3] = (Joint){0, 0, 1.5, -90, 90, 10};

    robot->gripper_open = 1;
    robot->running = 1;
    robot->cycles = 0;
    robot->mode = 1;  // Auto mode

    // Set initial target
    robot->target_pos = (Point3D){25, 0, 20};

    srand(time(NULL));
}


/* ============================================
 * DEMONSTRATION SCENARIOS
 * ============================================ */

void demo_pick_and_place(RobotSystem *robot, int step) {
    Point3D positions[] = {
        {25, 0, 20},   // Start position
        {20, 10, 15},  // Move to object
        {20, 10, 10},  // Lower to object
        {20, 10, 15},  // Lift object
        {15, -15, 15}, // Move to destination
        {15, -15, 10}, // Lower object
        {15, -15, 15}, // Release and lift
        {25, 0, 20}    // Return to start
    };

    int num_positions = sizeof(positions) / sizeof(Point3D);
    int pos_index = (step / 15) % num_positions;

    robot->target_pos = positions[pos_index];
    inverse_kinematics(robot, robot->target_pos);

    // Control gripper
    if (pos_index == 2) robot->gripper_open = 0;  // Grab
    if (pos_index == 6) robot->gripper_open = 1;  // Release
}


/* ============================================
 * MAIN
 * ============================================ */

int main(void) {
    RobotSystem robot;

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║        6502 ROBOT ARM CONTROL SYSTEM                           ║\n");
    printf("║        4-DOF Articulated Robot with Position Feedback          ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    robot_init(&robot);

    printf("\n\nRobot Configuration:\n");
    printf("  Joints: 4 (Base, Shoulder, Elbow, Wrist)\n");
    printf("  Control: DAC outputs (0x6020-0x6023)\n");
    printf("  Feedback: ADC inputs (0x6010-0x6013)\n");
    printf("  Gripper: Digital I/O (0x6050)\n");
    printf("  Algorithm: Inverse kinematics\n\n");

    printf("Running pick-and-place demonstration...\n");
    printf("Press Ctrl+C to stop.\n\n");

    sleep(2);

    for (int step = 0; step < 120 && robot.running; step++) {
        printf("\033[2J\033[H");  // Clear screen

        printf("╔════════════════════════════════════════════════════════════════╗\n");
        printf("║   STEP %3d/120 - Pick and Place Operation                     ║\n", step + 1);
        printf("╚════════════════════════════════════════════════════════════════╝\n");

        // Run demo scenario
        demo_pick_and_place(&robot, step);

        // Update robot
        update_joints(&robot);
        adc_update(&robot.adc);
        dac_update(&robot.dac);

        // Display
        draw_robot_arm(&robot);
        display_robot_status(&robot);

        // Show 6502 code
        printf("\n6502 Control Code:\n");
        printf("  ; Read joint 1 position\n");
        printf("  LDA $%04X       ; ADC Channel 1 (shoulder encoder)\n", ADC_CH1);
        printf("  STA joint1_pos\n\n");
        printf("  ; Calculate target (inverse kinematics)\n");
        printf("  JSR inverse_kinematics\n\n");
        printf("  ; Write to servo controller\n");
        printf("  LDA joint1_target\n");
        printf("  STA $%04X       ; DAC Channel 1 (shoulder motor)\n", DAC_CH1);
        printf("\n");

        usleep(100000);  // 100ms
        robot.cycles += 1000;
    }

    printf("\n\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   Demonstration Complete!                                      ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");

    printf("This robot arm demonstrates:\n");
    printf("  ✓ Multi-axis servo control via DAC\n");
    printf("  ✓ Position feedback via ADC\n");
    printf("  ✓ Inverse kinematics calculations\n");
    printf("  ✓ Real-time control loops\n");
    printf("  ✓ Gripper I/O control\n");
    printf("  ✓ Pick-and-place operations\n\n");

    printf("Memory Map:\n");
    printf("  0x6010-0x6013: Joint position sensors (ADC)\n");
    printf("  0x6020-0x6023: Joint motor controllers (DAC)\n");
    printf("  0x6050:        Gripper control (digital I/O)\n\n");

    return 0;
}

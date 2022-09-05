# O3DE Physics tests

***Testing physics in Open 3D Engine***

The purpose of this project is to test the O3DE physics capabilities in the context of robotic simulations. Two main aspects are considered: robot chassis with wheel suspension (mobility) and a manipulator arm.

## Requirements 

This project requires the [O3DE development branch](https://github.com/o3de/o3de/tree/development). It was created on Windows with O3DE build with the date 2022.08.11 and commit #e57b0df5d661867f6. In principle, it should work with newer versions. It should also work on Linux (not yet tested in its current form).

## Running the demo

1. Follow O3DE documentation to [Download](https://www.o3de.org/docs/welcome-guide/setup/setup-from-github/) and [Build](https://www.o3de.org/docs/welcome-guide/setup/setup-from-github/building-windows/) O3DE. Use the [O3DE development branch](https://github.com/o3de/o3de/tree/development).
1. Clone physics test repository `git clone https://github.com/RobotecAI/o3de-physics-test-scene`

Building the project using project manager on Windows:

1. Open O3DE project manager, select `Open Existing Project` (from the `New Project...` pull-down menu), and select o3de-physics-test-scene.
1. From the "hamburger menu" of the newly added project select `Edit Project Settings...` and select engine and hit `Save`.
1. `Build Project`
1. `Open Editor`
1. Load level

To build the project using the CLI interface on Windows or Linux follow [these instructions](https://docs.o3de.org/docs/welcome-guide/create/creating-projects-using-cli/). Before building the project on Linux register it with: `$O3DE_DIR_PATH/scripts/o3de.sh register -pp $PROJECT_DIR_PATH`.

## Contents

The project consists of the following levels:
- [mobile_robot_physics](#mobile_robot_physics) - Mobile Robot Physics PoC
- [manipulator_physics](#manipulator_physics) - Manipulator Physics PoC (`description on the way`)

In addition, the project includes 3 initial feasibility tests:
- [robot_arm_test1](Doc/Initial_Feasibility_tests.md#robot_arm_test1)
- [robot_arm_test2](Doc/Initial_Feasibility_tests.md#robot_arm_test2)
- [mobile_robot1](Doc/Initial_Feasibility_tests.md#mobile_robot1)


## <a name="mobile_robot_physics"></a>Mobile Robot Physics PoC

Level: **mobile_robot_physics**

![Mobile robot simulation](Doc/mobile_robot_physics.png)

The goal of this level is to test vehicle physics that can be applied to mobile robot. The scene contains a simple 4-wheels vehicle, with a spring-based suspension, which can be controlled using the keyboard. The vehicle can carry a payload, to test physics in different load conditions.

### Instructions

- Use keyboard arrows to control the vehicle (speed and steering).
- Press `Enter` to drop the payload.
- Use `WSAD` and mouse to move the camera around the vehicle.
- To disable the payload (entity: `payload`), either set its rigid body to Kinematic or move it to the side.

### Details

|   | 
| --| 
|For a technical details refer to [this document](Doc/Mobile_Robot_Technical_Details.md) |
|To read about issues and solutions/workarounds refer to [this document](Doc/Mobile_Robot_Issues.md) |

## <a name="manipulator_physics"></a>Manipulator Physics PoC

Level: **manipulator_physics**

![Manipulator physics simulation](Doc/apple_robot_manipulator_with_mockup.gif)

The goal of this level is to test how a manipulator can be created using physics implemented in O3DE. The scene contains a mock-up of an apple-picking robot with a fully functional 3 DOF manipulator. The manipulator performs linear movements in 3 directions:

-   In X axis: horizontally, along the vehicle
-   In Z axis: vertically
-   In Y axis: horizontally, movement of the apple-gripper / suction tube 

### Instructions

-   Use `left` and `right` arrows to move along the X axis. The movement is performed between predefined points.
-   Use `up` and `down` arrows to move along the Z axis. The movement is performed between predefined points.
-   Use `space` to slide out/in the gripper.

### Details

|   | 
| --| 
|To read about issues and solutions/workarounds refer to [this document](Doc/Manipulator_Issues.md) |

## Initial feasibility tests

[See here](Doc/Initial_Feasibility_tests.md)

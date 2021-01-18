# StateAndInputSystemUE4
*© 2018, Ludvig Baummann Olsson. All rights reserved.*

**Made for Unreal engine 4 version 4.20.3**

This project is a solution for state transitions and input handling meant for fighting games.
The main parts of the code included in the project are a state machine system, an input buffer system and a solution for controlling UI via C++.

The **state machine**ystem is meant to function as a means of transitioning between different fighting moves (States). The state system is able to interpret user input to determine what state it should transition to based on what state it is currently in.

The **input buffer** system is a way for the player controller to determine the state of all available buttons at a specific point in time (an input frame). The buffer stores multiple frames of input and sends them to the state machine for interpretation. The buffer also handles the removal of old inputs.

The **UI** portion of the code is mainly meant for debug purposes and only used to give feedback when running the project, so that the user is able to see inputs and state transitions.

## Sample States:

Currently there are three sample states available in the project to try out. You can create new states by creating a new data asset in the UE-editor that inherits from “StateBase”. Make sure to add a new link to any pre-existing state, with the newly created state as the “next state” in the link. Alternatively a newly created state can be set as the BP_SampleCharacter’s DefaultState. Any states that are unconnected are unreachable via user input.

From the state “Static Blaze”:
- Input **(↑ + L-button)** to reach the “Chill Grasp” state.
- Input **(← + L-button)** to reach the “Arc Buster” state. 

From the state “Chill Grasp”:
- Input **(↓, →,  L-button)** to reach the “Static Blaze” state. 

From the state “Arc Buster”:
- Input **(→, ↓, ←, L-button)** to reach the “Static Blaze” state. 


## The controls:

**Keyboard:**
- W =Up direction
- A = Left direction
- S = Down direction
- D = Right direction
- Y = L-button
- U = H-button
- I = S-button
- O = J-button

**Gamepad:**
- Left thumbstick =  Directions
- Left face button = L-button
- Top face button = H-button
- Right face button = S-button
- Bottom face button = J-button

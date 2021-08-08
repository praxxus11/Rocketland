# Rocketland

## Video Demonstration

[Link to video](https://www.youtube.com/watch?v=FZqm8Fz6o4Q)

## Branches

This project contains 4 branches: autoColabMerge, HumanControlled, presentation, and verifygpu (testing branch).

This is branch *HumanControlled*. This contains only CPU code and can be controlled by hand. For easier control, human controlled rockets have all three engines locked in throttle and angle. Flaps are also not controllable. 

### Controls

| Character  | Description              |
|------------|--------------------------|
| UP         | Increase engine thottle  |
| DOWN       | Decrease engine throttle |
| LEFT       | Turn engine left         |
| RIGHT      | Turn engine right        |
| W, A, S, D | Move camera              |
| -, +       | Zoom in/out camera       |
| ESC        | Quit                     |

## Dependencies

This project uses SFML 2.5.1 and Eigen 3.3.9. You must have the CUDA toolkit installed with the nvcc compiler if usage with GPU is desired.

## Images

![Crashing](./imgs/crash.png?raw=true "Crash")
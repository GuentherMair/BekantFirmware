# Using the BEKANT controller

The IKEA BEKANT desk has two buttons on the front of the controller
box: **up** (`△`) and **down** (`▽`). This firmware turns those two
buttons into a full position-memory controller — sit, stand, save.

## Basic motion

| To do this          | Press                       |
|---------------------|-----------------------------|
| Move the desk up    | hold `△`                    |
| Move the desk down  | hold `▽`                    |

Release the button to stop. The desk stops as soon as you let go.

## Memory positions

You can save one sitting position and one standing position. Once
saved, you can move to either with a single button gesture.

### Going to the upper (standing) position

Pick whichever feels natural — both work:

- **Combo-press:** hold `△` and tap `▽` while still holding `△`
- **Double-tap:** tap `△` twice within half a second

### Going to the lower (sitting) position

- **Combo-press:** hold `▽` and tap `△` while still holding `▽`
- **Double-tap:** tap `▽` twice within half a second

The desk will move on its own until it reaches the saved position.
Press any button to cancel mid-movement.

### Saving the current position

To overwrite one of the memory slots with the desk's current height:

1. Move the desk to the height you want to save.
2. Press and hold **both** buttons for 3 seconds.
3. The legs will click once when the position is saved. Release both
   buttons.

The save gesture overwrites whichever memory slot (sitting or
standing) is **closer** to the current position. So if the desk is
near the bottom, the lower memory is updated; if it's near the top,
the upper memory is updated. You don't have to tell the firmware
which one you're saving.

## First-time setup

The firmware ships with sensible defaults:

- **Lower position (sitting):** about 70 cm
- **Upper position (standing):** about 110 cm

To change them, just move the desk to the new height and use the
**save** gesture above. The new value is stored permanently in the
controller's EEPROM and will survive power cycles.

## Edge cases

| Situation                                          | What happens                                  |
|----------------------------------------------------|-----------------------------------------------|
| Press `△` then press `△` again after 1 second     | Two separate up movements, no memory move      |
| Hold both buttons for less than 3 seconds          | Nothing — you have to hold the full 3 s        |
| Press `△` once while already moving up            | No effect — keep holding                       |
| Power cut during a memory move                     | Desk stops; press a button to restart         |
| Press a button during a memory move                | Memory move cancels, you can drive manually   |

## Why two gestures?

The two encodings (combo-press and double-tap) are both supported on
purpose:

- **Combo-press** is the original ivanwick firmware style. It's
  hard to trigger accidentally because you have to deliberately hold
  one button and tap the other.
- **Double-tap** is the DieSteinhose fork style. It's what people
  learn from elevator buttons — natural and one-handed.

If you're new to the controller, try both and use whichever feels
better. The firmware doesn't care which you pick.

## Safety

- Keep the desk's path clear during all movement.
- Stay near the controller while a memory move is in progress, so
  you can cancel by pressing any button.
- The firmware drops the relay when the desk reaches the physical
  endstop, so the motor cannot stall indefinitely.
- If something goes wrong, cutting power stops the desk immediately.

## Disclaimer

Use at your own risk. Position memory is convenient but the original
IKEA firmware omits it deliberately so that a human is always in
the control loop during movement. This firmware keeps the same
spirit: any button press cancels an automatic move.

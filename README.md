# Bunny Hop

A simple Geometry Dash Geode mod that automatically boosts the player upward at a fixed interval during gameplay.

## Requirements

- Geometry Dash 2.2081
- Geode 5.10.1
- Android64

## Features

- Automatically applies an upward velocity at a fixed interval.
- Works independently of the current level.
- Intended to affect the active player regardless of gamemode.
- Does not modify level objects.

## Settings

### Bunny Hop

Enables or disables the automatic jumps.

Default: `true`

### Jump Interval

Controls how often the automatic upward boost occurs.

- Default: `0.5` seconds
- Minimum: `0.1` seconds
- Maximum: `10.0` seconds

## Building

Install the Geode SDK and Android64 SDK binaries, then build the mod for Android64:

```bash
geode sdk install 5.10.1
geode sdk install-binaries -p android64
geode build -p android64

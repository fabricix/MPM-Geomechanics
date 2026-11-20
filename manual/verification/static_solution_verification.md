### Saving and Loading Simulation States
For multi-stage analyses (such as generating an initial geostatic stress field via quasi-static damping before applying dynamic or undrained loads), the stress and particle state can be saved and reloaded in the JSON input file:

**Save state:**
```json
"save_state": true,
"save_state_file": "my_state_to_save.json"
```

**Load state:**
```json
"load_state": true,
"load_state_file": "my_state_to_load.json"
```
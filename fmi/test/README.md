# How to test FMU packages

## Test FMU export

Load the exported FMU in [FMpy](https://github.com/CATIA-Systems/FMPy/).

```python
import fmpy
fmpy.simulate_fmu(file, debug_logging=True)
```

With `file` the path to either a zipped fmu file or a regular folder (e.g. "simphonie/fmi/test/fmu"). 

### Parameters
- `filename`:               filename of the FMU or directory with extracted FMU
- `validate`:               validate the FMU and start values
- `start_time`:             simulation start time (None: use default experiment or 0 if not defined)
- `stop_time`:              simulation stop time (None: use default experiment or start_time + 1 if not defined)
- `solver`:                 solver to use for model exchange ('Euler' or 'CVode')
- `step_size`:              step size for the 'Euler' solver
- `relative_tolerance`:     relative tolerance for the 'CVode' solver and FMI 2.0 co-simulation FMUs
- `output_interval`:        interval for sampling the output
- `record_events`:          record outputs at events (model exchange only)
- `fmi_type`:               FMI type for the simulation (None: determine from FMU)
- `start_values`:           dictionary of variable name -> value pairs
- `apply_default_start_values`:  apply the start values from the model description (deprecated)
- `input`                  a structured numpy array that contains the input (see :class:`Input`)
- `output`:                 list of variables to record (None: record outputs)
- `timeout`:                timeout for the simulation
- `debug_logging`:          enable the FMU's debug logging
- `visible`:                interactive mode (True) or batch mode (False)
- `fmi_call_logger`:        callback function to log FMI calls
- `logger`:                 callback function passed to the FMU (experimental)
- `step_finished`:          callback to interact with the simulation (experimental)
- `model_description`:      the previously loaded model description (experimental)
- `fmu_instance`:           the previously instantiated FMU (experimental)
- `set_input_derivatives`:  set the input derivatives (FMI 2.0 Co-Simulation only)
- `remote_platform`:        platform to use for remoting server ('auto': determine automatically if current platform is not supported, None: no remoting; experimental)
- `early_return_allowed`:   allow early return in FMI 3.0 Co-Simulation
- `use_event_mode`:         use event mode in FMI 3.0 Co-Simulation if the FMU supports it
- `initialize`:             initialize the FMU
- `terminate`:              terminate the FMU
- `fmu_state`:              the FMU state or serialized FMU state to initialize the FMU
- `set_stop_time`:          communicate the stop time to the FMU instance

### Returns
- `result`:                 a structured numpy array that contains the result

## Test FMU import

- Download a FMU model from the [official samples](https://github.com/modelica/Reference-FMUs/).
- Run simphonie on a lua script that uses the FMILoad component:
```lua
s=require "simphonie_lua"
sim=s.CreateSimulator({
    name="MySim",
    lib="simphonie_kern",
    libraries={
        "simphonie_fmi",
    },
    components={
        MyFMISim={type="simphonie::fmi::FMILoad", path="test/BouncingBall", stepSize=0.1}, -- the path is either a path to a regular folder or a path to a .fmu zipped file
    },
    connections={
    },
    schedule={
        {name="MyFMISim/doStep", cycleTime_ms=50},
    }
})
sim:Publish()
sim:Configure()
sim.MyFMISim.g.Value = -18.0
sim:Connect()
sim:Run()
```

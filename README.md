## Setup


-----


#### Requirements:

* Qt version 5.11 or greater
* CMake version 3.5 or greater
* (*Optional*) Qt Creator 4.11 x86 or greater

#### Building:

In order to build with CMake alone, run
```
cmake --build PATH_TO_SOURCE --target all
```

Alternatively (if that does not work), open the project using Qt creator and build from there.

## Usage
-----

### CLI Usage
The software can always be used in CLI mode. To force CLI mode usage, you can compile with -DNOGUI to disable the GUI. This has the added benefit of reducing the number of dependencies on Qt and reducing the size of the executable.

Minimally, the software accepts a JSON file containing a geometry description and a reference to the CSV from data should be pulled, and outputs a first-order neighbourhood relation in the form of a list of identifiers and associated neighbour identifiers in a CSV file.
In CLI mode, the software accepts certain flags:

* `-o, --order <integer>` To what 'order' the neighbourhood relation should be built. Higher values add more neighbours. Default is 1.
* `-t, --tolerance <float>` Tolerance with which to build the relation. This influences the threshold at which a node is considered to be a neighbour of another. Default is 1.0.
* `-g, --gui` If set, opens the gui after evaluating command line arguments, regardless of if these were invalid. Correct argument values will not be passed to the gui. Does not work if compiled with -DNOGUI.

To see more detailed usage information, use the `-h` flag.




### GUI Usage
Import a CSV file containing a description of STT geometry using `Table -> Import CSV...`. The imported CSV will be visible in the Table View. Three geometry types are available depending on the STT description: `Cylindrical` `Cuboidal` and `Spherical` (*Subject to change*). Under the geometry selection box is a set of parameters required to describe the shape. Each parameter can be fixed for every element in the CSV, or set to use values from a given column for each field. The checkbox to the right of each parameter is used to toggle between these options.

In order to use a column for a certain field, you must right-click the header of the desired column and bind it to the appropriate field using the context menu.

Example:

Position 1, 2, 3 ☑

This will tell the program to use the values in columns 1, 2 and 3 (e.g x, y, z) to construct the position of the shape.

Example:

Radius 0.5 ☐

This will tell the program to use the same radius value for each element.

Parameters can be saved/loaded to/from disk in JSON format using `Table -> Save/Load Parameters`. The parameter configuration stores a relative path to the CSV file to which it applies.

After all the required parameters are set, you can generate a graph for the data by pressing the `Generate Nodes` button. Two parameters are available for controlling how the nodes are created:

`Spacing` is a scaling factor applied to the position value of the given shape, and can be used to visibly separate the nodes if they would otherwise be too close together. This is typically the case for STTs where each tube is physically touching its neighbours.

`Node Scale` can be used to increase or decrease the visual size of the generated nodes. At the default of 1, the size of the node is based on the physical size of the geometry in the x-y axis.

The generated nodes can be inspected in the Graph View tab. View controls:

* Left-click to select and de-select nodes
* Middle-click and hold to pan with the mouse
* Left-click and drag between two nodes to create a new edge
* Ctrl + MouseWheel to zoom in and out

The neighbourhood relation can be generated for the set of nodes by pressing the `Build Relation` button. The operation may take some time depending on the number of nodes (~ *O*(n^2)).

For performance reasons, the generated edges are not all immediately visible. Only when a node is selected will the edges to its neighbours be shown.

The set of neighbours for each node in the current graph can be exported to Text or Csv using `Graph -> Export as...`.

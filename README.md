# Heart Graph Plugin
A generic runtime node graph editor and viewer for Unreal Engine 5.1.

## Modules
### Heart
The core of HeartGraph. Defines the core types used to build a graph: `UHeartGraph`, `UHeartGraphNode`, and `UHeartGraphPin`, as well as a registry subsystem that ties them together.

### HeartCore
Underlying math utilities, and generic UI classes that aren't specifically for HeartGraph, but kept here to avoid dependencies on another plugin. At some point, I might move some or all of this to a seperate plugin if it becomes too much.

HeartCore implements a custom system for managing UI input, inspired mostly by UE's new EnhancedInput, that centralizes input definition into a single asset, `UHeartWidgetInputBindingAsset`, that functions much like an InputMappingContext.
Currently this system requires some c++ boilerplate to add it to custom widget types, but is implemented out-of-the-box for any HeartGraph widget.

### HeartCanvas
A toolset for displaying and editing a `UHeartGraph` using entirely UMG widgets. Essentially meant to replicate unreal's native `UEdGraph` editor, except with far more customization, to tailor the graph's appearance and behavior to the needs of the project.

### HeartEditor
A (currently WIP) asset authoring tool for Heart graphs using Unreal's native UEdGraph toolset. Such a workflow allows for creating a graph inside the Unreal Engine Editor, and viewing it using HeartCanvas at runtime (such as a perk tree).


## Dependencies

### StructUtils
`FInstancedStruct` is currently used for defining Pin Types. This may not always be the case, pending a possible refactor of Pin Type declaration.

### AssetSearch
I want to implement an asset search feature similar to Flow Graph's implementation.


## Planned Dependencies

### UMG Viewmodel
https://github.com/Drakynfly/HeartGraph/issues/10

### WaveFunctionCollapse
https://github.com/Drakynfly/HeartGraph/issues/11

### ControlFlows
https://github.com/Drakynfly/HeartGraph/issues/7#issuecomment-1399660883

### GameplayTags
I'm sure I'll need them at some point, I always do.

## Compatibility

### CommonUI
HeartCanvas specifically *doesn't* use CommonUI, as it's methods for defining UI input are not to my liking. However, using most CommonUI components for building Canvas widgets still works.

## Links
Demo Project: https://github.com/Drakynfly/HeartGraphDemo

Discord:      https://discord.gg/AAk9yNwKk8 (Drakynfly's Plugins)

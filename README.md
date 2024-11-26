# Heart Graph Plugin
A generic runtime node graph editor and viewer for Unreal Engine.
Supports versions 5.3 and 5.4, with legacy branch for 5.2 with a limited feature set.
Compiles on Windows and macOS.

## Modules
### Heart
The core of HeartGraph. Defines the core types used to build a graph: `UHeartGraph`, `UHeartGraphNode`, and `UHeartGraphSchema`, as well as a registry subsystem that ties them together.

### Blood
What flows through a heart? Blood, of course. Blood is the data shuttler for heart graphs, a variant data type system, designed for interoperability with FProperties, as well as quick, efficient access from both c++ and BP.

### HeartCore
Underlying math utilities, and generic UI classes that aren't specifically for HeartGraph, but kept here to avoid dependencies on another plugin. At some point, I might move some or all of this to a separate plugin if it becomes too much.

HeartCore implements a custom system for managing UI input, inspired mostly by UE's new EnhancedInput, that centralizes input definition into a single asset, `UHeartInputBindingAsset`, which functions much like an InputMappingContext.
Currently, this system requires some c++ boilerplate to add it to custom widget types, but is implemented out-of-the-box for any HeartGraph widget.

### HeartCanvas
A toolset for displaying and editing a `UHeartGraph` using entirely UMG widgets. Essentially meant to replicate Unreal's native `UEdGraph` editor, except with far more customization, to tailor the graph's appearance and behavior to the needs of the project.

### HeartScene
An alternative visualization method using `AActor` and `UActorComponent` classes, allowing for full 3D graph visualizers.

### HeartEditor (WIP)
An asset authoring tool for Heart graphs using Unreal's native UEdGraph toolset. Such a workflow allows for creating a graph inside the Unreal Engine Editor, and viewing it using HeartCanvas at runtime (such as a perk tree).

### HeartNet (WIP)
An addition to Heart that enables replication support of a HeartGraph.

## Plugin Dependencies
- This plugin depends on another free plugin I've made, which can be found here:
    - Flakes - A serialization backend: https://github.com/Drakynfly/Flakes

## Engine Plugin Dependencies

### AssetSearch
I want to implement an asset search feature similar to Flow Graph's implementation. Not implemented yet...

### EngineAssetDefinitions
Core engine module that is a plugin for some reason.

### EnhancedInput
This is only used by the SceneModule to link input, but I may add support for InputActions to Canvas later.

### GameplayTagsEditor
A "plugin dependency" by a technicality. I don't know why the main module is part of the core engine, but this module isn't. Anyway.


## Planned Dependencies

### UMG Viewmodel
https://github.com/Drakynfly/HeartGraph/issues/10

### ControlFlows
https://github.com/Drakynfly/HeartGraph/issues/7#issuecomment-1399660883

## Compatibility

### CommonUI
HeartCanvas specifically *doesn't* use CommonUI, as its methods for defining UI input are not to my liking. However, using most CommonUI components for building Canvas widgets still works.

## Links
Docs:         https://heart-1.gitbook.io/heart-plugin/

Demo Project: https://github.com/Drakynfly/HeartDemoProject / https://github.com/Drakynfly/HeartDemoContent

Discord:      [![Discord](https://img.shields.io/discord/996247217314738286.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/AAk9yNwKk8) (Drakynfly's Plugins)

## Acknowledgments

- [MothDocter's FlowGraph](https://github.com/MothCocoon/FlowGraph): Both literally and figuratively. Breaking apart and learning about the graphs in Flow is what started my interest in runtime graphs. But also, I stole a lot of the editor code to make Heart's Editor module.
- [Red Blob Games - Hexagonal Grids](https://www.redblobgames.com/grids/hexagons/): I followed this guide for making the `Heart::Hex` namespace.
- [olvb's nodesoup](https://github.com/olvb/nodesoup): Adapted for my own uses.
- [Sythenz's AssetEditorTemplate](https://github.com/Sythenz/AssetEditorTemplate): Adapted for my own uses.
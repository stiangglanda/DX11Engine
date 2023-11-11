# DX11Engine

It is a personal game engine that currently only supports DirectX 11.
I developed it purely for educational purposes!
I learned a lot during the development of this project, and I intend to use what I have learned in a new game engine that can be found here [Vulkan Engine](https://github.com/stiangglanda/VulkanEngine).
The things that I intend to do differently and have partially started doing are:
- using CMake
- using Github actions(CI/CD)
- using a clearer, more standardised project structure
- dependency management using git submodules and not just vcpkg like I did here since it is easier to set it up on a new machine.
- Since I will be using Vulkan, I intend to make it run on Linux.
- a modular and cleaner architecture
- Splitting the project into separate projects like Core, Editor and Client
- applying the lessons I have learned after reading Effective Modern C++
- Unit Testing

## Some Implemented Features Include:
- Shadow Mapping
- Screen space ambient occlusion
- Physics that use the [Nvidia PhysX](https://github.com/NVIDIAGameWorks/PhysX) engine 
- Terrein Rendering System
- GPU-accelerated particle system
- Font rendering
- Skeletal Animation
- loading of a diverse set of model formats thanks to [Assimp](https://github.com/assimp/assimp)
- an entity component system
- a WYSIWYG editor (not finished) shown below

![alt text](https://github.com/stiangglanda/DX11Engine/blob/master/EditorUI.png)

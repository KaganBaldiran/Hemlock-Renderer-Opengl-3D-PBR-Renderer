# Hemlock-Renderer-Opengl-3D-Renderer
Re-committed as a new repo to clean up the code and to continue developping.
Hemlock Renderer is a personal project I've been working on for the past two months. It's an Opengl based 3D PBR renderer written in C++.
Still is in an early stage of development and requires a considerable amount of work in order to bring it to life completely.
I'm mostly following widely used techniques and algorithms.

# Features

I'm trying to widen up the feature arsenal as much as can . But also trying to fix the known bugs.
Here are some of the key features

- Multi-object PBR workflow(Environment light sampling isn't supported for now)
- SSAO
- HDRI support along with simple cubemaps.
- Screen based post processing(Limited but I'll be adding features like DOF soon)
- Linear Shadow Mapping for directional lights(Soon to be replaced)
- Omnidirectional shadow mapping for point lights
- Native scene file save-read 

# To-Do

- Environment light sampling 
- Widely supported light types in shadow mapping 
- Area Lights
- Screen space global illumination
- Ray marched volume rendering 
- Node support for material authuring(Long term goal)

# Showcase
This is how it currently looks(Some of the models don't belong to me):

![SplashScreenImage](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-PBR-Renderer/assets/80681941/7c1af59e-8c1b-4a98-806d-f1ccf1f091d8)
![image](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-PBR-Renderer/assets/80681941/e25aca14-84ba-4474-8f3e-3e40e0a531a3)
![image](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-PBR-Renderer/assets/80681941/2e92fbe1-53e5-43b3-824c-7a145e7ce7d8)
![image](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-PBR-Renderer/assets/80681941/0043e6fa-31d7-4abc-a55d-b1bb7777b90a)
![image](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-Renderer/assets/80681941/f54b5c3a-2790-4524-ac9e-5c31dac62ce4)
![image](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-PBR-Renderer/assets/80681941/486a4163-b193-4ed2-8386-2b0103659365)
![image](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-Renderer/assets/80681941/db111067-8f33-46fe-9469-bb975aa2824c)
![image](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-Renderer/assets/80681941/43dda2df-a711-4fbe-803f-8842cc0ce12e)
![image](https://github.com/KaganBaldiran/Hemlock-Renderer-Opengl-3D-Renderer/assets/80681941/253c8d6d-d49a-4a20-b0f0-65f005bf0fc4)
![image](https://user-images.githubusercontent.com/80681941/232658091-e586612c-8063-4f7b-b5c7-ad8626e2e1a9.png)


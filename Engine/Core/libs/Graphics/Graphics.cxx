#include <Utils/macros.h>

#include "../Graphics/src/Graphics/Animations/Bone.cpp"

#include "../Graphics/src/Graphics/Pipeline/Vulkan/VulkanImGUI.cpp"
#include "../Graphics/src/Graphics/Pipeline/Vulkan/VulkanMemory.cpp"
#include "../Graphics/src/Graphics/Pipeline/Environment.cpp"
#include "../Graphics/src/Graphics/Pipeline/OpenGL.cpp"
#include "../Graphics/src/Graphics/Pipeline/TextureHelper.cpp"
#include "../Graphics/src/Graphics/Pipeline/Vulkan.cpp"
#include "../Graphics/src/Graphics/Pipeline/Pipeline.cpp"
#include "../Graphics/src/Graphics/Pipeline/EmptyPipeline.cpp"

#include "../Graphics/src/Graphics/GUI/Link.cpp"
#include "../Graphics/src/Graphics/GUI/ICanvas.cpp"
#include "../Graphics/src/Graphics/GUI/Node.cpp"
#include "../Graphics/src/Graphics/GUI/Icons.cpp"
#include "../Graphics/src/Graphics/GUI/NodeManager.cpp"
#include "../Graphics/src/Graphics/GUI/Pin.cpp"
#include "../Graphics/src/Graphics/GUI/Editor/MessageBox.cpp"
#include "../Graphics/src/Graphics/GUI/Editor/Theme.cpp"
#include "../Graphics/src/Graphics/GUI/WidgetManager.cpp"
#include "../Graphics/src/Graphics/GUI/Widget.cpp"

#include "../Graphics/src/Graphics/Lighting/DirectionalLight.cpp"
#include "../Graphics/src/Graphics/Lighting/Light.cpp"
#include "../Graphics/src/Graphics/Lighting/PointLight.cpp"
#include "../Graphics/src/Graphics/Lighting/Spotlight.cpp"

#include "../Graphics/src/Graphics/Loaders/FbxLoader.cpp"
#include "../Graphics/src/Graphics/Loaders/ImageLoader.cpp"
#include "../Graphics/src/Graphics/Loaders/ObjLoader.cpp"
#include "../Graphics/src/Graphics/Loaders/SRSL.cpp"
#include "../Graphics/src/Graphics/Loaders/SRSLParser.cpp"
#include "../Graphics/src/Graphics/Loaders/TextureLoader.cpp"
#include "../Graphics/src/Graphics/Loaders/ShaderProperties.cpp"

#include "../Graphics/src/Graphics/Memory/TextureConfigs.cpp"
#include "../Graphics/src/Graphics/Memory/MeshManager.cpp"
#include "../Graphics/src/Graphics/Memory/UBOManager.cpp"
#include "../Graphics/src/Graphics/Memory/ShaderProgramManager.cpp"
#include "../Graphics/src/Graphics/Memory/ShaderUBOBlock.cpp"
#include "../Graphics/src/Graphics/Memory/CameraManager.cpp"

#include "../Graphics/src/Graphics/Pass/BasePass.cpp"
#include "../Graphics/src/Graphics/Pass/SkyboxPass.cpp"
#include "../Graphics/src/Graphics/Pass/FramebufferPass.cpp"
#include "../Graphics/src/Graphics/Pass/SwapchainPass.cpp"
#include "../Graphics/src/Graphics/Pass/GroupPass.cpp"
#include "../Graphics/src/Graphics/Pass/WidgetPass.cpp"
#include "../Graphics/src/Graphics/Pass/OpaquePass.cpp"
#include "../Graphics/src/Graphics/Pass/TransparentPass.cpp"
#include "../Graphics/src/Graphics/Pass/SortedTransparentPass.cpp"
#include "../Graphics/src/Graphics/Pass/PostProcessPass.cpp"
#include "../Graphics/src/Graphics/Pass/DebugPass.cpp"
#include "../Graphics/src/Graphics/Pass/ColorBufferPass.cpp"

#include "../Graphics/src/Graphics/Font/Font.cpp"
#include "../Graphics/src/Graphics/Font/FontLoader.cpp"
#include "../Graphics/src/Graphics/Font/SDF.cpp"
#include "../Graphics/src/Graphics/Font/Text.cpp"
#include "../Graphics/src/Graphics/Font/TextBuilder.cpp"

#include "../Graphics/src/Graphics/UI/Sprite2D.cpp"
#include "../Graphics/src/Graphics/UI/Canvas.cpp"
#include "../Graphics/src/Graphics/UI/Anchor.cpp"

#include "../Graphics/src/Graphics/Render/RenderScene.cpp"
#include "../Graphics/src/Graphics/Render/ColorBuffer.cpp"
#include "../Graphics/src/Graphics/Render/RenderTechnique.cpp"
#include "../Graphics/src/Graphics/Render/MeshCluster.cpp"
#include "../Graphics/src/Graphics/Render/PostProcessing.cpp"
#include "../Graphics/src/Graphics/Render/Render.cpp"
#include "../Graphics/src/Graphics/Render/ShaderGenerator.cpp"
#include "../Graphics/src/Graphics/Render/RenderManager.cpp"
#include "../Graphics/src/Graphics/Render/RenderContext.cpp"
#include "../Graphics/src/Graphics/Render/SortedMeshQueue.cpp"
#include "../Graphics/src/Graphics/Render/DebugRenderer.cpp"
#include "../Graphics/src/Graphics/Render/Implementations/VulkanRender.cpp"

#include "../Graphics/src/Graphics/Types/Geometry/DebugWireframeMesh.cpp"
#include "../Graphics/src/Graphics/Types/Geometry/DebugLine.cpp"
#include "../Graphics/src/Graphics/Types/Geometry/IndexedMesh.cpp"
#include "../Graphics/src/Graphics/Types/Geometry/ProceduralMesh.cpp"
#include "../Graphics/src/Graphics/Types/Geometry/Mesh3D.cpp"
#include "../Graphics/src/Graphics/Types/Geometry/MeshComponent.cpp"
#include "../Graphics/src/Graphics/Types/Geometry/SkinnedMesh.cpp"

#include "../Graphics/src/Graphics/Types/EditorGrid.cpp"
#include "../Graphics/src/Graphics/Types/Framebuffer.cpp"
#include "../Graphics/src/Graphics/Types/Material.cpp"
#include "../Graphics/src/Graphics/Types/Mesh.cpp"
#include "../Graphics/src/Graphics/Types/Skybox.cpp"
#include "../Graphics/src/Graphics/Types/Texture.cpp"
#include "../Graphics/src/Graphics/Types/GraphicsCommand.cpp"
#include "../Graphics/src/Graphics/Types/Camera.cpp"
#include "../Graphics/src/Graphics/Types/Shader.cpp"
#include "../Graphics/src/Graphics/Types/RenderTexture.cpp"

#include "../Graphics/src/Graphics/Window/Window.cpp"
#include "../Graphics/src/Graphics/Window/BasicWindowImpl.cpp"

#if defined(SR_WIN32)
    #include "../Graphics/src/Graphics/Window/Win32Window.cpp"
#endif
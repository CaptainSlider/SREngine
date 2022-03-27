//
// Created by Nikita on 29.12.2020.
//

#include <macros.h>

#ifdef SR_WIN32
    #include <Windows.h>
    #include <shellapi.h>
#endif

#include <Debug.h>

#include <Engine.h>

#include <ResourceManager/ResourceManager.h>
#include <Environment/OpenGL.h>
#include <Environment/Vulkan.h>
#include <EntityComponentSystem/Transform.h>

#include <Types/Rigidbody.h>
#include <Types/Geometry/Mesh3D.h>
#include <Animations/Bone.h>
#include <World/World.h>
#include <Input/InputSystem.h>
#include <Memory/MeshAllocator.h>
#include <World/VisualChunk.h>
#include <World/VisualRegion.h>
#include <Utils/CmdOptions.h>
#include <Utils/Features.h>
#include <GUI/NodeManager.h>
#include <FbxLoader/Debug.h>
#include <Types/Marshal.h>

using namespace Framework;

using namespace Framework::Core;
using namespace Framework::Core::World;

using namespace Framework::Helper;
using namespace Framework::Helper::Math;
using namespace Framework::Helper::Types;
using namespace Framework::Helper::World;

using namespace Framework::Graphics;
using namespace Framework::Graphics::Types;
using namespace Framework::Graphics::Animations;

using namespace Framework::Physics;
using namespace Framework::Physics::Types;

/*
        +---------------+       +----+          +----+       +---------------+            +----------------+
        |               |       \     \        /     /       |               |           /                 |
        |               |        \     \      /     /        |               |          /                  |
        |      +--------+         \     \    /     /         |      +--------+         /         +---------+
        |      |                   \     \  /     /          |      |                 /        /
        |      +--------+           \     \/     /           |      +--------+       |        /
        |               |            \          /            |               |       |       |
        |               |            /          \            |               |       |       |
        |      +--------+           /     /\     \           |      +--------+       |        \
        |      |                   /     /  \     \          |      |                 \        \
        |      +--------+         /     /    \     \         |      +--------+         \        +---------+
        |               |        /     /      \     \        |               |          \                 |
        |               |       /     /        \     \       |               |           \                |
        +---------------+       +----+          +-----+      +---------------+            +---------------+
 */

int main(int argc, char **argv) {
    if constexpr (sizeof(size_t) != 8) {
        std::cerr << "The engine only supports 64-bit systems!\n";
        return -1;
    }

    std::string exe = FileSystem::GetPathToExe();
    Debug::Init(exe, true, Debug::Theme::Dark);
    Debug::SetLevel(Debug::Level::Low);

    if (auto folder = GetCmdOption(argv, argv + argc, "-resources"); folder.empty())
        ResourceManager::Instance().Init(exe + "/../../Resources");
    else
        ResourceManager::Instance().Init(folder);

    RuntimeTest::MarshalRunRuntimeTest();

    Features::Instance().Reload(ResourceManager::Instance().GetResPath().Concat("/Configs/Features.xml"));

    if (!FbxLoader::Debug::IsInit()) {
        FbxLoader::Debug::Init(
                [](const std::string &msg) { Helper::Debug::Error(msg); },
                [](const std::string &msg) { Helper::Debug::Warn(msg); }
        );
    }

    if (Features::Instance().Enabled("CrashHandler")) {
#ifdef SR_WIN32
        ShellExecute(nullptr, "open", (ResourceManager::Instance().GetResPath().Concat(
                "/Utilities/EngineCrashHandler.exe").CStr()),
                     ("--log log.txt --target " + FileSystem::GetExecutableFileName() + " --out " + exe + "\\").c_str(),
                     nullptr, SW_SHOWDEFAULT
        );
#endif
    }

    // Register all resource types
    {
        ResourceManager::Instance().RegisterType<RawMesh>();
        ResourceManager::Instance().RegisterType<Mesh>();
        ResourceManager::Instance().RegisterType<Texture>();
        ResourceManager::Instance().RegisterType<Material>();
        ResourceManager::Instance().RegisterType<Shader>();
    }

    // Register all components
    {
        //Component::RegisterComponent("SkinnedMesh", []() -> Component* { return new SkinnedMesh();  });
        ComponentManager::Instance().RegisterComponent<Mesh3D>([]() -> Mesh3D* { return Memory::MeshAllocator::Allocate<Mesh3D>(); });
        ComponentManager::Instance().RegisterComponent<Rigidbody>([]() -> Rigidbody* { return new Rigidbody(); });
        ComponentManager::Instance().RegisterComponent<Camera>([]() -> Camera* { return Camera::Allocate(); });
        ComponentManager::Instance().RegisterComponent<Bone>([]() -> Bone* { return new Bone(); });

        if (Helper::Features::Instance().Enabled("DebugChunks", false))
            Chunk::SetAllocator([](SRChunkAllocArgs) -> Chunk * { return new VisualChunk(SRChunkAllocVArgs); });

        if (Helper::Features::Instance().Enabled("DebugRegions", false))
            Region::SetAllocator([](SRRegionAllocArgs) -> Region* { return new VisualRegion(SRRegionAllocVArgs); });

        Scene::SetAllocator([](const std::string& name) -> Scene* { return new Core::World::World(name); });
    }

    const auto&& envDoc = Xml::Document::Load(ResourceManager::Instance().GetConfigPath().Concat("Environment.xml"));
    const auto&& envName = envDoc.TryRoot().TryGetNode("Environment").TryGetAttribute("Name").ToString("");

    if (envName == "OpenGL"){
        Environment::Set(new OpenGL());
    }
    else if (envName == "Vulkan") {
        Environment::Set(new Vulkan());
    }
    else if (envName.empty()) {
        SR_ERROR("System error: file \"Resources/Configs/Environment.xml\" does not exist!");
        ResourceManager::Instance().Stop();
        Debug::Stop();
        return -1500;
    }
    else {
        SR_ERROR("System error: unknown environment! \"" + envName + "\" does not support!");
        ResourceManager::Instance().Stop();
        Debug::Stop();
        return -2000;
    }

    Render* render = Render::Allocate("Main SpaRcle Render");
    if (!render) {
        SR_ERROR("FATAL: render is not support this pipeline!");
        ResourceManager::Instance().Stop();
        Debug::Stop();
        return -1000;
    }

    auto window = new Window(
            "SpaRcle Engine",
            "Engine/icon.ico",
            WindowFormat::_1366_768,
            render,
            false, // vsync
            false, // fullscreen
            true,  // resizable
            true,  // header enabled
            2
    );

    auto physics = new PhysEngine();

    auto&& engine = Engine::Instance();

    if(engine.Create(window, physics)) {
      if (engine.Init(Engine::MainScriptType::Engine)) {
          if (engine.Run()){

          }
          else
              SR_ERROR("Failed to running game engine!");
      }
      else
          SR_ERROR("Failed to initializing game engine!");
    }
    else
        SR_ERROR("Failed to creating game engine!");

    if (engine.IsRun()) {
        Debug::System("All systems successfully run!");

        engine.Await(); // await close engine
    }

    engine.Close();

    Framework::Helper::EntityManager::Destroy();
    Framework::Engine::Destroy();
    Framework::Graphics::GUI::NodeManager::Destroy();

    Debug::System("All systems successfully closed!");

    ResourceManager::Instance().Stop();

    return Debug::Stop();
}
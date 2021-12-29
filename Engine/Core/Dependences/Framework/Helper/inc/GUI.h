//
// Created by Nikita on 15.03.2021.
//

#ifndef GAMEENGINE_GUI_H
#define GAMEENGINE_GUI_H

#include <macros.h>
#include <Debug.h>

#ifdef SR_USE_IMGUI
    //#define IMGUI_DISABLE

    #define IMGUI_DEFINE_MATH_OPERATORS
    #define IMGUI_USE_WCHAR32

    #ifndef IM_ASSERT
        #define IM_ASSERT(_EXP) SRAssert1(_EXP)
    #endif

    #ifndef IM_ASSERT_USER_ERROR
        #define IM_ASSERT_USER_ERROR(_EXP, _MSG) SRAssert2(_EXP, _MSG)
    #endif

    #include <imgui.h>
    #include <misc/cpp/imgui_stdlib.h>
    #include <imgui_internal.h>

    #ifdef SR_WIN32
        #include <backends/imgui_impl_win32.h>
    #endif

    #ifdef SR_USE_VULKAN
        #include <backends/imgui_impl_vulkan.h>
    #endif

    #ifdef SR_USE_OPENGL
        #include <GL/glew.h>
        #include <backends/imgui_impl_opengl3.h>
    #endif

    #ifdef SR_USE_GLFW3
        #include <backends/imgui_impl_glfw.h>
    #endif

    #include <ImGuizmo.h>
#endif

#include <string>

namespace Framework::Helper::GUI {
    inline static void DrawTextOnCenter(const std::string& text, bool sameLine = true) {
        float font_size = ImGui::GetFontSize() * text.size() / 2;

        if (sameLine)
            ImGui::SameLine(
                    ImGui::GetWindowSize().x / 2 -
                    font_size + (font_size / 2)
            );

        ImGui::Text("%s", text.c_str());
    }
}

#endif //GAMEENGINE_GUI_H
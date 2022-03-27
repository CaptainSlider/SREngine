//
// Created by Monika on 11.02.2022.
//

#ifndef SRENGINE_HIERARCHY_H
#define SRENGINE_HIERARCHY_H

#include <GUI/Widget.h>
#include <Types/SafePointer.h>
#include <World/Scene.h>

#include <string>
#include <macros.h>
#include <mutex>
#include <Input/InputEvents.h>
#include <EntityComponentSystem/GameObject.h>

namespace SR_CORE_NS::GUI {
    class Hierarchy : public Graphics::GUI::Widget {
    public:
        Hierarchy();
        ~Hierarchy() override = default;

    public:
        void SetScene(const SR_WORLD_NS::Scene::Ptr& scene);

        void OnKeyDown(const Helper::KeyDownEvent& event) override;
        void OnKeyUp(const Helper::KeyUpEvent& event) override;

    private:
        void Draw() override;
        void CheckSelected(const Helper::GameObject::Ptr& gm) const;
        void ContextMenu(const Helper::GameObject::Ptr& gm, uint64_t id) const;
        void DrawChild(const Helper::GameObject::Ptr& root);
        void Copy() const;
        void Paste() const;

    private:
        const ImGuiTreeNodeFlags m_nodeFlagsWithChild = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        const ImGuiTreeNodeFlags m_nodeFlagsWithoutChild = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

        std::mutex m_mutex;
        SR_WORLD_NS::Scene::Ptr m_scene;
        std::list<Helper::GameObject::Ptr> m_pointersHolder;
        std::atomic<bool> m_shiftPressed;

    };
}

#endif //SRENGINE_HIERARCHY_H

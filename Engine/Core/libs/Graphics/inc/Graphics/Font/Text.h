//
// Created by Monika on 14.02.2022.
//

#ifndef SRENGINE_TEXT_H
#define SRENGINE_TEXT_H

#include <Graphics/Types/Vertices.h>
#include <Graphics/Types/Mesh.h>
#include <Utils/Types/UnicodeString.h>
#include <Utils/ECS/Component.h>

namespace SR_GTYPES_NS {
    class Font;

    class Text : public Mesh, public SR_UTILS_NS::Component {
        SR_ENTITY_SET_VERSION(1002);
        SR_INITIALIZE_COMPONENT(Text);
    public:
        typedef Vertices::SimpleVertex VertexType;

    public:
        Text();
        ~Text() override;

    public:
        static Component* LoadComponent(SR_HTYPES_NS::Marshal& marshal, const SR_HTYPES_NS::DataStorage* dataStorage);

        SR_NODISCARD SR_HTYPES_NS::Marshal::Ptr Save(SR_HTYPES_NS::Marshal::Ptr pMarshal, SR_UTILS_NS::SavableFlags flags) const override;

        void UseMaterial() override;
        void UseModelMatrix() override;

        void UseSamplers() override;

        void OnLoaded() override;
        void OnAttached() override;
        void OnDestroy() override;
        void OnMatrixDirty() override;
        void OnEnable() override;
        void OnDisable() override;

        SR_NODISCARD bool ExecuteInEditMode() const override { return true; }
        SR_NODISCARD bool IsCanCalculate() const override;
        SR_NODISCARD SR_FORCE_INLINE bool IsCanUpdate() const noexcept override { return false; }
        SR_NODISCARD SR_FORCE_INLINE bool GetKerning() const noexcept { return m_kerning; }
        SR_NODISCARD SR_FORCE_INLINE bool IsDebugEnabled() const noexcept { return m_debug; }
        SR_NODISCARD SR_FORCE_INLINE bool IsPreprocessorEnabled() const noexcept { return m_preprocessor; }
        SR_NODISCARD SR_FORCE_INLINE bool IsLocalizationEnabled() const noexcept { return m_localization; }
        SR_NODISCARD SR_FORCE_INLINE Font* GetFont() const noexcept { return m_font; }
        SR_NODISCARD SR_FORCE_INLINE SR_MATH_NS::UVector2 GetFontSize() const noexcept { return m_fontSize; }

        SR_NODISCARD SR_FORCE_INLINE bool IsMeshActive() const noexcept override {
            return SR_UTILS_NS::Component::IsActive();
        }

        SR_NODISCARD Component* CopyComponent() const override;

        const SR_MATH_NS::Matrix4x4& GetModelMatrix() const override {
            return m_modelMatrix;
        }

        SR_NODISCARD uint32_t GetAtlasWidth() const noexcept { return m_width; }
        SR_NODISCARD uint32_t GetAtlasHeight() const noexcept { return m_height; }

        const SR_HTYPES_NS::UnicodeString& GetText() const { return m_text; }

        void SetText(const std::string& text);
        void SetText(const std::u16string& text);
        void SetText(const std::u32string& text);
        void SetKerning(bool enabled);
        void SetDebug(bool enabled);
        void SetFont(Font* pFont);
        void SetFontSize(const SR_MATH_NS::UVector2& size);

        void Draw() override;

        bool Calculate() override;
        void FreeVideoMemory() override;

    private:
        SR_NODISCARD RenderScenePtr GetRenderScene();
        SR_NODISCARD bool BuildAtlas();

    private:
        RenderScenePtr m_renderScene;
        Font* m_font = nullptr;

        SR_MATH_NS::Matrix4x4 m_modelMatrix = SR_MATH_NS::Matrix4x4::Identity();

        int32_t m_id = SR_ID_INVALID;
        uint32_t m_width = 0;
        uint32_t m_height = 0;

        SR_MATH_NS::UVector2 m_fontSize = SR_MATH_NS::UVector2(512, 512);

        bool m_kerning = true;
        bool m_debug = false;
        bool m_preprocessor = false;
        bool m_localization = false;

        SR_HTYPES_NS::UnicodeString m_text;

    };
}

#endif //SRENGINE_TEXT_H

//
// Created by Monika on 12.03.2022.
//

#ifndef SRENGINE_MARSHAL_H
#define SRENGINE_MARSHAL_H

#include <Utils/MarshalUtils.h>

#include <FileSystem/Path.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Utils/StringFormat.h>

namespace SR_UTILS_NS {
    class MarshalDecodeNode;
}

namespace std {
    template<> struct hash<Framework::Helper::MarshalDecodeNode> {
        size_t operator()(Framework::Helper::MarshalDecodeNode const& value) const;
    };
}

namespace SR_UTILS_NS {
    struct MarshalAttribute {
#if SR_MARSHAL_USE_LIST
        std::string m_name;
#endif
        MARSHAL_TYPE m_type;
        std::string m_data;
    };

    class MarshalEncodeNode {
        friend class MarshalDecodeNode;
    public:
        MarshalEncodeNode();

        explicit MarshalEncodeNode(const std::string& name);

        MarshalEncodeNode(MarshalEncodeNode&& marshal) noexcept {
            m_hasNodes = std::exchange(marshal.m_hasNodes, {});
            m_count = std::exchange(marshal.m_count, {});
            m_name = std::exchange(marshal.m_name, {});
            m_stream = std::exchange(marshal.m_stream, {});
        }

        MarshalEncodeNode& operator=(MarshalEncodeNode&& marshal) noexcept {
            m_hasNodes = std::exchange(marshal.m_hasNodes, {});
            m_count = std::exchange(marshal.m_count, {});
            m_name = std::exchange(marshal.m_name, {});
            m_stream = std::exchange(marshal.m_stream, {});

            return *this;
        }

    public:
        static MarshalEncodeNode Load(const Path& path);
        static MarshalEncodeNode LoadFromMemory(const std::string& data);

    public:
        [[nodiscard]] MarshalDecodeNode Decode() const;

        bool Save(const Path& path, MarshalSaveMode mode = MarshalSaveMode::Binary) const;
        SR_NODISCARD std::stringstream Save() const;
        SR_NODISCARD bool Empty() const { return m_count == 0; }
        SR_NODISCARD bool Valid() const { return !Empty() && m_stream.rdbuf()->in_avail() > 0; }
        SR_NODISCARD std::string ToString() const;

        template<typename T, typename U> MarshalEncodeNode& SR_FASTCALL AppendDef(const T& value, const U& def) {
            if constexpr (std::is_same<T, Math::FColor>()) {
                AppendDef("r", value.r, def);
                AppendDef("g", value.g, def);
                AppendDef("b", value.b, def);
                AppendDef("a", value.a, def);
            }
            else if constexpr (std::is_same<T, Math::IVector2>() || std::is_same<T, Math::FVector2>()) {
                AppendDef("x", value.x, def);
                AppendDef("y", value.y, def);
            }
            else if constexpr (std::is_same<T, Math::IVector3>() || std::is_same<T, Math::FVector3>()) {
                AppendDef("x", value.x, def);
                AppendDef("y", value.y, def);
                AppendDef("z", value.z, def);
            }
            else
                static_assert(false, "unknown type!");

            return *this;
        }

        template<typename T> MarshalEncodeNode& SR_FASTCALL Append(const T& value) {
            if constexpr (std::is_same<T, MarshalEncodeNode>()) {
                m_stream << value.Save().rdbuf(); /// move buffer
                ++m_count;
                m_hasNodes = true;
                return *this;
            }
            else if constexpr (std::is_same<T, Math::FColor>()) {
                Append("r", value.r);
                Append("g", value.g);
                Append("b", value.b);
                Append("a", value.a);
            }
            else if constexpr (std::is_same<T, Math::IVector2>() || std::is_same<T, Math::FVector2>()) {
                Append("x", value.x);
                Append("y", value.y);
            }
            else if constexpr (std::is_same<T, Math::IVector3>() || std::is_same<T, Math::FVector3>()) {
                Append("x", value.x);
                Append("y", value.y);
                Append("z", value.z);
            }
            else
                static_assert(false, "unknown type!");

            return *this;
        }

        template<typename T, typename U> MarshalEncodeNode& SR_FASTCALL AppendDef(const std::string& name, const T& value, const U& def) {
            if (value == def)
                return *this;

            return Append(name, value);
        }

        template<typename T> MarshalEncodeNode& SR_FASTCALL Append(const std::string& name, const T& value);

    private:
        std::string m_name;
        bool m_hasNodes = false;

        std::stringstream m_stream;
        volatile uint16_t m_count = SR_UINT16_MAX;

    };

    class MarshalDecodeNode {
        friend class MarshalEncodeNode;
    public:
        explicit MarshalDecodeNode(const char* name)
            : m_name(name)
        { }

        explicit MarshalDecodeNode(std::string name)
            : m_name(std::move(name))
        { }

        MarshalDecodeNode& operator=(MarshalDecodeNode&& marshal) noexcept {
            m_name = std::exchange(marshal.m_name, {});
            m_attributes = std::exchange(marshal.m_attributes, {});
            m_nodes = std::exchange(marshal.m_nodes, {});
            return *this;
        }

        MarshalDecodeNode& operator=(const MarshalDecodeNode& marshal) {
            m_name = marshal.m_name;
            m_attributes = marshal.m_attributes;
            m_nodes = marshal.m_nodes;
            return *this;
        }

        MarshalDecodeNode(const MarshalDecodeNode& marshal) {
            m_name = marshal.m_name;
            m_attributes = marshal.m_attributes;
            m_nodes = marshal.m_nodes;
        }

        MarshalDecodeNode(MarshalDecodeNode&& marshal) noexcept {
            m_name = std::exchange(marshal.m_name, {});
            m_attributes = std::exchange(marshal.m_attributes, {});
            m_nodes = std::exchange(marshal.m_nodes, {});
        }

        MarshalDecodeNode() = default;

        explicit operator bool() const {
            return Valid();
        }

    public:
        static MarshalDecodeNode Load(const Path& path);
        static MarshalDecodeNode LoadFromMemory(const std::string& data);

    public:
        SR_NODISCARD bool Empty() const { return m_attributes.empty() && m_nodes.empty(); }
        SR_NODISCARD bool Valid() const { return !Empty(); }

        [[nodiscard]] MarshalEncodeNode Encode() const;
        void Encode(std::stringstream& stream) const;

        MarshalDecodeNode& SR_FASTCALL AppendNode(MarshalDecodeNode&& node);

        template<typename T> MarshalDecodeNode& SR_FASTCALL Append(std::string name, T value) {
            constexpr auto type = MarshalUtils::TypeToMarshal<T>();

        #if SR_MARSHAL_USE_LIST
            if constexpr (Math::IsNumber<T>()) {
                m_attributes.emplace_back(MarshalAttribute { std::move(name), type, std::to_string(value) });
            }
            else {
                m_attributes.emplace_back(MarshalAttribute { std::move(name), type, std::move(value) });
            }
        #else
            if constexpr (Math::IsNumber<T>() || Math::IsLogical<T>()) {
                m_attributes[name] = MarshalAttribute { type, std::to_string(value) };
            }
            else {
                m_attributes[name] = MarshalAttribute { type, std::move(value) };
            }
        #endif

            return *this;
        }

        template<typename T> T GetAttribute() const;
        template<typename T, typename U> T SR_FASTCALL GetAttributeDef(const U& def) const;

        template<typename T> T SR_FASTCALL GetAttributeDef(const std::string& name, const T& def) const;
        template<typename T> T SR_FASTCALL GetAttribute(const std::string& name) const;

        [[nodiscard]] MarshalDecodeNode SR_FASTCALL TryGetNode(const std::string& name) const;

        [[nodiscard]] MarshalDecodeNode  TryGetNodeRef(const std::string& name) const {
            return GetNodeRef(name);
        }

        [[nodiscard]] const MarshalDecodeNode& GetNodeRef(const std::string& name) const;
        [[nodiscard]] MarshalDecodeNode SR_FASTCALL GetNode(const std::string& name) const;
        [[nodiscard]] std::list<MarshalDecodeNode> GetNodes() const;

        SR_NODISCARD std::string Name() const { return m_name; }

        SR_NODISCARD std::string Dump(uint32_t tab = 0) const;
        SR_NODISCARD std::string ToJson(uint32_t tab = 0, bool root = true) const;

        bool operator==(const MarshalDecodeNode& other) const {
            return m_name == other.m_name;
        }

    private:
        std::string m_name;

#if SR_MARSHAL_USE_LIST
        std::list<MarshalAttribute> m_attributes;
        std::list<MarshalDecodeNode> m_nodes;
#else
        std::unordered_map<std::string, MarshalAttribute> m_attributes;
        std::unordered_map<std::string, std::list<MarshalDecodeNode>> m_nodes;
#endif

    };

    class Marshal {
    public:
        Marshal(Marshal&& marshal) noexcept {
            m_stream = std::exchange(marshal.m_stream, {});
        }

        Marshal& operator=(Marshal&& marshal) noexcept {
            m_stream = std::exchange(marshal.m_stream, {});
            return *this;
        }

        Marshal& operator=(const Marshal& marshal) = delete;
        Marshal(const Marshal& marshal) = delete;

    public:
        template<typename T> void Write(const T& value) {
            if constexpr (Math::IsNumber<T>()) {
                MarshalUtils::SaveValue(m_stream, value);
            }
            else if constexpr (Math::IsString<T>()) {
                MarshalUtils::SaveString(m_stream, value);
            }
            else
                static_assert("Unsupported type!");
        }

        template<typename T> T Read() {
            if constexpr (Math::IsNumber<T>()) {
                return MarshalUtils::LoadValue<std::stringstream, T>(m_stream);
            }
            else if constexpr (Math::IsString<T>()) {
                return MarshalUtils::LoadStr<std::stringstream>(m_stream);
            }
            else
                static_assert("Unsupported type!");
        }

    private:
        std::stringstream m_stream;

    };
}

namespace SR_UTILS_NS {
    namespace MarshalUtils {
        template<typename Stream> MarshalDecodeNode SR_FASTCALL LoadNode(Stream &stream) {
            MarshalDecodeNode node(std::move(MarshalUtils::LoadShortStr<Stream>(stream)));

            const uint16_t count = MarshalUtils::LoadValue<Stream, uint16_t>(stream);

            for (uint16_t i = 0; i < count; ++i) {
                auto &&type = MarshalUtils::LoadValue<Stream, MARSHAL_TYPE>(stream);

                if (type == MARSHAL_TYPE::Node) {
                    node.AppendNode(std::move(LoadNode<Stream>(stream)));
                } 
                else {
                    std::string name = MarshalUtils::LoadShortStr<Stream>(stream);

                    switch (type) {
                        case MARSHAL_TYPE::Double:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, double_t>(stream));
                            break;
                        case MARSHAL_TYPE::String:
                            node.Append(std::move(name), MarshalUtils::LoadStr<Stream>(stream));
                            break;
                        case MARSHAL_TYPE::Bool:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, bool>(stream));
                            break;
                        case MARSHAL_TYPE::Int8:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, int8_t>(stream));
                            break;
                        case MARSHAL_TYPE::UInt8:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, uint8_t>(stream));
                            break;
                        case MARSHAL_TYPE::Int16:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, int16_t>(stream));
                            break;
                        case MARSHAL_TYPE::UInt16:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, uint16_t>(stream));
                            break;
                        case MARSHAL_TYPE::Int32:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, int32_t>(stream));
                            break;
                        case MARSHAL_TYPE::UInt32:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, uint32_t>(stream));
                            break;
                        case MARSHAL_TYPE::Int64:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, int64_t>(stream));
                            break;
                        case MARSHAL_TYPE::UInt64:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, uint64_t>(stream));
                            break;
                        case MARSHAL_TYPE::Float:
                            node.Append(std::move(name), MarshalUtils::LoadValue<Stream, float_t>(stream));
                            break;
                        default: {
                            SRAssert(false);
                            return MarshalDecodeNode();
                        }
                    }
                }
            }
            return node;
        }
    }

    template<typename T> inline T MarshalDecodeNode::GetAttribute() const {
        if constexpr (std::is_same<T, Math::IVector3>()) {
             return Math::IVector3(GetAttribute<int32_t>("x"), GetAttribute<int32_t>("y"), GetAttribute<int32_t>("z"));
        }
        else if constexpr (std::is_same<T, Math::IVector2>()) {
            return Math::IVector2(GetAttribute<int32_t>("x"), GetAttribute<int32_t>("y"));
        }
        else
            static_assert(false, "unknown type!");
    }

    template<typename T> inline T MarshalDecodeNode::GetAttribute(const std::string &name) const {
#if SR_MARSHAL_USE_LIST
        for (const auto& attribute : m_attributes) {
            if (attribute.m_name == name) {
                if constexpr (Math::IsNumber<T>()) {
                    return LexicalCast<T>(attribute.m_data);
                }
                else
                    return attribute.m_data;
            }
        }

        SRAssert2(false, "Attribute not found! Name: " + name);
        return T();
#else
        auto&& pIt = m_attributes.find(name);

        if (pIt == m_attributes.end()) {
            SRAssert2(false, "Attribute not found! Name: " + name);
            return T();
        }

        if constexpr (Math::IsNumber<T>()) {
            return LexicalCast<T>(pIt->second.m_data);
        }
        else {
            return pIt->second.m_data;
        }
#endif
    }

    template<typename T, typename U> inline T MarshalDecodeNode::GetAttributeDef(const U& def) const {
        if constexpr (std::is_same<T, Math::IVector3>()) {
            return Math::IVector3(
                    GetAttributeDef<int32_t>("x", def),
                    GetAttributeDef<int32_t>("y", def),
                    GetAttributeDef<int32_t>("z", def)
            );
        }
        else if constexpr (std::is_same<T, Math::FVector3>()) {
            return Math::FVector3(
                    GetAttributeDef<Math::Unit>("x", def),
                    GetAttributeDef<Math::Unit>("y", def),
                    GetAttributeDef<Math::Unit>("z", def)
            );
        }
        else if constexpr (std::is_same<T, Math::IVector2>()) {
            return Math::IVector2(GetAttributeDef<int32_t>("x", def), GetAttributeDef<int32_t>("y", def));
        }
        else
            static_assert(false, "unknown type!");
    }

    template<typename T> inline T MarshalDecodeNode::GetAttributeDef(const std::string& name, const T& def) const {
#if SR_MARSHAL_USE_LIST
        for (const auto& attribute : m_attributes) {
            if (attribute.m_name == name) {
                if constexpr (Math::IsNumber<T>()) {
                    return LexicalCast<T>(attribute.m_data);
                }
                else
                    return attribute.m_data;
            }
        }

        return def;
#else
        if (m_attributes.count(name) == 1)
            return GetAttribute<T>(name);

        return def;
#endif
    }

    template<typename T> inline MarshalEncodeNode &MarshalEncodeNode::Append(const std::string &name, const T &value) {
        if (m_hasNodes) {
            SRAssert2(false, "Attributes can be added before node adding!");
            return *this;
        }

        constexpr auto type = MarshalUtils::TypeToMarshal<T>();

        MarshalUtils::SaveValue(m_stream, type);
        MarshalUtils::SaveShortString(m_stream, name);

        if constexpr (Math::IsNumber<T>()) {
            MarshalUtils::SaveValue(m_stream, value);
        }
        else {
            MarshalUtils::SaveString(m_stream, value);
        }

        ++m_count;

        return *this;
    }
}

namespace Framework::RuntimeTest {
    bool MarshalRunRuntimeTest();
}

inline size_t std::hash<Framework::Helper::MarshalDecodeNode>::operator()(const Framework::Helper::MarshalDecodeNode &value) const {
    std::hash<std::string> h;
    return h(value.Name()) + 0x9e3779b9 + (0 << 6u) + (0 >> 2u);
}

#endif //SRENGINE_MARSHAL_H

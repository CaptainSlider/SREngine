//
// Created by Nikita on 27.11.2020.
//
#define GLM_ENABLE_EXPERIMENTAL

#include <Math/Mathematics.h>
#include "EntityComponentSystem/Transform.h"
#include <EntityComponentSystem/GameObject.h>
#include <map>
#include <vector>
#include <iostream>
#include <Debug.h>
#include <imgui.h>

Framework::Helper::Transform::Transform(Framework::Helper::GameObject *parent) {
    this->m_gameObject = parent;
}

void Framework::Helper::Transform::SetPosition(Vector3 val, bool pivot) {
    auto delta = m_globalPosition - val;
    m_globalPosition = val;
    m_gameObject->UpdateComponentsPosition();

    this->UpdateLocalPosition();

    for (auto a: m_gameObject->m_children)
        a->m_transform->UpdateChildPosition(delta, pivot);
}

void Framework::Helper::Transform::SetRotation(const Quaternion& val, bool pivot) {
    //glm::quat delta = glm::quat(glm::radians(m_globalRotation)) - glm::quat(glm::radians(val));
    m_globalRotation = val;

    //std::cout << glm::to_string(glm::degrees(glm::eulerAngles(delta))) << std::endl;

    m_gameObject->UpdateComponentsRotation();

    this->UpdateLocalRotation();

   // for (auto a: m_gameObject->m_children)
   //     a->m_transform->UpdateChildRotation(glm::vec3(0), pivot);
}

void Framework::Helper::Transform::SetScale(Vector3 val, bool pivot) {
    auto delta = m_globalScale / val;
    m_globalScale = val;

    m_gameObject->UpdateComponentsScale();

    auto temp = m_globalPosition;
    this->SetPosition(Vector3(0,0,0));

    this->UpdateLocalScale();
    this->UpdateLocalPosition();

    for (auto a: m_gameObject->m_children)
        a->m_transform->UpdateChildScale(delta, pivot);

    this->SetPosition(temp);
}

void Framework::Helper::Transform::Translate(Vector3 val) noexcept {
    //this->SetPosition(m_localPosition + val);
    this->SetPosition(m_globalPosition + val);
}

void Framework::Helper::Transform::Rotate(Vector3 angle) noexcept {
    if (angle == Vector3(0,0,0))
        return;

    //glm::quat q = this->m_globalRotation;
    //q = glm::quat(glm::radians(angle)) * q;

    //if (angle.x != 0)
    //   q = glm::rotate(q, glm::radians(angle.x), glm::vec3(1,0,0));
    //if (angle.y != 0)
    //    q = glm::rotate(q, glm::radians(angle.y), glm::vec3(0,1,0));
    //if (angle.z != 0)
    //    q = glm::rotate(q, glm::radians(angle.z), glm::vec3(0,0,1));

    //this->SetRotation(q);

    this->SetRotation(m_globalRotation.EulerAngle() + angle.Radians());
    //this->SetRotation(m_globalRotation * Quaternion(angle.Radians()));
   //Vector3 vec3 = Vector3::FromGLM(angle).Radians();
   //Vector3 euler = m_globalRotation.EulerAngle();

   //std::cout << (euler + vec3).Degrees().ToString() << std::endl;

   //this->SetRotation(euler + vec3);
    //this->SetRotation(m_localRotation + angle);
    //this->SetRotation(glm::degrees(glm::eulerAngles(
    //       m_globalRotation + glm::quat(glm::radians(angle))
    //        )));
   // this->SetRotation(glm::degrees(glm::eulerAngles(m_globalRotation)) + angle);
}

Framework::Helper::Math::Vector3 Framework::Helper::Transform::Forward(bool local) const noexcept {
    return m_globalRotation * forward;
}
Framework::Helper::Math::Vector3 Framework::Helper::Transform::Right(bool local) const noexcept {
    //return right.Rotate(m_globalRotation);
    return m_globalRotation * right;
}
Framework::Helper::Math::Vector3 Framework::Helper::Transform::Up(bool local) const noexcept {
    //return up.Rotate(m_globalRotation);
    return m_globalRotation * up;
}

//float Magnitude(glm::vec3 vec){
//    return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
//}

//glm::vec3 Framework::Helper::Transform::Direction(glm::vec3 point) noexcept {
    //glm::vec3 heading = point - m_globalPosition;
    //float distance = Magnitude(heading);
    //if (std::isnan(distance))
   //     return glm::vec3(0,0,0);
   // return heading / distance;
//}
void Framework::Helper::Transform::SetMatrix(glm::mat4 matrix, bool pivot) noexcept  {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;

    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

    auto euler = Quaternion(rotation).EulerAngle(false);

    //glm::vec3 deltaScale = scale / m_globalScale;
    Vector3 deltaScale = m_globalScale / scale;
    Vector3 deltaPosition = m_globalPosition - translation;
    //auto deltaRotate = m_globalRotation - euler;

    if (deltaScale != Vector3(1,1,1)) {
        this->SetPosition(Vector3(0,0,0));
        this->m_globalScale = scale;

        this->UpdateLocalScale();

        for (auto a : this->m_gameObject->m_children)
            a->m_transform->UpdateChildScale(deltaScale, pivot);
    }

    //if (!deltaRotate.Empty()) {
        this->m_globalRotation = euler;
        this->UpdateLocalRotation();
        for (auto a : this->m_gameObject->m_children)
            a->m_transform->UpdateChildRotation(Vector3(0,0,0), pivot);
    //}

    this->SetPosition(translation);
    //this->m_globalPosition = translation;

    this->UpdateLocalPosition();

    m_gameObject->UpdateComponents();

    //for (auto a : this->m_gameObject->m_children) {
    //    a->m_transform->UpdateChildPosition(deltaPosition, pivot);
    //}
}

glm::mat4 Framework::Helper::Transform::GetMatrix() const noexcept  {
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), m_globalPosition.ToGLM());

    //glm::vec3 v = checkVec3Zero(m_globalRotation.EulerAngle().Degrees().ToGLM());
    //modelMat *= mat4_cast(glm::quat(glm::radians(v)));

    glm::vec3 v = m_globalRotation.EulerAngle().ToGLM();
    modelMat *= mat4_cast(glm::quat(v));

    return glm::scale(modelMat, m_globalScale.ToGLM());

    //return glm::mat4(1);
}

void Framework::Helper::Transform::UpdateChildPosition(Vector3 delta, bool pivot) {
    m_globalPosition -= delta;

    this->UpdateLocalPosition();

    this->m_gameObject->UpdateComponentsPosition();

    for (auto a : m_gameObject->m_children)
        a->m_transform->UpdateChildPosition(delta, pivot);
}

void Framework::Helper::Transform::UpdateChildScale(Vector3 delta, bool pivot) {
    auto newScale = m_parent->m_globalScale * m_localScale;
    this->m_globalScale = newScale;

    this->UpdateLocalScale();

    this->m_globalPosition /= delta;

    this->m_gameObject->UpdateComponentsPosition();
    this->m_gameObject->UpdateComponentsScale();

    this->UpdateLocalPosition();

    for (auto a : m_gameObject->m_children)
        a->m_transform->UpdateChildScale(delta, pivot);
}

void Framework::Helper::Transform::UpdateChildRotation(Vector3 delta, bool pivot) {
    //glm::quat newRotate = glm::quat(glm::radians(m_parent->m_globalRotation)) * glm::quat(glm::radians(m_localRotation));
    //this->m_globalRotation = glm::degrees(glm::eulerAngles(newRotate));

    /*this->m_globalRotation = m_parent->m_globalRotation + m_localRotation;

    this->UpdateLocalRotation();

    this->m_gameObject->UpdateComponentsRotation();

    this->UpdateLocalPosition();

    for (auto a : m_gameObject->m_children)
        a->m_transform->UpdateChildRotation(delta, pivot);*/
}

void Framework::Helper::Transform::UpdateLocalPosition() {
    if (m_parent) {
        m_localPosition = (m_globalPosition - m_parent->m_globalPosition) / m_parent->m_globalScale;
        //glm::quat q = glm::quat(glm::radians(-m_parent->m_globalRotation));
        //m_localPosition = q * m_localPosition;

        /*glm::vec3 dir = this->Direction(m_parent->m_globalPosition);
        float dist = this->Distance(m_parent->m_globalPosition);

        glm::fquat q = glm::radians(-glm::vec3(
                m_parent->m_globalRotation.x,
                m_parent->m_globalRotation.y,
                m_parent->m_globalRotation.z
        ));
        m_localPosition = (q * dir); */

        //glm::vec3 point = m_parent->m_globalPosition - ((q * (dir * m_parent->Forward())) * dist);
        //glm::vec3 point = m_parent->m_globalPosition - (m_parent->Forward() * dist);

        //m_localPosition = q * forward;
        //m_localPosition = point / m_parent->m_globalScale;

        //this->m_localPosition = m_parent->Forward();

        //m_localPosition = {m_localPosition.x, m_localPosition.y, m_localPosition.z};
        //m_localPosition = {m_localPosition.x, -m_localPosition.y, -m_localPosition.z};
    } else
        m_localPosition = m_globalPosition;
}

void Framework::Helper::Transform::UpdateLocalScale() {
    if (m_parent) {
        this->m_localScale = m_globalScale / m_parent->m_globalScale;
    } else
        m_localScale = m_globalScale;
}

void Framework::Helper::Transform::UpdateLocalRotation() {
    if (m_parent) {
        this->m_localRotation = m_globalRotation - m_parent->m_globalRotation;
    } else
        m_localRotation = m_globalRotation;
}
#pragma once
#include <cstdint>
#include "offsets.h"

// Compatibility aliases for your cheat code
namespace Offsets {

    // ----- Humanoid -----
    inline constexpr uintptr_t WalkSpeed = offsets::Humanoid::Walkspeed;
    inline constexpr uintptr_t WalkSpeedCheck = offsets::Humanoid::WalkspeedCheck;
    inline constexpr uintptr_t Health = offsets::Humanoid::Health;
    inline constexpr uintptr_t MaxHealth = offsets::Humanoid::MaxHealth;
    inline constexpr uintptr_t JumpPower = offsets::Humanoid::JumpPower;

    // ----- Camera -----
    inline constexpr uintptr_t CameraSubject = offsets::Camera::CameraSubject;
    inline constexpr uintptr_t CameraType = offsets::Camera::CameraType;
    inline constexpr uintptr_t FieldOfView = offsets::Camera::FieldOfView;
    inline constexpr uintptr_t CameraPos = offsets::Camera::Position;
    inline constexpr uintptr_t CameraRotation = offsets::Camera::Rotation;

    // ----- Instance -----
    inline constexpr uintptr_t Name = offsets::Instance::Name;
    inline constexpr uintptr_t Parent = offsets::Instance::Parent;
    inline constexpr uintptr_t Children = offsets::Instance::ChildrenStart;
    inline constexpr uintptr_t ChildrenEnd = offsets::Instance::ChildrenEnd;
    inline constexpr uintptr_t ClassDescriptor = offsets::Instance::ClassDescriptor;

    // ----- Player -----
    inline constexpr uintptr_t LocalPlayer = offsets::Player::LocalPlayer;
    inline constexpr uintptr_t ModelInstance = offsets::Player::ModelInstance;

    // ----- Primitive / BasePart -----
    inline constexpr uintptr_t Position = offsets::Primitive::Position;
    inline constexpr uintptr_t Velocity = offsets::Primitive::AssemblyLinearVelocity;
    inline constexpr uintptr_t CanCollide = offsets::PrimitiveFlags::CanCollide;

    // ----- VisualEngine -----
    inline constexpr uintptr_t VisualEnginePointer = offsets::VisualEngine::Pointer;
    inline constexpr uintptr_t viewmatrix = offsets::VisualEngine::ViewMatrix;

    // ----- DataModel -----
    inline constexpr uintptr_t FakeDataModelPointer = offsets::FakeDataModel::Pointer;
    inline constexpr uintptr_t FakeDataModelToDataModel = offsets::FakeDataModel::RealDataModel;

    // ----- TaskScheduler -----
    inline constexpr uintptr_t TaskSchedulerPointer = offsets::TaskScheduler::Pointer;

    // ----- Misc -----
    inline constexpr uintptr_t StringLength = offsets::Misc::StringLength;

    // ----- Workspace -----
    inline constexpr uintptr_t CurrentCamera = offsets::Workspace::CurrentCamera;
}
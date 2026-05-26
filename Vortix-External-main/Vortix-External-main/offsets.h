#pragma once
#include <cstdint>
#include <string>
#include <iostream>

using uintptr_t = unsigned __int64;

namespace Offsets {
    inline std::string ClientVersion = "version-2b1721d47abf49aa";

    // Workspace / Camera
    inline constexpr uintptr_t Camera = 0x4b0;
    inline constexpr uintptr_t CameraPos = 0x11c;
    inline constexpr uintptr_t CameraRotation = 0xf8;
    inline constexpr uintptr_t CameraSubject = 0xe8;
    inline constexpr uintptr_t CameraType = 0x158;
    inline constexpr uintptr_t FieldOfView = 0x160;
    inline constexpr uintptr_t CurrentCamera = 0x4b0;

    // Humanoid
    inline constexpr uintptr_t Health = 0x194;
    inline constexpr uintptr_t MaxHealth = 0x1b4;
    inline constexpr uintptr_t WalkSpeed = 0x1dc;
    inline constexpr uintptr_t WalkSpeedCheck = 0x3c4;
    inline constexpr uintptr_t JumpPower = 0x1b0;
    inline constexpr uintptr_t AutoJumpEnabled = 0x1e0;
    inline constexpr uintptr_t AutoRotate = 0x1e1;
    inline constexpr uintptr_t PlatformStand = 0x1e8;
    inline constexpr uintptr_t Sit = 0x1e9;
    inline constexpr uintptr_t UseJumpPower = 0x1ec;
    inline constexpr uintptr_t HumanoidRootPart = 0x488;
    inline constexpr uintptr_t DisplayName = 0xd0;

    // Instance
    inline constexpr uintptr_t Name = 0xb0;
    inline constexpr uintptr_t Parent = 0x70;
    inline constexpr uintptr_t Children = 0x78;
    inline constexpr uintptr_t ChildrenEnd = 0x8;
    inline constexpr uintptr_t ClassDescriptor = 0x18;
    inline constexpr uintptr_t ClassName = 0x8;
    inline constexpr uintptr_t ClassDescriptorToClassName = 0x8;

    // Player
    inline constexpr uintptr_t LocalPlayer = 0x138;
    inline constexpr uintptr_t ModelInstance = 0x3a8;
    inline constexpr uintptr_t UserId = 0x2d8;
    inline constexpr uintptr_t AccountAge = 0x32c;
    inline constexpr uintptr_t Team = 0x2b0;
    inline constexpr uintptr_t TeamColor = 0x374;
    inline constexpr uintptr_t Mouse = 0x1178;

    // Primitive / BasePart
    inline constexpr uintptr_t Position = 0xec;
    inline constexpr uintptr_t Velocity = 0xf8;
    inline constexpr uintptr_t AssemblyLinearVelocity = 0xf8;
    inline constexpr uintptr_t AssemblyAngularVelocity = 0x104;
    inline constexpr uintptr_t CanCollide = 0x8;
    inline constexpr uintptr_t Anchored = 0x2;
    inline constexpr uintptr_t CanTouch = 0x10;
    inline constexpr uintptr_t CanQuery = 0x20;
    inline constexpr uintptr_t Size = 0x1b8;
    inline constexpr uintptr_t Rotation = 0xc8;
    inline constexpr uintptr_t Primitive = 0x148;
    inline constexpr uintptr_t Color3 = 0x194;
    inline constexpr uintptr_t Transparency = 0xf0;
    inline constexpr uintptr_t Reflectance = 0xec;
    inline constexpr uintptr_t CastShadow = 0xf5;
    inline constexpr uintptr_t Locked = 0xf6;

    // Visual Engine
    inline constexpr uintptr_t VisualEnginePointer = 0x7bd51f8;
    inline constexpr uintptr_t viewmatrix = 0x140;
    inline constexpr uintptr_t ViewMatrix = 0x140;
    inline constexpr uintptr_t Dimensions = 0xaa0;

    // DataModel / FakeDataModel
    inline constexpr uintptr_t FakeDataModelPointer = 0x74f6758;
    inline constexpr uintptr_t FakeDataModelToDataModel = 0x1d0;
    inline constexpr uintptr_t Workspace = 0x178;
    inline constexpr uintptr_t PlaceId = 0x1a0;
    inline constexpr uintptr_t GameId = 0x198;
    inline constexpr uintptr_t JobId = 0x138;
    inline constexpr uintptr_t ScriptContext = 0x440;

    // TaskScheduler
    inline constexpr uintptr_t TaskSchedulerPointer = 0x7bfe988;

    // World
    inline constexpr uintptr_t Gravity = 0x210;

    // Lighting
    inline constexpr uintptr_t Brightness = 0x128;
    inline constexpr uintptr_t ClockTime = 0x1c0;
    inline constexpr uintptr_t FogEnd = 0x13c;
    inline constexpr uintptr_t FogStart = 0x140;

    // ========== MISSING MEMBERS YOUR CODE NEEDS ==========
    inline constexpr uintptr_t StringLength = 0x10;
    inline constexpr uintptr_t Value = 0xd0;
    inline constexpr uintptr_t Adornee = 0x108;

    // Dummy fetch function
    inline bool fetchOffsets() {
        std::cout << "Using static offsets for " << ClientVersion << std::endl;
        return true;
    }
}
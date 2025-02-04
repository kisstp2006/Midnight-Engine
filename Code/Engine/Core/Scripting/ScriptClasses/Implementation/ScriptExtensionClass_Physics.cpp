#include <Core/CorePCH.h>

#include <Core/Interfaces/PhysicsWorldModule.h>
#include <Core/Scripting/ScriptAttributes.h>
#include <Core/Scripting/ScriptClasses/ScriptExtensionClass_Physics.h>

// clang-format off
EZ_BEGIN_STATIC_REFLECTED_TYPE(ezScriptExtensionClass_Physics, ezNoBase, 1, ezRTTINoAllocator)
{
  EZ_BEGIN_FUNCTIONS
  {
    EZ_SCRIPT_FUNCTION_PROPERTY(GetGravity, In, "World"),
    EZ_SCRIPT_FUNCTION_PROPERTY(Raycast, Out, "HitPosition", Out, "HitNormal", Out, "HitObject", In, "World", In, "Start", In, "Direction", In, "Distance", In, "CollisionLayer", In, "ShapeTypes", In, "IgnoreObjectID")->AddAttributes(
      new ezFunctionArgumentAttributes(7, new ezDynamicEnumAttribute("PhysicsCollisionLayer")),
      new ezFunctionArgumentAttributes(8, new ezDefaultValueAttribute((ezInt32)ezPhysicsShapeType::Static | (ezInt32)ezPhysicsShapeType::Dynamic)),
      new ezFunctionArgumentAttributes(9, new ezDefaultValueAttribute((ezInt32)ezInvalidIndex))),

    EZ_SCRIPT_FUNCTION_PROPERTY(OverlapTestSphere, In, "World", In, "Radius", In, "Position", In, "CollisionLayer", In, "ShapeTypes")->AddAttributes(
      new ezFunctionArgumentAttributes(3, new ezDynamicEnumAttribute("PhysicsCollisionLayer")),
      new ezFunctionArgumentAttributes(4, new ezDefaultValueAttribute((ezInt32)ezPhysicsShapeType::Static | (ezInt32)ezPhysicsShapeType::Dynamic))),

    EZ_SCRIPT_FUNCTION_PROPERTY(OverlapTestCapsule, In, "World", In, "Radius", In, "Height", In, "Transform", In, "CollisionLayer", In, "ShapeTypes")->AddAttributes(
      new ezFunctionArgumentAttributes(4, new ezDynamicEnumAttribute("PhysicsCollisionLayer")),
      new ezFunctionArgumentAttributes(5, new ezDefaultValueAttribute((ezInt32)ezPhysicsShapeType::Static | (ezInt32)ezPhysicsShapeType::Dynamic))),

    EZ_SCRIPT_FUNCTION_PROPERTY(SweepTestSphere, Out, "HitPosition", Out, "HitNormal", Out, "HitObject", In, "World", In, "Radius", In, "Start", In, "Direction", In, "Distance", In, "CollisionLayer", In, "ShapeTypes")->AddAttributes(
      new ezFunctionArgumentAttributes(8, new ezDynamicEnumAttribute("PhysicsCollisionLayer")),
      new ezFunctionArgumentAttributes(9, new ezDefaultValueAttribute((ezInt32)ezPhysicsShapeType::Static | (ezInt32)ezPhysicsShapeType::Dynamic))),

    EZ_SCRIPT_FUNCTION_PROPERTY(SweepTestCapsule, Out, "HitPosition", Out, "HitNormal", Out, "HitObject", In, "World", In, "Radius", In, "Height", In, "Start", In, "Direction", In, "Distance", In, "CollisionLayer", In, "ShapeTypes")->AddAttributes(
      new ezFunctionArgumentAttributes(9, new ezDynamicEnumAttribute("PhysicsCollisionLayer")),
      new ezFunctionArgumentAttributes(10, new ezDefaultValueAttribute((ezInt32)ezPhysicsShapeType::Static | (ezInt32)ezPhysicsShapeType::Dynamic))),
  }
  EZ_END_FUNCTIONS;
  EZ_BEGIN_ATTRIBUTES
  {
    new ezScriptExtensionAttribute("Physics"),
  }
  EZ_END_ATTRIBUTES;
}
EZ_END_STATIC_REFLECTED_TYPE;
// clang-format on

ezVec3 ezScriptExtensionClass_Physics::GetGravity(ezWorld* pWorld)
{
  if (auto pModule = pWorld->GetModuleReadOnly<ezPhysicsWorldModuleInterface>())
  {
    return pModule->GetGravity();
  }

  return ezVec3::MakeZero();
}

bool ezScriptExtensionClass_Physics::Raycast(ezVec3& out_vHitPosition, ezVec3& out_vHitNormal, ezGameObjectHandle& out_hHitObject, ezWorld* pWorld, const ezVec3& vStart, const ezVec3& vDirection, float fDistance, ezUInt8 uiCollisionLayer, ezBitflags<ezPhysicsShapeType> shapeTypes /*= ezPhysicsShapeType::Static | ezPhysicsShapeType::Dynamic*/, ezUInt32 uiIgnoreObjectID)
{
  if (auto pModule = pWorld->GetModuleReadOnly<ezPhysicsWorldModuleInterface>())
  {
    ezPhysicsCastResult res;
    ezPhysicsQueryParameters params;
    params.m_ShapeTypes = shapeTypes;
    params.m_uiCollisionLayer = uiCollisionLayer;
    params.m_uiIgnoreObjectFilterID = uiIgnoreObjectID;
    params.m_bIgnoreInitialOverlap = true;

    if (pModule->Raycast(res, vStart, vDirection, fDistance, params))
    {
      // res.m_hSurface
      out_vHitPosition = res.m_vPosition;
      out_vHitNormal = res.m_vNormal;
      out_hHitObject = res.m_hActorObject;
      return true;
    }
  }

  return false;
}

bool ezScriptExtensionClass_Physics::OverlapTestSphere(ezWorld* pWorld, float fRadius, const ezVec3& vPosition, ezUInt8 uiCollisionLayer, ezBitflags<ezPhysicsShapeType> shapeTypes /*= ezPhysicsShapeType::Static | ezPhysicsShapeType::Dynamic*/)
{
  if (auto pModule = pWorld->GetModuleReadOnly<ezPhysicsWorldModuleInterface>())
  {
    ezPhysicsQueryParameters params;
    params.m_ShapeTypes = shapeTypes;
    params.m_uiCollisionLayer = uiCollisionLayer;

    return pModule->OverlapTestSphere(fRadius, vPosition, params);
  }

  return false;
}

bool ezScriptExtensionClass_Physics::OverlapTestCapsule(ezWorld* pWorld, float fRadius, float fHeight, const ezTransform& transform, ezUInt8 uiCollisionLayer, ezBitflags<ezPhysicsShapeType> shapeTypes /*= ezPhysicsShapeType::Static | ezPhysicsShapeType::Dynamic*/)
{
  if (auto pModule = pWorld->GetModuleReadOnly<ezPhysicsWorldModuleInterface>())
  {
    ezPhysicsQueryParameters params;
    params.m_ShapeTypes = shapeTypes;
    params.m_uiCollisionLayer = uiCollisionLayer;

    return pModule->OverlapTestCapsule(fRadius, fHeight, transform, params);
  }
  return false;
}

bool ezScriptExtensionClass_Physics::SweepTestSphere(ezVec3& out_vHitPosition, ezVec3& out_vHitNormal, ezGameObjectHandle& out_hHitObject, ezWorld* pWorld, float fRadius, const ezVec3& vStart, const ezVec3& vDirection, float fDistance, ezUInt8 uiCollisionLayer, ezBitflags<ezPhysicsShapeType> shapeTypes /*= ezPhysicsShapeType::Static | ezPhysicsShapeType::Dynamic*/)
{
  if (auto pModule = pWorld->GetModuleReadOnly<ezPhysicsWorldModuleInterface>())
  {
    ezPhysicsCastResult res;
    ezPhysicsQueryParameters params;
    params.m_ShapeTypes = shapeTypes;
    params.m_uiCollisionLayer = uiCollisionLayer;

    if (pModule->SweepTestSphere(res, fRadius, vStart, vDirection, fDistance, params))
    {
      out_vHitPosition = res.m_vPosition;
      out_vHitNormal = res.m_vNormal;
      out_hHitObject = res.m_hActorObject;
      return true;
    }
  }
  return false;
}

bool ezScriptExtensionClass_Physics::SweepTestCapsule(ezVec3& out_vHitPosition, ezVec3& out_vHitNormal, ezGameObjectHandle& out_hHitObject, ezWorld* pWorld, float fRadius, float fHeight, const ezTransform& start, const ezVec3& vDirection, float fDistance, ezUInt8 uiCollisionLayer, ezBitflags<ezPhysicsShapeType> shapeTypes /*= ezPhysicsShapeType::Static | ezPhysicsShapeType::Dynamic*/)
{
  if (auto pModule = pWorld->GetModuleReadOnly<ezPhysicsWorldModuleInterface>())
  {
    ezPhysicsCastResult res;
    ezPhysicsQueryParameters params;
    params.m_ShapeTypes = shapeTypes;
    params.m_uiCollisionLayer = uiCollisionLayer;

    if (pModule->SweepTestCapsule(res, fRadius, fHeight, start, vDirection, fDistance, params))
    {
      out_vHitPosition = res.m_vPosition;
      out_vHitNormal = res.m_vNormal;
      out_hHitObject = res.m_hActorObject;
      return true;
    }
  }
  return false;
}

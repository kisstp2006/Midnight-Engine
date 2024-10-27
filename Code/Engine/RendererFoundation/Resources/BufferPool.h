#pragma once

class ezGALDevice;

class EZ_RENDERERFOUNDATION_DLL ezGALBufferPool
{
public:
  ezGALBufferPool() = default;
  ~ezGALBufferPool();

  void Initialize(const ezGALBufferCreationDescription& desc, ezStringView sDebugName);
  void Deinitialize();
  bool IsInitialized() const { return m_EventSubscriptionID != 0;}

  ezGALBufferHandle GetNewBuffer() const;
  ezGALBufferHandle GetCurrentBuffer() const;

private:
  void GALStaticDeviceEventHandler(const ezGALDeviceEvent& e);

  EZ_DISALLOW_COPY_AND_ASSIGN(ezGALBufferPool);
  ezGALBufferCreationDescription m_Desc;
  ezString m_sDebugName;
  ezEventSubscriptionID m_EventSubscriptionID = {};
  mutable ezHybridArray<ezGALBufferHandle, 1> m_UsedBuffers;
  mutable ezHybridArray<ezGALBufferHandle, 1> m_FreeBuffers;
};